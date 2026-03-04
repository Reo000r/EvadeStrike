#include "Application.h"
#include "Input.h"
#include "Scene/SceneController.h"
#include "Statistics.h"
#include "DebugDraw.h"
#include "Effect/EffectManager.h"
#include "SoundManager.h"
#include "Scene/ResultDataHolder.h"
#include "Library/System/UI/UIManager.h"
#include "Library/System/UI/UIButtonManager.h"
#include <resource.h>

#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <cassert>
#include <string>

Application& Application::GetInstance()
{
	// 初実行時にメモリ確保
	static Application app;
	return app;
}

bool Application::Init()
{
#ifdef _DEBUG
	AllocConsole();							// コンソール
	_out = 0;
	freopen_s(&_out, "CON", "w", stdout);	// stdout
	_in = 0;
	freopen_s(&_in, "CON", "r", stdin);		// stdin
#endif	// _DEBUG

	// ウィンドウテキストを指定
	SetWindowText(L"EvadeStrike");
	SetMainWindowText(L"EvadeStrike");
	// アイコンを指定
	SetWindowIconID(IDI_ICON1);
	// DirectX11を使用する
	// Effekseerの為
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	// スクリーン初期化処理
	SetGraphMode(Statistics::kScreenWidth, Statistics::kScreenHeight, 32);
	ChangeWindowMode(true);

	// DxLibの初期化処理
	if (DxLib_Init()) {
#ifdef _DEBUG
		printf("DxLib初期化失敗\n");
		assert(false && "DxLib_Init_Error");
#endif	// _DEBUG
		return false;
	}

#ifdef _DEBUG
	printf("DxLib初期化成功\n");
#endif	// _DEBUG

	SetDrawScreen(DX_SCREEN_BACK);

	// 3D関連設定
	SetUseZBuffer3D(true);		// Zバッファ有効化
	SetWriteZBuffer3D(true);	// Zバッファへの書き込みを行う
	SetUseBackCulling(true);	// バックカリングを有効にする

	// デフォルトの入力種別を設定
	Input::GetInstance().SetInputType(Input::PeripheralType::pad1);

	SoundManager::GetInstance().Init();

	// GetRandシード設定
	unsigned int t = static_cast<unsigned int>(time(nullptr));
	SRand(t);

	return true;
}

void Application::Run()
{
	// シングルトンオブジェクトを取得
	SceneController& sceneController = SceneController::GetInstance();
	EffectManager& effectManager = EffectManager::GetInstance();
	Input& input = Input::GetInstance();
	DebugDraw& debugDraw = DebugDraw::GetInstance();

	bool continueLoop = true;
	continueLoop = effectManager.Init();

#ifdef _DEBUG
	printf("\n---ゲームループ開始---\n\n");
#endif	// _DEBUG

	// ループ終了が要求されるまで実行
	while (continueLoop) {
		// 問題が起こっていればfalseになる
		continueLoop = (ProcessMessage() != -1);
		// 今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		ClearDrawScreen();

		// 入力更新
		input.Update();

		if (!CanUpdate()) {
			// 指定のFPSに固定する
			while (GetNowHiPerformanceCount() - time < Statistics::kStopTime) {
			}
			continue;
		}

#ifdef _DEBUG
		// デバッグ描画情報を初期化
		debugDraw.Clear();
#endif // _DEBUG

		// ゲーム部分
		sceneController.Update();
		effectManager.Update();
		UIManager::GetInstance().Update();
		// UIButtonManagerのUpdateは各シーンの都合に合わせる

		// シャドウマップへの描画
		sceneController.Draw();
		effectManager.Draw();

		// リザルト用の画像を取得
		ResultDataHolder::GetInstance().CopyResultScreen();

#ifdef _DEBUG
		// デバッグ描画
		debugDraw.Draw();
#endif // _DEBUG

		ScreenFlip();

#ifdef _DEBUG
		// 終了キーが押されたら
		if (input.IsPress("Debug:Exit1") && input.IsPress("Debug:Exit2")) {
			continueLoop = false;	// 処理を抜ける
		}
#endif // _DEBUG

		// FPS60に固定する
		while (GetNowHiPerformanceCount() - time < Statistics::kStopTime) {
		}
	}
}

void Application::Terminate()
{
	EffectManager::GetInstance().Terminate();
	SoundManager::GetInstance().Terminate();

	Effkseer_End();
	DxLib_End();
#ifdef _DEBUG
	fclose(_out); fclose(_in); FreeConsole();//コンソール解放
#endif	// _DEBUG
}

bool Application::CanUpdate()
{
	Input& input = Input::GetInstance();

	// ポーズ中なら
	if (_isPause) {
		// ポーズボタンが押されたら
		if (input.IsTrigger("Debug:Pause")) {
			// ポーズを解除
			_isPause = false;
			return true;
		}
		// 1fだけ進めてよいなら
		if (input.IsTrigger("Debug:UpdateMoment")) {
			return true;
		}
		return false;
	}
	// ポーズ中でなければ
	else {
		// ポーズボタンが押されたら
		if (input.IsTrigger("Debug:Pause")) {
			// ポーズ中にする
			_isPause = true;
			return false;
		}
		return true;
	}
}

#include "SceneTitle.h"
#include "SceneGamePlay.h"
#include "SceneTutorial.h"
#include "SceneController.h"
#include "Library/Physics/Physics.h"
#include "Library/System/SoundManager.h"
#include "Library/System/TutorialFlagHolder.h"

#include "Library/System/Input.h"
#include "Library/Geometry/Vector2.h"
#include "Library/System/Statistics.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Effect/EffectManager.h"

#include "Library/System/UI/UIButtonManager.h"
#include "Library/System/UI/UIButton.h"
#include "Library/System/UI/UIManager.h"
#include "ResultDataHolder.h"

#include <DxLib.h>
#include <cassert>
#include <functional>
#include <EffekseerForDXLib.h>

SceneTitle::SceneTitle() :
	_frame(Statistics::kFadeInterval),
	_nowUpdateState(&SceneTitle::FadeinUpdate),
	_nowDrawState(&SceneTitle::FadeDraw),
	_nextSceneName(NextSceneName::GamePlay),
	_tutorialShowSkipPopup(false),
	_nextScene(nullptr),
	_movieHandle(-1)
{
}

SceneTitle::~SceneTitle()
{
	if (_movieHandle != -1) {
		DeleteGraph(_movieHandle);
		_movieHandle = -1;
	}
}

void SceneTitle::Init()
{
	SoundManager::GetInstance().PlaySoundType(BGMType::Title);
	UIManager::GetInstance().Init();
	UIButtonManager::GetInstance().Init();
	ResultDataHolder::GetInstance().Init();

	int playHandle = LoadGraph(L"Data/Graph/EvadeStrike_Play.png");
	auto start = std::make_shared<UIButton>(
		playHandle,
		Vector2(Statistics::kScreenWidth * 0.5f,
			Statistics::kScreenHeight * 0.7f),
		0.2f * Statistics::kScreenWidthFullHDRatio,
		[this]() {
			// アプリケーション起動後、初めてゲーム開始が押された場合は
			// チュートリアルへ遷移し、スキップ確認ポップアップを表示する
			if (!TutorialFlagHolder::GetInstance().HasStartedGameOnce()) {
				TutorialFlagHolder::GetInstance().MarkGameStarted();
				_nextSceneName = NextSceneName::Tutorial;
				_tutorialShowSkipPopup = true;
			}
			else {
				_nextSceneName = NextSceneName::GamePlay;
			}
			_nowUpdateState = &SceneTitle::FadeoutUpdate;
			_nowDrawState = &SceneTitle::FadeDraw;
			_frame = 0;
		}
	);
	// チュートリアルボタン(押下時はスキップ確認ポップアップを表示しない)
	int tutorialHandle = LoadGraph(L"Data/Graph/EvadeStrike_Tutorial.png");
	auto tutorial = std::make_shared<UIButton>(
		tutorialHandle,
		Vector2(Statistics::kScreenWidth * 0.5f,
			Statistics::kScreenHeight * 0.79f),
		0.12f * Statistics::kScreenWidthFullHDRatio,
		[this]() {
			_nextSceneName = NextSceneName::Tutorial;
			_tutorialShowSkipPopup = false;
			_nowUpdateState = &SceneTitle::FadeoutUpdate;
			_nowDrawState = &SceneTitle::FadeDraw;
			_frame = 0;
		}
	);
	int exitHandle = LoadGraph(L"Data/Graph/EvadeStrike_Exit.png");
	auto exit = std::make_shared<UIButton>(
		exitHandle,
		Vector2(Statistics::kScreenWidth * 0.5f,
			Statistics::kScreenHeight * 0.9f),
		0.2f * Statistics::kScreenWidthFullHDRatio,
		[]() {
			// 終了処理
			EffectManager::GetInstance().Terminate();
			SoundManager::GetInstance().Terminate();
			Effkseer_End();
			DxLib_End();
			return;
		}
	);
	// ロゴ
	int logoHandle = LoadGraph(L"Data/Graph/EvadeStrike_Logo_Title.png");
	auto logo = std::make_shared<UIButton>(
		logoHandle,
		Vector2(Statistics::kScreenWidth * 0.5f, 
			Statistics::kScreenHeight * 0.3f),
		0.8f * Statistics::kScreenWidthFullHDRatio,
		[](){}
	);
	// 背景
	int backHandle = LoadGraph(L"Data/Graph/EvadeStrike_Logo_Back.png");
	auto logoback = std::make_shared<UIButton>(
		backHandle,
		Vector2(Statistics::kScreenWidth * 0.5f, 
			Statistics::kScreenHeight * 0.3f),
		0.8f * Statistics::kScreenWidthFullHDRatio,
		[](){}
	);

	start->SetDown(tutorial);
	tutorial->SetUp(start);
	tutorial->SetDown(exit);
	exit->SetUp(tutorial);
	
	start->SetSelected(true);

	logo->SetSelectable(false);
	logo->SetAnimType(UIAnimType::None);
	logoback->SetSelectable(false);
	logoback->SetAnimSpeed(1.5f);
	logoback->SetAnimType(UIAnimType::Rotate);

	UIButtonManager::GetInstance().AddButton(start);
	UIButtonManager::GetInstance().AddButton(tutorial);
	UIButtonManager::GetInstance().AddButton(exit);
	UIButtonManager::GetInstance().AddButton(logoback);
	UIButtonManager::GetInstance().AddButton(logo);


	// 動画ファイルをグラフィックハンドルとして開く
	_movieHandle = OpenMovieToGraph(L"Data/Movie/Title_Background.mp4", TRUE);
	if (_movieHandle != -1) {
		// ループ再生とミュート
		PlayMovieToGraph(_movieHandle, DX_PLAYTYPE_LOOP);
		SetMovieVolumeToGraph(0, _movieHandle);
	}
	else {
#ifdef _DEBUG
		printf("動画ファイルが見つからない");
#endif
	}
}

void SceneTitle::Update()
{
	(this->*_nowUpdateState)();
}

void SceneTitle::Draw() const
{
	(this->*_nowDrawState)();
}

void SceneTitle::FadeinUpdate()
{
	_frame--;
	if (_frame <= 0) {
		_nowUpdateState = &SceneTitle::NormalUpdate;
		_nowDrawState = &SceneTitle::NormalDraw;
	}
}

void SceneTitle::NormalUpdate()
{
	// 決定を押したら
	if (Input::GetInstance().IsPress("Debug:NextScene1") &&
		Input::GetInstance().IsPress("Debug:NextScene2")) {
		_nextSceneName = NextSceneName::GamePlay;
		_nowUpdateState = &SceneTitle::FadeoutUpdate;
		_nowDrawState = &SceneTitle::FadeDraw;
		_frame = 0;
	}

	UIButtonManager::GetInstance().Update();
	std::function<void()> action = UIButtonManager::GetInstance().FetchAction();
	// ボタンが押されていたら
	if (action != nullptr) {
		// 登録した行動を起こす
		action();
		return;
	}
}

void SceneTitle::FadeoutUpdate()
{
	_frame++;

	if (_frame >= Statistics::kFadeInterval) {
		if (_nextSceneName == NextSceneName::GamePlay) {
			EffectManager::GetInstance().DeleteAllEffect();
			_nextScene = std::make_shared<SceneGamePlay>();
		}
		else if (_nextSceneName == NextSceneName::Tutorial) {
			EffectManager::GetInstance().DeleteAllEffect();
			_nextScene = std::make_shared<SceneTutorial>(_tutorialShowSkipPopup);
		}
		else {
			assert(false && "次のシーンが不明");
			return;
		}

		if (!_nextScene) {
			assert(false && "次のシーンが不明");
			return;
		}
		SceneController::GetInstance().ChangeScene(_nextScene);
		return;
	}
}

void SceneTitle::FadeDraw() const
{
	DrawBackgroundMovie();

	UIManager::GetInstance().Draw();
	UIButtonManager::GetInstance().Draw();

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene Title");
#endif

	// フェードイン/アウトの処理
	// フェード割合の計算(0.0-1.0)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneTitle::NormalDraw() const
{
	DrawBackgroundMovie();

	UIManager::GetInstance().Draw();
	UIButtonManager::GetInstance().Draw();

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene Title");
#endif
}

void SceneTitle::DrawBackgroundMovie() const
{
	// 背景動画を描画
	if (_movieHandle != -1) {
		// 動画の元サイズを取得
		int movieW, movieH;
		GetGraphSize(_movieHandle, &movieW, &movieH);

		// 画面のアスペクト比
		float screenAspect = static_cast<float>(Statistics::kScreenWidth)
			/ static_cast<float>(Statistics::kScreenHeight);
		float movieAspect = static_cast<float>(movieW) / static_cast<float>(movieH);

		int drawX, drawY, drawW, drawH;

		if (movieAspect > screenAspect) {
			// 動画が画面より横長の場合は高さに合わせると幅がはみ出る
			drawH = Statistics::kScreenHeight;
			drawW = static_cast<int>(drawH * movieAspect);
			drawX = (Statistics::kScreenWidth - drawW) / 2;
			drawY = 0;
		}
		else {
			// 動画が画面より縦長の場合は幅に合わせると高さがはみ出る
			drawW = Statistics::kScreenWidth;
			drawH = static_cast<int>(drawW / movieAspect);
			drawX = 0;
			drawY = (Statistics::kScreenHeight - drawH) / 2;
		}

		// はみ出た部分は自動的にクリップされる
		DrawExtendGraph(drawX, drawY, drawX + drawW, drawY + drawH, _movieHandle, FALSE);
		
		constexpr float kAlphaRate = 0.5f;
		SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * kAlphaRate));
		DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
		// BlendModeを使った後はNOBLENDにしておくことを忘れず
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}


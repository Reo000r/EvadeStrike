#include "SceneTitle.h"
#include "SceneGamePlay.h"
#include "SceneController.h"
#include "Library/Physics/Physics.h"
#include "Library/System/SoundManager.h"

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
	_nextScene(nullptr)
{
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
			Statistics::kScreenHeight * 0.8f),
		0.2f * Statistics::kScreenWidthFullHDRatio,
		[this]() {
			_nextSceneName = NextSceneName::GamePlay;
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

	start->SetDown(exit);
	exit->SetUp(start);
	
	start->SetSelected(true);

	logo->SetSelectable(false);
	logo->SetAnimType(UIAnimType::None);
	logoback->SetSelectable(false);
	logoback->SetAnimSpeed(1.5f);
	logoback->SetAnimType(UIAnimType::Rotate);

	UIButtonManager::GetInstance().AddButton(start);
	UIButtonManager::GetInstance().AddButton(exit);
	UIButtonManager::GetInstance().AddButton(logoback);
	UIButtonManager::GetInstance().AddButton(logo);
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
	UIManager::GetInstance().Draw();
	UIButtonManager::GetInstance().Draw();

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene Title");
#endif
}


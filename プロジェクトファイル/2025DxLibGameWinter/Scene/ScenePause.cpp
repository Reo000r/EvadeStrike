#include "ScenePause.h"
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

ScenePause::ScenePause() :
	_isPauseEnd(false)
{
}

void ScenePause::Init()
{
	// もし既存のシーンにボタンがある場合は変更が必要

	int playHandle = LoadGraph(L"Data/Graph/EvadeStrike_Play.png");
	auto resume = std::make_shared<UIButton>(
		playHandle,
		Vector2(Statistics::kScreenWidth * 0.5f,
			Statistics::kScreenHeight * 0.8f),
		0.2f * Statistics::kScreenWidthFullHDRatio,
		[this]() {
			_isPauseEnd = true;
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

	resume->SetDown(exit);
	exit->SetUp(resume);

	resume->SetSelected(true);

	UIButtonManager::GetInstance().AddButton(resume);
	UIButtonManager::GetInstance().AddButton(exit);
}

void ScenePause::Update()
{
	if (Input::GetInstance().IsTrigger("Debug:PauseScene") ||
		_isPauseEnd) {
		// ここでボタンをすべて消しているが、
		// 大元のシーンにボタンがある場合それも消えてしまう
		UIButtonManager::GetInstance().Init();
		SceneController::GetInstance().PopScene();
		return;
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

void ScenePause::Draw() const
{
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * 0.5f));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 大元のシーンがボタンを使用していた場合、
	// このDrawで大元のシーンのボタンが上に出てきてしまう
	UIButtonManager::GetInstance().Draw();

	DrawFormatString(
		Statistics::kScreenCenterWidth,
		Statistics::kScreenCenterHeight, 
		0xffffff, L"Scene Pause");

#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, L"Scene Pause");
#endif
}

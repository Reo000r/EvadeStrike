#include "SceneGamePlay.h"
#include "SceneResult.h"
#include "Library/System/Event/EventManager.h"
#include "Objects/Character/Player/Player.h"
#include "ResultDataHolder.h"

#include "Library/System/Input.h"
#include "Library/System/TimeScaleManager.h"

#include <DxLib.h>

namespace {
	const std::string kStageCsvPath = "Data/CSV/StagePlaceData.csv";
}

SceneGamePlay::SceneGamePlay() :
	SceneGameBase()
{
}

std::string SceneGamePlay::GetStageCsvPath() const
{
	return kStageCsvPath;
}

BGMType SceneGamePlay::GetBGMType() const
{
	return BGMType::GamePlay;
}

void SceneGamePlay::OnFadeInComplete()
{
	// リザルト画面の背景を取得しておく
	ResultDataHolder::GetInstance().ReserveCopyResultScreen();
}

void SceneGamePlay::OnAdditionalUpdate()
{
#ifdef _DEBUG
	// 速度変更
	TimeScaleManager& manager = TimeScaleManager::GetInstance();
	float afterScale = 0.0f;
	if (Input::GetInstance().IsTrigger("Debug:Key1")) {
		afterScale = manager.GetPlayerCurrentScale() + 0.2f;
		manager.ChangeScale(afterScale);
		printf("Scale加算:%.2f\n", afterScale);
	}
	if (Input::GetInstance().IsTrigger("Debug:Key2")) {
		afterScale = manager.GetPlayerCurrentScale() - 0.2f;
		manager.ChangeScale(afterScale);
		printf("Scale減算:%.2f\n", afterScale);
	}
	if (Input::GetInstance().IsTrigger("Debug:Key3")) {
		afterScale = 1.0f;
		manager.ChangeScale(afterScale);
		printf("Scaleリセット:%.2f\n", afterScale);
	}
#endif // _DEBUG
}

bool SceneGamePlay::CheckSceneCompleteCondition() const
{
#ifdef _DEBUG
	if (Input::GetInstance().IsPress("Debug:NextScene1") &&
		Input::GetInstance().IsPress("Debug:NextScene2")) {
		return true;
	}
#endif // _DEBUG
	if (_eventManager->IsClearable()) {
		return true;
	}
	if (!_player.lock()->IsAlive()) {
		return true;
	}
	return false;
}

std::shared_ptr<SceneBase> SceneGamePlay::CreateNextScene() const
{
	return std::make_shared<SceneResult>();
}
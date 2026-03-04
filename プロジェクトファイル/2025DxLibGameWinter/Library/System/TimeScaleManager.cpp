#include "TimeScaleManager.h"
#include "GameObjectManager.h"
#include "Library/Objects/GameObject.h"
#include <functional>

namespace {
	constexpr float kDefaultGameSpeed = 1.0f;
}

TimeScaleManager::TimeScaleManager() :
	_playerData(),
	_otherData()
{
}

TimeScaleManager& TimeScaleManager::GetInstance()
{
	static TimeScaleManager manager;
	return manager;
}

void TimeScaleManager::Init()
{
	// 指定の値で初期化
	_otherData.currentScale = kDefaultGameSpeed;
	_otherData.addScaleValue = 0.0f;
	_otherData.nextScale = _otherData.currentScale;

	_playerData = _otherData;
}

void TimeScaleManager::Update()
{
	// 更新
	_playerData.Update();
	_otherData.Update();
}

void TimeScaleManager::ChangeScale(const float scale, const int frame)
{
	ChangePlayerScale(scale, frame);
	ChangeOtherScale(scale, frame);
}

void TimeScaleManager::ChangePlayerScale(const float scale, const int frame)
{
	_playerData.nextScale = scale;
	_playerData.addScaleValue = (scale - _playerData.currentScale) / frame;
}

void TimeScaleManager::ChangeOtherScale(const float scale, const int frame)
{
	_otherData.nextScale = scale;
	_otherData.addScaleValue = (scale - _otherData.currentScale) / frame;
}

float TimeScaleManager::GetDefaultScale()
{
	return kDefaultGameSpeed;
}



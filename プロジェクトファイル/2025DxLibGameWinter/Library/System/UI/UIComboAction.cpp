#include "UIComboAction.h"
#include "Library/Geometry/Easing.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>

namespace {
	constexpr float kScale = 0.6f * Statistics::kScreenWidthFullHDRatio;

	constexpr float kScaleMin = 0.0f;
	constexpr float kScaleMax = kScale;
	constexpr int kAnimFrame = 0.5f * Statistics::kFPS;
}

UIComboAction::UIComboAction(
	int graphHandle, Position2 centerPos, bool isDraw, 
	float scale, float alpha) :
	UIGraph(graphHandle, centerPos, isDraw, scale, alpha),
	_nowUpdateState(&UIComboAction::UpdateIn),
	_isStay(true),
	_scaleAnimProgress(0.0f),
	_progScale(1.0f)
{
}

UIComboAction::~UIComboAction()
{
}

void UIComboAction::Update()
{
	(this->*_nowUpdateState)();
}

void UIComboAction::UpdateIn()
{
	if (_scaleAnimProgress >= 1.0f) {
		_nowUpdateState = &UIComboAction::UpdateStay;
		return;
	}

	UpdateAnimScale(_progScale);
}

void UIComboAction::UpdateStay()
{
	// 待機状態が終了したら
	if (!_isStay) {
		_progScale *= -1.0f;
		_nowUpdateState = &UIComboAction::UpdateOut;
		return;
	}
}

void UIComboAction::UpdateOut()
{
	// 終了可能なら
	if (_scaleAnimProgress <= 0.0f) {
		SetDeleteState(true);
		return;
	}

	UpdateAnimScale(_progScale);
}

void UIComboAction::UpdateAnimScale(float progScale)
{
	if (_scaleAnimProgress <= 0.0f) {
		_scaleAnimProgress = 0.0f;
	}
	_scaleAnimProgress += 1.0f / kAnimFrame * progScale;

	float prog = Easing::Get(_scaleAnimProgress, EasingType::EaseOutElastic);
	if (progScale < 0.0f) prog = Easing::Get(_scaleAnimProgress, EasingType::EaseOutCubic);
	float scale = prog * (kScaleMax - kScaleMin);
	scale += kScaleMin;
	if (scale < 0.0f) scale = 0.0f;
	SetScale(scale);
}

#include "UIComboScore.h"
#include "Library/System/Statistics.h"
#include "Library/Geometry/Easing.h"
#include <DxLib.h>

namespace {
	constexpr float kScale = 0.8f * Statistics::kScreenWidthFullHDRatio;

	constexpr float kScaleMin = 0.0f;
	constexpr float kScaleMax = kScale;
	constexpr int kMaxDrawFrame = 1.5 * Statistics::kFPS;
	constexpr int kAnimFrame = 0.5f * Statistics::kFPS;

	const Vector2 kAnimPosOffset = Vector2(200.0f, 0.0f) * Statistics::kScreenWidthFullHDRatio;
}

UIComboScore::UIComboScore(int graphHandle, Position2 centerPos, 
	int drawScore) :
	UIBase(centerPos, true, true),
	_nowUpdateState(&UIComboScore::UpdateIn),
	_graphHandle(graphHandle),
	_drawScore(drawScore),
	_scale(kScaleMin),
	_basePos(),
	_posOffset(),
	_drawFrame(kMaxDrawFrame),
	_moveAnimProgress(0.0f),
	_scaleAnimProgress(0.0f),
	_progScale(1.0f)
{
}

UIComboScore::~UIComboScore()
{
	if (_graphHandle >= 0) DeleteGraph(_graphHandle);
}

void UIComboScore::Init()
{
}

void UIComboScore::Update()
{
	(this->*_nowUpdateState)();
}

void UIComboScore::Draw(Vector2 shakeOffset)
{
	if (!_isDraw) return;
	int w, h;
	GetGraphSize(_graphHandle, &w, &h);
	int drawPosX = _basePos.x + _posOffset.x + shakeOffset.x;
	int drawPosY = _basePos.y + _posOffset.y + shakeOffset.y;
	DrawRectRotaGraph(
		drawPosX, drawPosY, // 画面上の描画中心
		0, 0,			// 元画像の左上座標
		w, h,			// 元画像の幅と高さ
		_scale,			// 拡大率
		0.0,			// 回転（ラジアン）
		_graphHandle,	// グラフィックハンドル
		TRUE,			// 透過フラグ
		FALSE			// 反転フラグ
	);
}

void UIComboScore::UpdateIn()
{
	if (_scaleAnimProgress >= 1.0f) {
		_nowUpdateState = &UIComboScore::UpdateStay;
		return;
	}

	UpdateAnimMove(_progScale);
	UpdateAnimScale(_progScale);
}

void UIComboScore::UpdateStay()
{
	_drawFrame--;
	// 描画フレームが終了したら
	if (_drawFrame <= 0) {
		_drawFrame = 0;
		_progScale *= -1.0f;
		_nowUpdateState = &UIComboScore::UpdateOut;
		return;
	}
}

void UIComboScore::UpdateOut()
{
	// 終了可能なら
	if (_scaleAnimProgress <= 0.0f) {
		SetDeleteState(true);
		return;
	}

	UpdateAnimMove(_progScale);
	UpdateAnimScale(_progScale);
}

void UIComboScore::UpdateAnimMove(float progScale)
{
	if (_moveAnimProgress <= 0.0f) {
		_moveAnimProgress = 0.0f;
	}
	_moveAnimProgress += 1.0f / kAnimFrame * progScale;
	float prog = Easing::Get(_moveAnimProgress, EasingType::EaseOutElastic);
	if (progScale < 0.0f) prog = Easing::Get(_moveAnimProgress, EasingType::EaseOutCubic);
	_posOffset = kAnimPosOffset * (1.0f - prog);
}

void UIComboScore::UpdateAnimScale(float progScale)
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
	_scale = scale;
}

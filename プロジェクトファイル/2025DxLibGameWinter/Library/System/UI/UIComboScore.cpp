#include "UIComboScore.h"
#include "Library/System/Statistics.h"
#include "Library/Geometry/Easing.h"
#include "Scene/ResultDataHolder.h"
#include <DxLib.h>

namespace {
	constexpr float kScale = 0.8f * Statistics::kScreenWidthFullHDRatio;

	constexpr float kScaleMin = 0.0f;
	constexpr float kScaleMax = kScale;
	constexpr int kMaxDrawFrame = 1.5 * Statistics::kFPS;
	constexpr int kAnimFrame = 0.5f * Statistics::kFPS;

	constexpr int kMaxDigits = 2;
	constexpr float kScoreScaleOffset = 0.5f;

	const Vector2 kAnimPosOffset = Vector2(200.0f, 0.0f) * Statistics::kScreenWidthFullHDRatio;
}

UIComboScore::UIComboScore(int baseGraphHandle, int scoreGraphHandle, Position2 centerPos,
	int drawScore) :
	UIBase(centerPos, true, true),
	_nowUpdateState(&UIComboScore::UpdateIn),
	_baseGraphHandle(baseGraphHandle),
	_scoreGraphHandle(scoreGraphHandle),
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
	if (_baseGraphHandle >= 0) DeleteGraph(_baseGraphHandle);
}

void UIComboScore::Init()
{
	ResultDataHolder::GetInstance().AddScore(_drawScore);
}

void UIComboScore::Update()
{
	(this->*_nowUpdateState)();
}

void UIComboScore::Draw(Vector2 shakeOffset)
{
	if (!_isDraw) return;
	int w, h;
	GetGraphSize(_baseGraphHandle, &w, &h);
	int drawPosX = _basePos.x + _posOffset.x + shakeOffset.x;
	int drawPosY = _basePos.y + _posOffset.y + shakeOffset.y;
	DrawRectRotaGraph(
		drawPosX, drawPosY, // 画面上の描画中心
		0, 0,			// 元画像の左上座標
		w, h,			// 元画像の幅と高さ
		_scale,			// 拡大率
		0.0,			// 回転（ラジアン）
		_baseGraphHandle,	// グラフィックハンドル
		TRUE,			// 透過フラグ
		FALSE			// 反転フラグ
	);

	DrawScore(Position2(drawPosX, drawPosY), 
		kMaxDigits, _scale * kScoreScaleOffset);
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

void UIComboScore::DrawScore(Position2 centerPos, int maxDigits, float scale)
{
	if (_scoreGraphHandle == -1) return;

	int allWidth, allHeight;
	GetGraphSize(_scoreGraphHandle, &allWidth, &allHeight);

	int charWidth = allWidth / 10;
	int charHeight = allHeight;

	// 実際に画面上で占める1文字の幅
	float displayWidth = charWidth * scale;

	for (int i = 0; i < maxDigits; ++i) {
		int powerOfTen = 1;
		for (int j = 0; j < (maxDigits - 1 - i); ++j) {
			powerOfTen *= 10;
		}
		int digit = (_drawScore / powerOfTen) % 10;

		int srcX = digit * charWidth;

		// startXを左端にするため、中心座標を「displayWidth / 2」分だけ右にずらして開始する
		float drawX = centerPos.x + (displayWidth / 2.0f) + (i * displayWidth);
		float drawY = centerPos.y;

		DrawRectRotaGraph(
			(int)drawX, (int)drawY,
			srcX, 0,
			charWidth, charHeight,
			(double)scale, 0.0,
			_scoreGraphHandle, TRUE
		);
	}
}

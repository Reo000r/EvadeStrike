#include "UILockonTarget.h"
#include "Library/Geometry/Easing.h"
#include <DxLib.h>

namespace {
	constexpr float kBaseAlpha = 0.5f;
	constexpr float kDecAlpha = 0.4f * kBaseAlpha;

	// 減少アニメーションの総時間
	constexpr int kDecAnimTime = 90;
	// 減少し始めるまでの時間
	constexpr int kStartDecAnimTime = kDecAnimTime - 60;
	
	// 描画アニメーションの総時間
	constexpr int kDrawAnimTime = 15;
	// アニメーションで追加する拡縮量
	constexpr float kDrawAnimScaleMul = 0.3f;
}

UILockonTarget::UILockonTarget(
	int graphHandle, Position2 centerPos, float scale) :
	UIBase(centerPos, false, false),
	_graphHandle(graphHandle),
	_percent(0.0f),
	_baseScale(scale),
	_animStartPercent(0.0f),
	_decAnimFrame(0),
	_drawAnimFrame(0)
{
}

UILockonTarget::~UILockonTarget()
{
}

void UILockonTarget::Init()
{
}

void UILockonTarget::Update()
{
	UpdateAnim();
}

void UILockonTarget::Draw(Vector2 shakeOffset)
{
	if (!_isDraw && _drawAnimFrame <= 0) return;

	int drawPosX = static_cast<int>(_centerPos.x + shakeOffset.x);
	int drawPosY = static_cast<int>(_centerPos.y + shakeOffset.y);
	float currentScale = CalcCurrentScale();
	float currentAlpha = CalcCurrentAlpha();
	// 減少アニメーション描画
	float visualPercent = GetAnimPercent();
	printf("LockAlpha:%.02f\n", kDecAlpha * currentAlpha);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * kDecAlpha * currentAlpha));
	// DrawCircleGaugeを使用し円形のゲージを表示
	DrawCircleGauge(drawPosX, drawPosY, visualPercent, _graphHandle, 0.0, currentScale);

	// メインゲージ描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * kBaseAlpha * currentAlpha));
	// DrawCircleGaugeを使用し円形のゲージを表示
	DrawCircleGauge(drawPosX, drawPosY, _percent, _graphHandle, 0.0, currentScale);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void UILockonTarget::SetDrawState(bool drawState)
{
	// 変更がない場合はreturn
	if (_isDraw == drawState) return;
	_isDraw = drawState;

	// アニメーションに反転処理があった場合は
	if (_drawAnimFrame > 0) {
		// 既存のアニメーションの途中から反転アニメーションを行う
		_drawAnimFrame = kDrawAnimTime - _drawAnimFrame;
	}
	else {
		_drawAnimFrame = kDrawAnimTime;
	}
}

float UILockonTarget::GetAnimPercent() const
{
	float ret = 0.0f;

	if (CalcDecAnimProg() > 0.0f) {
		float progress = CalcDecAnimProg();
		float decAmount = (_animStartPercent - _percent) * Easing::Get(progress, EasingType::EaseOutCubic);
		ret = _percent + decAmount;
	}
	else {
		ret = _percent;
	}
	return ret;
}

void UILockonTarget::SetPercent(float percent)
{
	// パーセントが更新されたら
	if (_percent != percent) {
		// アニメーション更新中であれば
		if (_decAnimFrame > 0) {
			// 現在のアニメーション
			_animStartPercent = GetAnimPercent();
		}
		// 更新中でなければ
		else {
			// 値が正常に減っている場合は
			if (_percent > percent) {
				_animStartPercent = _percent;
			}
			// 前回描画した割合より増えている場合は
			else {
				_animStartPercent = 100.0f;
			}
		}
		// アニメーション時間をリセット
		_decAnimFrame = kDecAnimTime;
	}
	_percent = percent;
}

void UILockonTarget::UpdateAnim()
{
	UpdateAnimDec();
	UpdateAnimDraw();
}

void UILockonTarget::UpdateAnimDec()
{
	if (_decAnimFrame <= 0) {
		return;
	}
	_decAnimFrame--;
}

void UILockonTarget::UpdateAnimDraw()
{
	if (_drawAnimFrame <= 0) {
		return;
	}
	_drawAnimFrame--;
}

float UILockonTarget::CalcDecAnimProg() const
{
	float ret = static_cast<float>(_decAnimFrame) / kStartDecAnimTime;
	// 1以上なら丸める
	if (ret > 1.0f) ret = 1.0f;
	return ret;
}

float UILockonTarget::CalcCurrentDrawAnimProg() const
{
	float progress = static_cast<float>(
		_drawAnimFrame) / kDrawAnimTime;
	// 反転
	if (_isDraw) progress = progress * -1.0f + 1.0f;
	return progress;
}

float UILockonTarget::CalcCurrentScale() const
{
	float progress = CalcCurrentDrawAnimProg();
	progress = Easing::Get(progress, EasingType::EaseOutCubic);;
	// scaleは反転してほしいため反転させる
	progress = progress * -1.0f + 1.0f;
	float ret = _baseScale + 
		(_baseScale * kDrawAnimScaleMul * progress);
	return ret;
}

float UILockonTarget::CalcCurrentAlpha() const
{
	float progress = CalcCurrentDrawAnimProg();
	float ret = Easing::Get(progress, EasingType::EaseOutCubic);;
	return ret;
}

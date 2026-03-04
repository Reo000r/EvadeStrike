#include "UIPlayerHP.h"
#include "Library/System/Statistics.h"
#include "Library/Geometry/Easing.h"
#include "Objects/Character/Player/Player.h"
#include "UIManager.h"
#include <DxLib.h>
#include <functional>

namespace {
	constexpr float kScale = 0.6f * Statistics::kScreenWidthFullHDRatio;

	const Vector2 kGaugeOffset = Vector2(15, -62) * kScale;

	// アニメーションを行う時間
	constexpr int kShakeAnimTime = 10;
	constexpr int kDecAnimTime = 90;
	// アニメーションを行い始める時間
	constexpr int kStartDecAnimTime = kDecAnimTime - 60;
	// 振動の大きさ(カメラの振動量依存)
	constexpr float kMagnitude = 0.7f;
}

UIPlayerHP::UIPlayerHP(
	int baseGraphHandle,
	int gaugeGraphHandle,
	int decGaugeGraphHandle) :
	UIBase(Position2(Statistics::kScreenWidth * 0.25f, 
		Statistics::kScreenHeight * 0.1f), true),
	_baseGraphHandle(baseGraphHandle),
	_gaugeGraphHandle(gaugeGraphHandle),
	_decGaugeGraphHandle(decGaugeGraphHandle),
	_scale(kScale),
	_player(),
	_maxHp(0.0f),
	_lastHp(0.0f),
	_animStartHp(0.0f),
	_shakeAnimFrame(0),
	_decAnimFrame(0),
	_decAnimProgress(0.0f)
{
}

UIPlayerHP::~UIPlayerHP()
{
	if (_baseGraphHandle >= 0) DeleteGraph(_baseGraphHandle);
	if (_gaugeGraphHandle >= 0) DeleteGraph(_gaugeGraphHandle);
	if (_decGaugeGraphHandle >= 0) DeleteGraph(_decGaugeGraphHandle);
}

void UIPlayerHP::Init()
{
}

void UIPlayerHP::Update()
{
	float currentHp = _player.lock()->GetHitPoint();

	// HPが減っていたら
	if (currentHp < _lastHp) {
		// アニメーションを始める
		_shakeAnimFrame = kShakeAnimTime;
		// 減少アニメーションを行い始めていれば
		if (_decAnimFrame <= kStartDecAnimTime) {
			_decAnimFrame = kDecAnimTime;
			_animStartHp = _lastHp;
		}
	}

	// アニメーション更新
	UpdateAnim();
	// HP更新
	_lastHp = currentHp;
}

void UIPlayerHP::Draw(Vector2 shakeOffset)
{
	float hp = _player.lock()->GetHitPoint();
	float ratio = 0.0f;
	// HPがあった場合
	if (hp > 0.0f) {
		ratio = hp / _maxHp;
	}

	// 各要素の描画
	Position2 drawPosBase = _centerPos + shakeOffset;
	DrawHPBase(drawPosBase);
	Position2 drawPosGauge = drawPosBase + kGaugeOffset;
	DrawHPGaugeDec(drawPosGauge, ratio);
	DrawHPGauge(drawPosGauge, ratio);
}

void UIPlayerHP::UpdateAnim()
{
	// アニメーションの更新
	UpdateAnimDec();
}

void UIPlayerHP::UpdateAnimDec()
{
	// アニメーションを行っていなければreturn
	if (_decAnimFrame <= 0) {
		return;
	}
	_decAnimFrame--;
	_decAnimProgress =
		static_cast<float>(_decAnimFrame) / kStartDecAnimTime;
}

void UIPlayerHP::DrawHPBase(Position2 drawPos) const
{
	int w, h;
	GetGraphSize(_baseGraphHandle, &w, &h);
	DrawRectRotaGraph(
		static_cast<int>(drawPos.x),
		static_cast<int>(drawPos.y), // 画面上の描画中心
		0, 0,				// 元画像の左上座標
		w, h,				// 元画像の幅と高さ
		_scale,				// 拡大率
		0.0,				// 回転（ラジアン）
		_baseGraphHandle,	// グラフィックハンドル
		TRUE,				// 透過フラグ
		FALSE				// 反転フラグ
	);
}

void UIPlayerHP::DrawHPGauge(Position2 drawPos, float ratio) const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	int w, h;
	GetGraphSize(_gaugeGraphHandle, &w, &h);
	int drawW = static_cast<int>(w * ratio);
	// 中心を保つためのオフセット
	float offsetMoveX = (w - drawW) * 0.5f * _scale;
	DrawRectRotaGraph(
		static_cast<int>(drawPos.x - offsetMoveX),
		static_cast<int>(drawPos.y), // 画面上の描画中心
		0, 0,				// 元画像の左上座標
		drawW, h,			// 元画像の幅と高さ
		_scale,				// 拡大率
		0.0,				// 回転（ラジアン）
		_gaugeGraphHandle,	// グラフィックハンドル
		TRUE,				// 透過フラグ
		FALSE				// 反転フラグ
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void UIPlayerHP::DrawHPGaugeDec(Position2 drawPos, float ratio)
{
	if (_decAnimProgress <= 0.0f) return;
	if (_decAnimProgress > 1.0f) {
		_decAnimProgress = 1.0f;
	}

	float baseRatio = _animStartHp / _maxHp;
	float decAmount = baseRatio - ratio;
	decAmount *= Easing::Get(_decAnimProgress, EasingType::EaseOutCubic);

	int w, h;
	GetGraphSize(_decGaugeGraphHandle, &w, &h);
	int drawW = static_cast<int>(w * (decAmount + ratio));
	// 中心を保つためのオフセット
	float offsetMoveX = (w - drawW) * 0.5f * _scale;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 127);
	DrawRectRotaGraph(
		static_cast<int>(drawPos.x - offsetMoveX),
		static_cast<int>(drawPos.y), // 画面上の描画中心
		0, 0,				// 元画像の左上座標
		drawW, h,			// 元画像の幅と高さ
		_scale,				// 拡大率
		0.0,				// 回転（ラジアン）
		_gaugeGraphHandle,	// グラフィックハンドル
		TRUE,				// 透過フラグ
		FALSE				// 反転フラグ
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

#include "UITutorialControllerDisplay.h"
#include "Library/System/Input.h"
#include "Library/System/Statistics.h"
#include "Library/Geometry/Easing.h"
#include <DxLib.h>
#include <algorithm>
#include <cmath>

namespace {
	// 点滅の周期(秒)
	constexpr float kBlinkCycle = 1.0f;
	// 実押下時のフラッシュ持続時間(秒)
	constexpr float kFlashDuration = 0.35f;
	// スティック入力時の見た目上の最大移動量
	constexpr float kStickTiltRadius = 0.005f;
	// スティック入力を行ったとみなす閾値
	constexpr float kStickPressThreshold = 0.2f;
	// コントローラー画像のパス
	const std::wstring kGraphExtension = L".png";
	const std::wstring kControllerGraphPath = L"Data/Graph/Tutorial/ControllerBase" + kGraphExtension;
	const std::wstring kButtonGraphBasePath = L"Data/Graph/Tutorial/Button_";
}

UITutorialControllerDisplay::UITutorialControllerDisplay(Position2 anchorPos, float scale) :
	UIBase(anchorPos, true, false),
	_controllerImageHandle(-1),
	_overlays(),
	_scale(scale),
	_leftStickCurrentTilt(),
	_rightStickCurrentTilt()
#ifdef _DEBUG
	,
	_isDebugAdjustMode(false),
	_debugSelectedIndex(0)
#endif
{
}

void UITutorialControllerDisplay::Init()
{
	_controllerImageHandle = LoadGraph(kControllerGraphPath.c_str());
	InitOverlays();
}

void UITutorialControllerDisplay::InitOverlays()
{
	_overlays.clear();

	// 以下のオフセットは仮の配置。
	// 実際のコントローラー画像を組み込んだ後、_DEBUG時のデバッグ調整モード
	// (Debug:ControllerAdjustToggle で切替)を用いて微調整すること。
	auto addOverlay = [this](const std::wstring& key, const wchar_t* fileName, Vector2 offset) {
		ButtonOverlay ov;
		ov.key = key;
		ov.imageHandle = LoadGraph(fileName);
		ov.offsetFromAnchorBase = offset;
		_overlays.push_back(ov);
		};

	addOverlay(L"A", (kButtonGraphBasePath + L"A" + kGraphExtension).c_str(), Vector2(0.0f, 0.0f));
	addOverlay(L"B", (kButtonGraphBasePath + L"B" + kGraphExtension).c_str(), Vector2(14.0f, -14.0f));
	addOverlay(L"X", (kButtonGraphBasePath + L"X" + kGraphExtension).c_str(), Vector2(-14.0f, -14.0f));
	addOverlay(L"Y", (kButtonGraphBasePath + L"Y" + kGraphExtension).c_str(), Vector2(0.0f, -28.0f));
	addOverlay(L"LB", (kButtonGraphBasePath + L"LB" + kGraphExtension).c_str(), Vector2(0.0f, 0.0f));
	addOverlay(L"RB", (kButtonGraphBasePath + L"RB" + kGraphExtension).c_str(), Vector2(0.0f, 0.0f));
	addOverlay(L"LT", (kButtonGraphBasePath + L"LT" + kGraphExtension).c_str(), Vector2(0.0f, 0.0f));
	addOverlay(L"RT", (kButtonGraphBasePath + L"RT" + kGraphExtension).c_str(), Vector2(0.0f, 0.0f));
	addOverlay(L"LS", (kButtonGraphBasePath + L"LS" + kGraphExtension).c_str(), Vector2(-80.0f, -30.0f));
	addOverlay(L"RS", (kButtonGraphBasePath + L"RS" + kGraphExtension).c_str(), Vector2(0.0f, 0.0f));
	addOverlay(L"Pause", (kButtonGraphBasePath + L"Pause" + kGraphExtension).c_str(), Vector2(0.0f, 0.0f));
}

void UITutorialControllerDisplay::SetActiveButtons(const std::vector<std::wstring>& buttonKeys)
{
	for (auto& ov : _overlays) {
		bool isActive = std::find(buttonKeys.begin(), buttonKeys.end(), ov.key) != buttonKeys.end();
		ov.isActive = isActive;
		if (!isActive) {
			ov.blinkTime = 0.0f;
		}
	}
}

bool UITutorialControllerDisplay::IsInputForButtonKey(const std::wstring& key)
{
	if (key == L"A")     return Input::GetInstance().IsTrigger("Gameplay:Jump");
	if (key == L"B")     return Input::GetInstance().IsTrigger("Gameplay:Kick");
	//if (key == L"X")     return Input::GetInstance().IsTrigger("Debug:ChangePlayerGravity");
	if (key == L"Y")     return Input::GetInstance().IsTrigger("Gameplay:Punch");
	//if (key == L"LB")    return Input::GetInstance().IsTrigger("Debug:JustDodge");
	if (key == L"RB")    return Input::GetInstance().IsTrigger("Gameplay:Dodge");
	if (key == L"LS")    return Input::GetInstance().GetPadLeftStick().Magnitude() > kStickPressThreshold;
	if (key == L"RS")    return Input::GetInstance().GetPadRightStick().Magnitude() > kStickPressThreshold;
	if (key == L"Pause") return Input::GetInstance().IsTrigger("Debug:PauseScene");
	return false;
}

void UITutorialControllerDisplay::Update()
{
	float dt = 1.0f / Statistics::kFPS;

	for (auto& ov : _overlays) {
		if (ov.isActive) {
			ov.blinkTime += dt;
		}
		else {
			ov.blinkTime = 0.0f;
		}

		if (IsInputForButtonKey(ov.key)) {
			ov.flashTime = kFlashDuration;
		}
		if (ov.flashTime > 0.0f) {
			ov.flashTime -= dt;
			if (ov.flashTime < 0.0f) ov.flashTime = 0.0f;
		}
	}

	// スティックの傾き取得(-1.0~1.0)
	_leftStickCurrentTilt = Input::GetInstance().GetPadLeftStick();
	_rightStickCurrentTilt = Input::GetInstance().GetPadRightStick();

#ifdef _DEBUG
	DebugUpdate();
#endif
}

void UITutorialControllerDisplay::Draw(Vector2 shakeOffset)
{
	if (!_isDraw) return;

	Position2 anchor = GetCenterPos() + shakeOffset;

	// コントローラー本体
	if (_controllerImageHandle >= 0) {
		int w = 0, h = 0;
		GetGraphSize(_controllerImageHandle, &w, &h);
		DrawRectRotaGraph(
			static_cast<int>(anchor.x), static_cast<int>(anchor.y),
			0, 0, w, h,
			_scale, 0.0,
			_controllerImageHandle, TRUE, FALSE);
	}

	// 各ボタンオーバーレイ
	for (const auto& ov : _overlays) {
		if (ov.imageHandle < 0) continue;

		Vector2 offset = ov.offsetFromAnchorBase * _scale;

		// LS/RSはスティックの傾きに応じて描画位置をずらす
		if (ov.key == L"LS") offset += _leftStickCurrentTilt * (kStickTiltRadius * _scale);
		if (ov.key == L"RS") offset += _rightStickCurrentTilt * (kStickTiltRadius * _scale);

		Position2 drawPos = anchor + offset;

		float alpha = 0.0f;
		if (ov.isActive) {
			// 点滅
			float phase = fmodf(ov.blinkTime, kBlinkCycle) / kBlinkCycle;
			float triangle = ((1.0f - phase) * 2.0f);
			if (phase < 0.5f) {
				triangle = phase * 2.0f;
			}
			alpha = Easing::Get(triangle, EasingType::EaseInOutSine);
		}
		// 実押下時のフラッシュを加算
		alpha += ov.flashTime / kFlashDuration;
		if (alpha > 1.0f) alpha = 1.0f;
		if (alpha <= 0.0f) continue;

		int w = 0, h = 0;
		GetGraphSize(ov.imageHandle, &w, &h);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * alpha));
		DrawRectRotaGraph(
			static_cast<int>(drawPos.x), static_cast<int>(drawPos.y),
			0, 0, w, h,
			_scale, 0.0,
			ov.imageHandle, TRUE, FALSE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

#ifdef _DEBUG
	DebugDraw();
#endif
}

#ifdef _DEBUG
void UITutorialControllerDisplay::DebugUpdate()
{
	if (Input::GetInstance().IsTrigger("Debug:ControllerAdjustToggle")) {
		_isDebugAdjustMode = !_isDebugAdjustMode;
	}
	if (!_isDebugAdjustMode || _overlays.empty()) return;

	if (Input::GetInstance().IsTrigger("Debug:ControllerAdjustNext")) {
		_debugSelectedIndex = (_debugSelectedIndex + 1) % static_cast<int>(_overlays.size());
	}
	if (Input::GetInstance().IsTrigger("Debug:ControllerAdjustPrev")) {
		_debugSelectedIndex = (_debugSelectedIndex - 1 + static_cast<int>(_overlays.size())) % static_cast<int>(_overlays.size());
	}

	ButtonOverlay& target = _overlays[_debugSelectedIndex];
	float step = 2.0f;
	if (Input::GetInstance().IsPress("Debug:ControllerAdjustFine")) {
		step = 0.5f;
	}
	if (Input::GetInstance().IsPress("System:Left"))  target.offsetFromAnchorBase.x -= step;
	if (Input::GetInstance().IsPress("System:Right")) target.offsetFromAnchorBase.x += step;
	if (Input::GetInstance().IsPress("System:Up"))    target.offsetFromAnchorBase.y -= step;
	if (Input::GetInstance().IsPress("System:Down"))  target.offsetFromAnchorBase.y += step;

	if (Input::GetInstance().IsTrigger("Debug:ControllerAdjustPrint")) {
		printf("[TutorialController] key=%ls offset=(%.1f, %.1f)\n",
			target.key.c_str(), target.offsetFromAnchorBase.x, target.offsetFromAnchorBase.y);
	}
}

void UITutorialControllerDisplay::DebugDraw() const
{
	if (!_isDebugAdjustMode || _overlays.empty()) return;

	const ButtonOverlay& target = _overlays[_debugSelectedIndex];
	DrawFormatString(20, 100, 0xffff00,
		L"[ControllerDebug] target=%s offset=(%.1f, %.1f)  F1:終了 [Shift]:対象切替 矢印:移動 Ctrl:微調整 Space:出力",
		target.key.c_str(), target.offsetFromAnchorBase.x, target.offsetFromAnchorBase.y);
}
#endif
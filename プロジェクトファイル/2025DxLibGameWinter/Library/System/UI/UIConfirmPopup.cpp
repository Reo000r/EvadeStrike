#include "UIConfirmPopup.h"
#include "Library/System/Input.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>

namespace {
	constexpr int kBoxWidth = 560;
	constexpr int kBoxHeight = 200;
	constexpr int kFontSize = 26;

	constexpr unsigned int kSelectedColor = 0xffffaa;
	constexpr unsigned int kUnselectedColor = 0xffffff;

	constexpr int kUnderLineHeight = 5;
	constexpr int kUnderLineThickness = 3;
}

UIConfirmPopup::UIConfirmPopup(
	const std::wstring& message,
	Callback_t onYes,
	Callback_t onNo) :
	UIBase(Position2(Statistics::kScreenCenterWidth, Statistics::kScreenCenterHeight), true, false),
	_message(message),
	_onYes(onYes),
	_onNo(onNo),
	_isYesSelected(true)
{
}

void UIConfirmPopup::Init()
{
	// 処理なし
}

void UIConfirmPopup::Update()
{
	if (Input::GetInstance().IsTrigger("System:Left") ||
		Input::GetInstance().IsTrigger("System:Right")) {
		_isYesSelected = !_isYesSelected;
	}

	if (Input::GetInstance().IsTrigger("System:Submit")) {
		if (_isYesSelected) {
			if (_onYes) _onYes();
		}
		else {
			if (_onNo) _onNo();
		}
		// 決定後は自身を削除対象にする
		SetDeleteState(true);
	}
}

void UIConfirmPopup::Draw(Vector2 shakeOffset)
{
	if (!_isDraw) return;

	Position2 center = GetCenterPos() + shakeOffset;

	int left = static_cast<int>(center.x) - kBoxWidth / 2;
	int top = static_cast<int>(center.y) - kBoxHeight / 2;
	int right = left + kBoxWidth;
	int bottom = top + kBoxHeight;

	// 背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(left, top, right, bottom, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(left, top, right, bottom, 0xffffff, FALSE);

	int defaultFontSize = GetFontSize();
	SetFontSize(kFontSize);

	// メッセージ
	DrawFormatString(left + 30, top + 30, 0xffffff, L"%s", _message.c_str());

	// 選択肢
	unsigned int yesColor = kSelectedColor;
	unsigned int noColor = kUnselectedColor;
	if (!_isYesSelected) {
		yesColor = kUnselectedColor;
		noColor = kSelectedColor;
	}
	DrawFormatString(static_cast<int>(center.x) - 100, bottom - 60, yesColor, L"はい");
	DrawFormatString(static_cast<int>(center.x) + 40, bottom - 60, noColor, L"いいえ");

	// 下線を引く
	int lineY = bottom - kUnderLineHeight;
	if (_isYesSelected) {
		int x = static_cast<int>(center.x) - 100;
		int width = GetDrawFormatStringWidth(L"はい");
		DrawLine(x, lineY, x + width, lineY, 0xffffff, kUnderLineThickness);
	}
	else {
		int x = static_cast<int>(center.x) + 40;
		int width = GetDrawFormatStringWidth(L"いいえ");
		DrawLine(x, lineY, x + width, lineY, 0xffffff, kUnderLineThickness);
	}

	SetFontSize(defaultFontSize);
}
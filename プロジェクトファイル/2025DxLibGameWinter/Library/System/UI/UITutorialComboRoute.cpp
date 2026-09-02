#include "UITutorialComboRoute.h"
#include <DxLib.h>

namespace {
	constexpr int kBaseFontSize = 24;
	constexpr float kBaseLineHeight = 36.0f;
	constexpr unsigned int kTextColor = 0xffffff;

	// îwåi
	constexpr unsigned int kBackgroundColor = 0x000000;
	constexpr int kBackgroundAlpha = 160;
	constexpr float kBackgroundPadding = 20.0f;
}

UITutorialComboRoute::UITutorialComboRoute(Position2 anchorPos, float scale) :
	UIBase(anchorPos, false, false),
	_comboList(),
	_scale(scale)
{
}

void UITutorialComboRoute::Init()
{
	// èàóùÇ»Çµ
}

void UITutorialComboRoute::Update()
{
	// èàóùÇ»Çµ
}

void UITutorialComboRoute::Draw(Vector2 shakeOffset)
{
	if (!_isDraw) return;
	if (_comboList.empty()) return;

	Position2 basePos = GetCenterPos() + shakeOffset;

	int defaultFontSize = GetFontSize();
	SetFontSize(static_cast<int>(kBaseFontSize * _scale));

	float lineHeight = kBaseLineHeight * _scale;

	// ï\é¶ï∂éöóÒÇêÊÇ…ç\ízÇµÇ¬Ç¬ÅAîwåiïùåàíËÇÃÇΩÇﬂç≈ëÂï∂éöïùÇåvë™Ç∑ÇÈ
	std::vector<std::wstring> lines;
	int maxWidth = 0;
	for (const auto& combo : _comboList) {
		std::wstring line = BuildComboRouteText(combo);
		int width = GetDrawStringWidth(line.c_str(), static_cast<int>(line.length()));
		if (width > maxWidth) maxWidth = width;
		lines.push_back(line);
	}

	// îwåi
	float padding = kBackgroundPadding * _scale;
	int boxLeft = static_cast<int>(basePos.x - padding);
	int boxTop = static_cast<int>(basePos.y - padding);
	int boxRight = static_cast<int>(basePos.x + static_cast<float>(maxWidth) + padding);
	int boxBottom = static_cast<int>(basePos.y + lineHeight * static_cast<float>(lines.size()) + padding);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kBackgroundAlpha);
	DrawBox(boxLeft, boxTop, boxRight, boxBottom, kBackgroundColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// ï∂éö
	for (size_t i = 0; i < lines.size(); ++i) {
		float y = basePos.y + lineHeight * static_cast<float>(i);
		DrawFormatString(
			static_cast<int>(basePos.x), static_cast<int>(y),
			kTextColor, L"%s", lines[i].c_str());
	}

	SetFontSize(defaultFontSize);
}

std::wstring UITutorialComboRoute::ActionKindToLabel(PlayerActionKind kind)
{
	switch (kind) {
	case PlayerActionKind::Punch: return L"Y";
	case PlayerActionKind::Kick:  return L"B";
	default: return L"?";
	}
}

std::wstring UITutorialComboRoute::BuildComboRouteText(const PlayerComboHolder::ComboData_t& combo)
{
	std::wstring text;
	for (size_t i = 0; i < combo.size(); ++i) {
		if (i > 0) text += L"Å®";
		text += ActionKindToLabel(combo[i]);
	}
	return text;
}

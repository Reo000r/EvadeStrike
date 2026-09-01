#include "UITutorialTableOfContents.h"
#include <DxLib.h>

namespace {
	// scaleが1の時の基準フォントサイズ・行間(px)
	constexpr int kBaseFontSize = 24;
	constexpr float kBaseLineHeight = 32.0f;

	// 現在地/完了済み/未着手の文字色
	constexpr unsigned int kColorCurrent = 0x000000;
	constexpr unsigned int kColorCompleted = 0x1E90FF;
	constexpr unsigned int kColorNotStarted = 0x808080;

	// 背景
	constexpr unsigned int kBackgroundColor = 0x000000;
	constexpr int kBackgroundAlpha = 160;
	constexpr float kBackgroundPadding = 20.0f;
}

UITutorialTableOfContents::UITutorialTableOfContents(Position2 anchorPos, float scale) :
	UIBase(anchorPos, true, false),
	_topics(),
	_currentRegionId(-1),
	_scale(scale)
{
}

void UITutorialTableOfContents::Init()
{
	// 処理なし
}

void UITutorialTableOfContents::Update()
{
	// 処理なし
}

void UITutorialTableOfContents::Draw(Vector2 shakeOffset)
{
	if (!_isDraw) return;
	if (_topics.empty()) return;

	Position2 basePos = GetCenterPos() + shakeOffset;

	int defaultFontSize = GetFontSize();
	SetFontSize(static_cast<int>(kBaseFontSize * _scale));

	float lineHeight = kBaseLineHeight * _scale;

	// 最大文字幅を計測して背景の横幅を決める
	int maxWidth = 0;
	for (const auto& entry : _topics) {
		int width = GetDrawStringWidth(entry.name.c_str(), static_cast<int>(entry.name.length()));
		if (width > maxWidth) maxWidth = width;
	}

	// 背景
	float padding = kBackgroundPadding * _scale;
	int boxLeft = static_cast<int>(basePos.x - padding);
	int boxTop = static_cast<int>(basePos.y - padding);
	int boxRight = static_cast<int>(basePos.x + static_cast<float>(maxWidth) + padding);
	int boxBottom = static_cast<int>(basePos.y + lineHeight * static_cast<float>(_topics.size()) + padding);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kBackgroundAlpha);
	DrawBox(boxLeft, boxTop, boxRight, boxBottom, kBackgroundColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	for (size_t i = 0; i < _topics.size(); ++i) {
		const TutorialTocEntry& entry = _topics[i];

		unsigned int color = kColorNotStarted;
		if (entry.regionId == _currentRegionId) {
			color = kColorCurrent;
		}
		else if (entry.regionId < _currentRegionId) {
			color = kColorCompleted;
		}

		float y = basePos.y + lineHeight * static_cast<float>(i);
		DrawFormatString(
			static_cast<int>(basePos.x), static_cast<int>(y),
			color, L"%s", entry.name.c_str());
	}

	SetFontSize(defaultFontSize);
}
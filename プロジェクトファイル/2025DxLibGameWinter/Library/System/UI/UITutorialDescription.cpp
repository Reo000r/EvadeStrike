#include "UITutorialDescription.h"
#include "UITutorialControllerDisplay.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>

namespace {
	// scaleが1の時の基準フォントサイズ・行間(px)
	constexpr int kBaseFontSize = 28;
	constexpr float kBaseLineHeight = 36.0f;
	// scaleが1の時のブロック半幅の既定値(px)
	constexpr float kDefaultBlockHalfWidth = 450.0f;
	constexpr unsigned int kTextColor = 0xffffff;
	// 背景
	constexpr unsigned int kBackgroundColor = 0x000000;
	constexpr int kBackgroundAlpha = 160;
	constexpr float kBackgroundPadding = 24.0f;

	// タイプライター演出(1秒あたりに出現させる文字数)
	constexpr float kCharsPerSecond = 30.0f;
}

UITutorialDescription::UITutorialDescription(Position2 anchorPos, float scale) :
	UIBase(anchorPos, true, false),
	_fullText(),
	_visibleCharCount(0),
	_revealTimer(0.0f),
	_skipButtonKeys(),
	_scale(scale),
	_blockHalfWidthBase(kDefaultBlockHalfWidth)
{
}

void UITutorialDescription::Init()
{
	// 処理なし
}

void UITutorialDescription::Update()
{
	if (!IsRevealing()) return;

	// 出現中に対象ボタンが押されたら全文表示にする
	for (const auto& key : _skipButtonKeys) {
		if (UITutorialControllerDisplay::IsInputForButtonKey(key)) {
			SkipToFullText();
			return;
		}
	}

	// 経過時間に応じて表示文字数を増やす
	const float kSecondsPerChar = 1.0f / kCharsPerSecond;
	_revealTimer += 1.0f / Statistics::kFPS;
	while (_revealTimer >= kSecondsPerChar && IsRevealing()) {
		_visibleCharCount++;
		_revealTimer -= kSecondsPerChar;
	}
}

void UITutorialDescription::Draw(Vector2 shakeOffset)
{
	if (!_isDraw) return;
	if (_fullText.empty()) return;

	// 背景サイズは全文表示時の行数を基準に固定する(出現中にサイズが変わらないように)
	std::vector<std::wstring> allLines = SplitLines(_fullText);
	// 実際に描画するのは現在出現済みの文字までの行
	std::wstring visibleText = _fullText.substr(0, _visibleCharCount);
	std::vector<std::wstring> visibleLines = SplitLines(visibleText);

	Position2 anchor = GetCenterPos() + shakeOffset;

	int defaultFontSize = GetFontSize();
	SetFontSize(static_cast<int>(kBaseFontSize * _scale));

	float lineHeight = kBaseLineHeight * _scale;
	float blockHeight = lineHeight * static_cast<float>(allLines.size());
	float blockLeftX = anchor.x - _blockHalfWidthBase * _scale;
	// アンカーをブロック下端として、そこから上へ積み上げていく
	float baseY = anchor.y - blockHeight;

	// 背景
	float padding = kBackgroundPadding * _scale;
	int boxLeft = static_cast<int>(blockLeftX - padding);
	int boxTop = static_cast<int>(baseY - padding);
	int boxRight = static_cast<int>(blockLeftX + _blockHalfWidthBase * 2.0f * _scale + padding);
	int boxBottom = static_cast<int>(anchor.y + padding);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kBackgroundAlpha);
	DrawBox(boxLeft, boxTop, boxRight, boxBottom, kBackgroundColor, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 文字
	for (size_t i = 0; i < visibleLines.size(); ++i) {
		float y = baseY + lineHeight * static_cast<float>(i);
		DrawFormatString(
			static_cast<int>(blockLeftX), static_cast<int>(y),
			kTextColor, L"%s", visibleLines[i].c_str());
	}

	SetFontSize(defaultFontSize);
}

void UITutorialDescription::SetDescriptionText(const std::wstring& text)
{
	_fullText = text;
	// 最初からやり直す
	_visibleCharCount = 0;
	_revealTimer = 0.0f;
}

bool UITutorialDescription::IsRevealing() const
{
	return _visibleCharCount < static_cast<int>(_fullText.length());
}

void UITutorialDescription::SkipToFullText()
{
	_visibleCharCount = static_cast<int>(_fullText.length());
}

std::vector<std::wstring> UITutorialDescription::SplitLines(const std::wstring& text) const
{
	std::vector<std::wstring> lines;
	std::wstring current;
	for (wchar_t c : text) {
		if (c == L'\n') {
			lines.push_back(current);
			current.clear();
		}
		else {
			current += c;
		}
	}
	lines.push_back(current);
	return lines;
}

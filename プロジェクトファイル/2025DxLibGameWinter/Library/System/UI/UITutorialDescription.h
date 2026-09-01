#pragma once
#include "UIBase.h"
#include <string>
#include <vector>

/// <summary>
/// チュートリアル説明文UI
/// </summary>
class UITutorialDescription : public UIBase
{
public:
	UITutorialDescription(Position2 anchorPos, float scale);
	~UITutorialDescription() override = default;

	void Init() override;
	void Update() override;
	void Draw(Vector2 shakeOffset) override;

	/// <summary>
	/// 説明文を設定する
	/// </summary>
	void SetDescriptionText(const std::wstring& text);

	/// <summary>
	/// <para> 出現中に押されたら全文表示にする対象ボタンキーを設定する </para>
	/// <para> 通常はUITutorialControllerDisplay::SetActiveButtonsと同じ値を渡す </para>
	/// </summary>
	void SetSkipButtons(const std::vector<std::wstring>& buttonKeys) { _skipButtonKeys = buttonKeys; }

	/// <summary>
	/// 現在一文字ずつ出現している最中かどうか
	/// </summary>
	bool IsRevealing() const;

	/// <summary>
	/// 出現演出を中断し全文表示する
	/// </summary>
	void SkipToFullText();

	/// <summary>
	/// アンカー座標を設定する(画面下中央を想定)
	/// </summary>
	void SetAnchorPos(Position2 pos) { SetCenterPos(pos); }

	/// <summary>
	/// スケールを設定する
	/// </summary>
	void SetScale(float scale) { _scale = scale; }

private:

	/// <summary>
	/// 文字列を\nで複数行に分割する
	/// </summary>
	std::vector<std::wstring> SplitLines(const std::wstring& text) const;

	std::wstring _fullText;		// 設定された説明文
	int _visibleCharCount;		// 現在表示している文字数
	float _revealTimer;			// 次の1文字を出すまでの経過時間
	std::vector<std::wstring> _skipButtonKeys;	// 押されたら全文表示にするボタンキー一覧

	float _scale;

	// scaleが1の時のブロック半幅
	float _blockHalfWidthBase;
};
#pragma once
#include "UIBase.h"
#include "Library/System/Tutorial/TutorialManager.h"
#include <vector>

/// <summary>
/// チュートリアル説明目次UI
/// </summary>
class UITutorialTableOfContents : public UIBase
{
public:
	UITutorialTableOfContents(Position2 anchorPos, float scale);
	~UITutorialTableOfContents() override = default;

	void Init() override;
	void Update() override;
	void Draw(Vector2 shakeOffset) override;

	/// <summary>
	/// 目次項目一覧を設定する
	/// </summary>
	void SetTopics(const std::vector<TutorialTocEntry>& topics) { _topics = topics; }

	/// <summary>
	/// 現在の区画IDを設定する
	/// </summary>
	void SetCurrentRegion(int regionId) { _currentRegionId = regionId; }

	/// <summary>
	/// アンカー座標を設定する
	/// </summary>
	void SetAnchorPos(Position2 pos) { SetCenterPos(pos); }

	/// <summary>
	/// スケールを設定する
	/// </summary>
	void SetScale(float scale) { _scale = scale; }

private:

	std::vector<TutorialTocEntry> _topics;
	int _currentRegionId;

	float _scale;
};

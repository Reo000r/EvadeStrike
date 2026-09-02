#pragma once
#include "UIBase.h"
#include "Objects/Character/Player/PlayerComboHolder.h"
#include <vector>

/// <summary>
/// コンボルート表示UI
/// </summary>
class UITutorialComboRoute : public UIBase
{
public:
	UITutorialComboRoute(Position2 anchorPos, float scale);
	~UITutorialComboRoute() override = default;

	void Init() override;
	void Update() override;
	void Draw(Vector2 shakeOffset) override;

	/// <summary>
	/// 表示するコンボ一覧を設定する
	/// </summary>
	void SetComboList(const std::vector<PlayerComboHolder::ComboData_t>& comboList) { _comboList = comboList; }

	/// <summary>
	/// アンカー座標を設定する
	/// </summary>
	void SetAnchorPos(Position2 pos) { SetCenterPos(pos); }

	/// <summary>
	/// スケールを設定する
	/// </summary>
	void SetScale(float scale) { _scale = scale; }

private:

	/// <summary>
	/// PlayerActionKindを表示用のラベルに変換する
	/// </summary>
	std::wstring ActionKindToLabel(PlayerActionKind kind);

	/// <summary>
	/// 1つのコンボを文字列に変換する
	/// </summary>
	std::wstring BuildComboRouteText(const PlayerComboHolder::ComboData_t& combo);

	std::vector<PlayerComboHolder::ComboData_t> _comboList;
	float _scale;
};

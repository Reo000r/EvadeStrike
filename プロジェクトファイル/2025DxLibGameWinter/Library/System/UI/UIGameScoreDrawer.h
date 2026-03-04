#pragma once
#include "UIScoreDrawer.h"
#include "Objects/Character/Player/PlayerComboHolder.h"
#include <vector>
#include <memory>

class UIComboAction;
class UIComboScore;

class UIGameScoreDrawer : public UIScoreDrawer
{
public:
	UIGameScoreDrawer();

	void Init() override;
	void Update() override;
	void Draw(Vector2 shakeOffset) override;

	void SetPlayerComboHolder(std::weak_ptr<PlayerComboHolder> comboHolder);

	/// <summary>
	/// 現在のコンボを描画に追加
	/// </summary>
	/// <param name="action"></param>
	void AddDrawComboAction();
	/// <summary>
	/// 現在の攻撃を描画に追加
	/// </summary>
	/// <param name="action"></param>
	void AddDrawComboScore();

	void ResetActionList();

private:

	/// <summary>
	/// コンボスコアの表示位置を更新
	/// </summary>
	void UpdateComboScoreBasePos();
	/// <summary>
	/// コンボ表示の更新
	/// </summary>
	void UpdateCurrentAction();
	/// <summary>
	/// スコア表示の更新
	/// </summary>
	void UpdateScoreValue();
	/// <summary>
	/// PlayerComboHolderから現在の攻撃を取得
	/// </summary>
	PlayerActionKind GetCurrentAction();

	int GetActionGraph(PlayerActionKind kind);

	Position2 _drawComboScoreBasePos;
	int _lastScoreListSize;
	std::vector<PlayerActionKind> _lastComboList;
	std::vector<std::weak_ptr<UIComboAction>> _drawComboActionUIList;
	std::vector<std::weak_ptr<UIComboScore>> _drawComboScoreUIList;

	std::weak_ptr<PlayerComboHolder> _comboHolder;
};


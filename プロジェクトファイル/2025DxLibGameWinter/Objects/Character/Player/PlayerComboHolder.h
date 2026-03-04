#pragma once
#include <vector>
#include <string>

/// <summary>
/// アクション種
/// </summary>
enum class PlayerActionKind : int {
	Punch = 0,	// 弱攻撃
	Kick = 1,	// 強攻撃
	Special = 2,// 特殊攻撃
	None = -1,	// 不正な入力情報
};

/// <summary>
/// <para> プレイヤーの攻撃がどのように遷移したか保存する </para>
/// <para> 現在の入力がコンボになっているか判定する </para>
/// </summary>
class PlayerComboHolder
{
public:
	PlayerComboHolder();
	~PlayerComboHolder() = default;

	/// <summary>
	/// コンボ情報を読み込む
	/// </summary>
	void LoadCombo();

	/// <summary>
	/// アクション追加
	/// </summary>
	/// <param name="action"></param>
	void AddAction(PlayerActionKind action);
	/// <summary>
	/// コンボが成り立っているかチェック
	/// </summary>
	/// <returns></returns>
	bool CheckComboConsist();
	/// <summary>
	/// 引数の入力があった場合コンボが成り立っているかチェック
	/// </summary>
	/// <param name="action"></param>
	/// <returns></returns>
	bool CheckComboConsist(PlayerActionKind action);
	/// <summary>
	/// コンボデータを削除
	/// </summary>
	/// <returns></returns>
	void ResetActionData();

	// コンボデータ一つ分
	using ComboData_t = std::vector<PlayerActionKind>;

	ComboData_t GetActionList() const ;

private:

	/// <summary>
	/// 引数の文字列をデータ化して返す
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	PlayerComboHolder::ComboData_t DataSplit(std::wstring str);

	// 攻撃遷移保存
	ComboData_t _actions;
	// 定められたコンボ一覧
	std::vector<ComboData_t> _comboList;
};


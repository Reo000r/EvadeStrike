#pragma once
#include "Library/Geometry/Vector3.h"
#include "Library/Geometry/Vector2.h"

#include <map>
#include <string>
#include <vector>

/// <summary>
/// 入力系をコントロールするシングルトンクラス
/// </summary>
class Input final {
private:
	Input();
	Input(const Input&) = delete;
	void operator=(const Input&) = delete;

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static Input& GetInstance();

	/// <summary>
	/// 入力情報の更新
	/// </summary>
	void Update();

	// 周辺機器種別
	enum class PeripheralType {
		keybd,	// キーボード
		pad1	// パッド1
	};

	// 押されたかどうかを記録していくもの
	using InputRecord_t = std::map<std::string, bool>;

	/// <summary>
	/// ゲーム起動時にデフォルトにする入力種別を設定
	/// </summary>
	/// <param name="type"></param>
	void SetInputType(const PeripheralType type);

	/// <summary>
	/// 最後に入力された機器を返す
	/// </summary>
	/// <returns></returns>
	PeripheralType GetLastInputType() const;

	/// <summary>
	/// 押されているかどうかの取得
	/// </summary>
	/// <param name="key">判定を行うボタン</param>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsPress(const std::string key) const;
	/// <summary>
	/// 押されているかどうかの取得
	/// </summary>
	/// <param name="record">入力情報</param>
	/// <param name="key">判定を行うボタン</param>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsPress(const InputRecord_t record, const std::string key) const;
	/// <summary>
	/// 押された瞬間かどうかの取得
	/// </summary>
	/// <param name="key">判定を行うボタン</param>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsTrigger(const std::string key) const;

	/// <summary>
	/// 右スティックの入力情報をVector2型で返す
	/// </summary>
	/// <returns>右スティックの入力情報</returns>
	Vector2 GetPadRightSitck() const;
	/// <summary>
	/// <para> 右スティックの入力情報をVector2型で返す </para>
	/// <para> 直前にあった入力を返す </para>
	/// </summary>
	/// <returns>右スティックの入力情報</returns>
	Vector2 GetPadRightSitckLast() const;

	/// <summary>
	/// 左スティックの入力情報をVector2型で返す
	/// </summary>
	/// <returns>左スティックの入力情報</returns>
	Vector2 GetPadLeftStick() const;
	/// <summary>
	/// <para> 左スティックの入力情報をVector2型で返す </para>
	/// <para> 直前にあった入力を返す </para>
	/// </summary>
	/// <returns>左スティックの入力情報</returns>
	Vector2 GetPadLeftSitckLast() const;

	/// <summary>
	/// 右クリックが行われているか取得
	/// </summary>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsPressMouseRightClick() const;
	/// <summary>
	/// 左クリックが行われているか取得
	/// </summary>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsPressMouseLeftClick() const;
	/// <summary>
	/// ホイールクリックが行われているか取得
	/// </summary>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsPressMouseMiddleClick() const;

	/// <summary>
	/// 右クリックが行われた瞬間であるか取得
	/// </summary>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsTriggerMouseRightClick() const;
	/// <summary>
	/// 左クリックが行われた瞬間であるか取得
	/// </summary>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsTriggerMouseLeftClick() const;
	/// <summary>
	/// ホイールクリックが行われた瞬間であるか取得
	/// </summary>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsTriggerMouseMiddleClick() const;

	/// <summary>
	/// マウスの位置情報をVector2型で返す
	/// </summary>
	/// <returns></returns>
	Vector2 GetMousePosition() const;
	/// <summary>
	/// <para> マウスの位置情報をVector2型で返す </para>
	/// <para> 直前にあった入力を返す </para>
	/// </summary>
	/// <returns></returns>
	Vector2 GetMousePositionLast() const;


	/// <summary>
	/// 現在押しているかどうかの入力を返す
	/// </summary>
	/// <returns></returns>
	InputRecord_t GetCurrentInputRecord() { return _current; }
	/// <summary>
	/// 直前に押されていたかどうかの入力を返す
	/// </summary>
	/// <returns></returns>
	InputRecord_t GetLastInputRecord() { return _last; }

private:

	/// <summary>
	/// 入力情報定義用
	/// </summary>
	struct InputState {
		PeripheralType type;	// 周辺機器種別
		int id;					// 実際の入力を表す定数
	};

	using InputTable_t = std::map<std::string, std::vector<InputState>>;
	InputTable_t _inputTable;		// 名前と実際の入力の対応表
	InputTable_t _tempInputTable;	// 名前と実際の入力の対応表(一時的なコピー)

	/// <summary>
	/// 入力を初期値に設定する
	/// </summary>
	void SetDefault();

	InputRecord_t _current;		// 現在押してるかどうか
	InputRecord_t _last;		// 直前に押されていたかどうか

	// 最後に入力された機器
	PeripheralType _lastInputType;

	// 入力保存
	int _currentRawPadState;			// 今の生パッドステート
	int _lastRawPadState;				// 直前の生パッドステート
	char _currentRawKeybdState[256];	// 今のキーボード状態
	char _lastRawKeybdState[256];		// 直前のキーボード状態

	// 左右スティック入力情報
	Vector2 _currentRightStickInput;
	Vector2 _lastRightStickInput;
	Vector2 _currentLeftStickInput;
	Vector2 _lastLeftStickInput;

	// マウス入力情報
	int _currentRawMouseState;
	int _lastRawMouseState;
	Vector2 _currentMousePosition;
	Vector2 _lastMousePosition;

	/// <summary>
	/// <para> 現在のキーボード状態を調べて最も若いキーコードを返す </para>
	/// <para> ひとつも押されてなければ-1を返す </para>
	/// </summary>
	/// <returns></returns>
	int GetKeyboradState()const;

	/// <summary>
	/// <para> 現在のPAD状態を調べて最も若いキー入力を返す </para>
	/// <para> ひとつも押されてなければ-1を返す </para>
	/// </summary>
	int GetPadState(int padno)const;

	/// <summary>
	/// 現在の入力テーブルをセーブする
	/// </summary>
	void SaveInputTable();

	/// <summary>
	/// 入力テーブルをロードして反映する
	/// </summary>
	void LoadInputTable();
};
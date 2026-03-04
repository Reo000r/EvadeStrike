#pragma once
#include <deque>
#include "Input.h"

/// <summary>
/// 指定フレームまでの入力情報を保存するシングルトンクラス
/// </summary>
class InputStateHolder
{
private:
	InputStateHolder();
	InputStateHolder(const InputStateHolder&) = delete;
	void operator=(const InputStateHolder&) = delete;

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static InputStateHolder& GetInstance();

	/// <summary>
	/// 入力情報の更新
	/// </summary>
	void Update();

	/// <summary>
	/// 最大入力保存数を返す
	/// </summary>
	/// <returns></returns>
	int GetMaxHoldRecord() const;

	/// <summary>
	/// 保存されている入力情報をすべて返す
	/// </summary>
	/// <returns></returns>
	std::deque<Input::InputRecord_t> GetInputRecords() const { return _records; }
	/// <summary>
	/// <para> 指定された要素番目の入力情報を返す </para>
	/// <para> 0からmax-1 </para>
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Input::InputRecord_t GetInputRecord(int num) const;

	/// <summary>
	/// 指定要素数までの最短の入力フレーム数を返す
	/// </summary>
	/// <param name="inputName">入力名</param>
	/// <param name="frame">探索要素数</param>
	/// <returns>入力されていたなら要素数、されていなければ-1</returns>
	int GetPressFrame(std::string inputName, float frame) const;
	// MEMO:引数名が雑

	/// <summary>
	/// 入力の流れを取り始める
	/// </summary>
	void StartCurrentRecords();
	/// <summary>
	/// 入力の流れの情報をリセットする
	/// </summary>
	void ResetCurrentRecords();

private:
	// 先頭への要素追加、末尾要素削除を行うためdeque
	// https://cpprefjp.github.io/reference/deque/deque.html
	
	// 入力情報の保存
	// 攻撃が遷移した場合は一つ要素削除
	std::deque<Input::InputRecord_t> _records;

	// 現在の一連の入力の流れ
	// 要素があったら追加、なければ追加しない
	// 要請があったら要素削除
	std::deque<Input::InputRecord_t> _currentRecords;
};


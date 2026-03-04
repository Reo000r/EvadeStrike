#pragma once
#include "EnemyWeakStateBase.h"

class AttackCol;

class EnemyWeakStateAttack : public EnemyWeakStateBase
{
public:
	EnemyWeakStateAttack(std::weak_ptr<EnemyWeak> parent,
		std::shared_ptr<AttackCol> attackCol);
	~EnemyWeakStateAttack() = default;

	/// <summary>
	/// ステート開始直後の処理
	/// </summary>
	void OnEnter() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// ステート終了直前の処理
	/// </summary>
	void OnExit() override;

protected:
	// このステート内で変更可能な攻撃判定
	std::shared_ptr<AttackCol> _attackCol;

private:
	/// <summary>
	/// <para> ステートの遷移条件を確認する </para>
	/// <para> 変更可能なステートがあればそのポインタを返す </para>
	/// <para> 変更がなければnullptrを返す </para>
	/// </summary>
	std::shared_ptr<EnemyWeakStateBase> CheckStateTransition() override;
};


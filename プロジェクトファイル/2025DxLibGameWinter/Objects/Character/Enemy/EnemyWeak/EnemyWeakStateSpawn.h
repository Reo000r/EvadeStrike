#pragma once
#include "EnemyWeakStateBase.h"

class EnemyWeakStateSpawn : public EnemyWeakStateBase
{
public:
	EnemyWeakStateSpawn(std::weak_ptr<EnemyWeak> parent);
	~EnemyWeakStateSpawn() = default;

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

private:

	/// <summary>
	/// <para> ステートの遷移条件を確認する </para>
	/// <para> 変更可能なステートがあればそのポインタを返す </para>
	/// <para> 変更がなければnullptrを返す </para>
	/// </summary>
	std::shared_ptr<EnemyWeakStateBase> CheckStateTransition() override;
};


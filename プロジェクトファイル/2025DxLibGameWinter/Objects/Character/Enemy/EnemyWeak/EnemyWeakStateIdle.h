#pragma once
#include "EnemyWeakStateBase.h"

class EffekseerEffect;

class EnemyWeakStateIdle : public EnemyWeakStateBase
{
public:
	EnemyWeakStateIdle(std::weak_ptr<EnemyWeak> parent);
	~EnemyWeakStateIdle() = default;

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

	// 攻撃時に発生する追従させるエフェクト
	std::weak_ptr<EffekseerEffect> _currentEffect;
};


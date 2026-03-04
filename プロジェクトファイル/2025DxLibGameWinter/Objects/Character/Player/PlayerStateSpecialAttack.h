#pragma once
#include "PlayerStateAttack.h"

class PlayerStateSpecialAttack : public PlayerStateAttack
{
public:
	PlayerStateSpecialAttack(std::weak_ptr<Player> player,
		PlayerAttackAnimData data, std::shared_ptr<AttackCol> attackCol);
	~PlayerStateSpecialAttack() = default;

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
	std::shared_ptr<PlayerStateBase> CheckStateTransition() override;
};


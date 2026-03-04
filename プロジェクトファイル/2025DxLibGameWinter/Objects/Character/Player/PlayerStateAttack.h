#pragma once
#include "PlayerStateBase.h"
#include "PlayerAttackAnimData.h"
#include "PlayerComboHolder.h"

class AttackCol;
class EffekseerEffect;

class PlayerStateAttack : public PlayerStateBase
{
public:
	PlayerStateAttack(std::weak_ptr<Player> player,
		PlayerAttackAnimData data, std::shared_ptr<AttackCol> attackCol, 
		PlayerActionKind actionKind);
	~PlayerStateAttack() = default;

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

	void SetData(PlayerAttackAnimData data) { _data = data; }
	PlayerAttackAnimData GetData() { return _data; }

protected:
	// idやアニメーションなどが入ったデータ
	PlayerAttackAnimData _data;

	// このステート内で変更可能な攻撃判定
	std::shared_ptr<AttackCol> _attackCol;

	// 攻撃判定を生成する基準の行列
	Matrix4x4 _attackColParentMatrix;

	// 攻撃時に発生する追従させるエフェクト
	std::weak_ptr<EffekseerEffect> _currentEffect;

private:
	/// <summary>
	/// <para> ステートの遷移条件を確認する </para>
	/// <para> 変更可能なステートがあればそのポインタを返す </para>
	/// <para> 変更がなければnullptrを返す </para>
	/// </summary>
	std::shared_ptr<PlayerStateBase> CheckStateTransition() override;
};


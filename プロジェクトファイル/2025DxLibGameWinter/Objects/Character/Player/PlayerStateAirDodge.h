#pragma once
#include "PlayerStateBase.h"

class JustDodgeCollider;
class EffekseerEffect;

class PlayerStateAirDodge : public PlayerStateBase
{
public:
	PlayerStateAirDodge(std::weak_ptr<Player> player);
	~PlayerStateAirDodge() = default;

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

	/// <summary>
	/// ジャスト回避が行えるか
	/// </summary>
	/// <param name="currentFrameRate">アニメーション進行度</param>
	/// <returns></returns>
	bool CanJustDodgeColActive(float currentFrameRate);

	void StartJustDodge();

	// 攻撃が当たった場合ジャスト回避を行う判定
	std::shared_ptr<JustDodgeCollider> _justDodgeCollider;

	// 回避エフェクト
	std::weak_ptr<EffekseerEffect> _currentEffect;
};
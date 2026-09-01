#include "EnemyBossStateChase.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"

EnemyBossStateChase::EnemyBossStateChase(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent)
{
}

void EnemyBossStateChase::OnEnter()
{
	// 移動量をゼロにする
	Stop();

	// 走りアニメーションを再生する
	GetAnimator()->ChangeAnim(kAnimNameDash, true, 1.0f);
}

void EnemyBossStateChase::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// プレイヤーの方向へゆっくり回転する
	RotateToPlayer(kBossTurnSpeed);

	// 向いている方向へ前進する
	AddTransformForward(kBossChaseSpeed);
}

void EnemyBossStateChase::OnExit()
{
	// 特になし
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateChase::CheckStateTransition()
{
	if (GetParentPtr()->HasAttackAuthority())
	{
		// 攻撃権あり：近接攻撃範囲に入ったら待機ステートへ（攻撃判断を Idle に委ねる）
		if (CanAttackRangeInPlayer(kBossAttack1Range))
		{
			return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
		}
	}
	else
	{
		// 攻撃権なし：待機距離内に入ったら待機ステートへ
		if (CanAttackRangeInPlayer(kBossIdleRange))
		{
			return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
		}
	}

	return nullptr;
}

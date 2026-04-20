#include "EnemyBossStateIdle.h"
#include "EnemyBossStateChase.h"
#include "EnemyBossStateAttack1.h"
#include "EnemyBossStateAttack2.h"
#include "EnemyBossStateAttack3.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/TimeScaleManager.h"
#include <DxLib.h>

namespace
{
	// 攻撃予告エフェクト生成タイミング（残り秒数）
	constexpr float kAttackNoticeEffectTime = 0.5f;

	// 攻撃種選択の乱数境界（フェーズ2時）
	// 0〜99のうち
	//   0〜39  → Attack2（範囲）
	//   40〜69 → Attack3（ブレス）
	//   70〜99 → Attack1（近接）
	constexpr int kPhase2Attack2Boundary = 40;
	constexpr int kPhase2Attack3Boundary = 70;
}

EnemyBossStateIdle::EnemyBossStateIdle(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent)
{
}

void EnemyBossStateIdle::OnEnter()
{
	// ステート遷移待機時間を設定する
	// フェーズ2では短縮する
	float transTime = kBossStateTransitionTime;
	if (GetPhase() >= 2)
	{
		transTime *= kPhase2IntervalRate;
	}
	SetStateTransitionTime(transTime);

	// 移動量をゼロにする
	Stop();

	// 待機アニメーションを再生する
	GetAnimator()->ChangeAnim(kAnimNameIdle, true, 1.0f);

	// プレイヤーの方向をゆっくり向く
	RotateToPlayer(kBossTurnSpeed);
}

void EnemyBossStateIdle::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// プレイヤーの方向をゆっくり向き続ける
	RotateToPlayer(kBossTurnSpeed);

	// 攻撃予告エフェクトの生成タイミングを確認する
	if (!_attackNoticeEffect.lock())
	{
		float noticeTime = kAttackNoticeEffectTime
			* TimeScaleManager::GetInstance().GetOtherCurrentScale();

		// 攻撃権があり、各タイマーが残り指定秒を切っていれば予告エフェクトを出す
		if (GetAttackInterval() <= noticeTime &&
			GetStateTransitionTime() <= noticeTime &&
			GetParentPtr()->HasAttackAuthority())
		{
			Position3 generatePos = GetParentPtr()->GetCenterPos();
			generatePos.y += 200;	// 頭部に寄せる
			_attackNoticeEffect = EffectManager::GetInstance().GenerateEffect(
				"Atk_Breakdown.efkefc", generatePos);
			if (_attackNoticeEffect.lock())
			{
				_attackNoticeEffect.lock()->SetPlaySpeed(0.4f);
				_attackNoticeEffect.lock()->SetScale(Vector3(0.8f, 0.8f, 0.8f));
			}
		}
	}
	else
	{
		// エフェクト位置を追従させる
		if (_attackNoticeEffect.lock() && _attackNoticeEffect.lock()->IsPlaying()) {
			Position3 pos = GetParentPtr()->GetCenterPos();
			pos.y += 200;
			_attackNoticeEffect.lock()->SetPos(pos);
		}
	}
}

void EnemyBossStateIdle::OnExit()
{
	// 予告エフェクトを停止する
	if (_attackNoticeEffect.lock()) {
		_attackNoticeEffect.lock()->DeleteEffect();
	}
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateIdle::CheckStateTransition()
{
	// 攻撃が可能な場合、距離とフェーズに応じて攻撃種を選択する
	if (CanAttack())
	{
		bool inAttack1Range = CanAttackRangeInPlayer(kBossAttack1Range);
		bool inAttack2Range = CanAttackRangeInPlayer(kBossAttack2Range);
		bool inAttack3Range = CanAttackRangeInPlayer(kBossAttack3Range);

		if (GetPhase() >= 2)
		{
			// フェーズ2では乱数で攻撃種を選択する（距離条件に関係なく選ぶ）
			int r = GetRand(99);
			if (r < kPhase2Attack2Boundary && inAttack2Range)
			{
				return std::make_shared<EnemyBossStateAttack2>(GetParentPtr());
			}
			else if (r < kPhase2Attack3Boundary && inAttack3Range)
			{
				return std::make_shared<EnemyBossStateAttack3>(GetParentPtr());
			}
			else if (inAttack1Range)
			{
				return std::make_shared<EnemyBossStateAttack1>(GetParentPtr());
			}
			// いずれの距離条件も満たさない場合は攻撃しない（Chaseに任せる）
		}
		else
		{
			// フェーズ1では距離優先で攻撃種を決定する
			if (inAttack1Range)
			{
				// 至近距離なら近接攻撃
				return std::make_shared<EnemyBossStateAttack1>(GetParentPtr());
			}
			else if (inAttack2Range)
			{
				// 中距離なら範囲攻撃
				return std::make_shared<EnemyBossStateAttack2>(GetParentPtr());
			}
			else if (inAttack3Range)
			{
				// 遠距離ならブレス攻撃
				return std::make_shared<EnemyBossStateAttack3>(GetParentPtr());
			}
		}
	}

	// 遷移待機時間が経過していれば追跡・待機の切り替えを確認する
	if (IsNothingStateTransitionTime())
	{
		if (GetParentPtr()->HasAttackAuthority())
		{
			// 攻撃権あり：攻撃1範囲外なら追跡へ
			if (!CanAttackRangeInPlayer(kBossAttack1Range))
			{
				return std::make_shared<EnemyBossStateChase>(GetParentPtr());
			}
		}
		else
		{
			// 攻撃権なし：待機範囲外なら追跡へ
			if (!CanAttackRangeInPlayer(kBossIdleRange))
			{
				return std::make_shared<EnemyBossStateChase>(GetParentPtr());
			}
		}
	}

	return nullptr;
}

#include "EnemyBossStateAttack3.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Library/System/Statistics.h"
#include "Library/System/SoundManager.h"

namespace
{
	// 発射を開始するアニメーション進行度
	constexpr float kFireStartRate = 0.25f;

	// フェーズ2時の発射回数増加量
	constexpr int kPhase2ExtraFireCount = 4;
}

EnemyBossStateAttack3::EnemyBossStateAttack3(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent),
	_fireTimer(0.0f),
	_remainFireCount(kBressFireCount),
	_firingDone(false)
{
}

void EnemyBossStateAttack3::OnEnter()
{
	// 移動量をゼロにする
	Stop();

	// ブレス攻撃アニメーションを再生する
	// （ブレス専用アニメーションが無い場合は SpecialAttack2 等を流用すること）
	GetAnimator()->ChangeAnim(kAnimNameSpecialAttack2, false, 1.0f);

	// プレイヤーの方向へ即時回転する（発射前に正面を合わせる）
	RotateToPlayer();

	// 発射カウントを設定する
	_remainFireCount = kBressFireCount;
	if (GetPhase() >= 2)
	{
		// フェーズ2では発射回数を増やす
		_remainFireCount += kPhase2ExtraFireCount;
	}

	// 発射タイマーを0にする（最初の弾はすぐ発射できるようにしない、溜め後に発射する）
	_fireTimer = 0.0f;
	_firingDone = false;
}

void EnemyBossStateAttack3::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	const std::shared_ptr<AnimationModel::AnimData> animData =
		GetAnimator()->GetCurrentAnimData();
	const float rate = animData->frame / animData->totalFrame;

	// 発射フェーズが終わっていなければ弾の発射処理を行う
	if (!_firingDone)
	{
		// 発射開始進行度に達していなければ発射しない（溜め中はプレイヤーを向く）
		if (rate < kFireStartRate)
		{
			RotateToPlayer(kBossTurnSpeed);
			return;
		}

		// 発射インターバルを加算する
		float timeScale = GetParentPtr()->GetCurrentTimeScale();
		_fireTimer += timeScale / Statistics::kFPS;

		// 発射間隔に達したら弾を発射する
		if (_fireTimer >= kBressFireInterval)
		{
			_fireTimer -= kBressFireInterval;

			// 弾を発射する
			FireBressProjectile();
			SoundManager::GetInstance().PlaySoundType(SEType::AttackSwing);

			--_remainFireCount;

			// 指定回数発射し終えたら発射フェーズを終了する
			if (_remainFireCount <= 0)
			{
				_firingDone = true;
			}
		}
	}
	// 発射完了後はアニメーション終了を待つのみ（遷移は CheckStateTransition が担う）
}

void EnemyBossStateAttack3::OnExit()
{
	// 攻撃インターバルを設定する（最も長め）
	float interval = kBossAttack3Interval;
	if (GetPhase() >= 2)
	{
		interval *= kPhase2IntervalRate;
	}
	SetAttackInterval(interval);
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateAttack3::CheckStateTransition()
{
	// 発射が完了しており、かつアニメーションが終了していれば遷移する
	if (_firingDone && GetAnimator()->GetCurrentAnimData()->isEnd)
	{
		return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
	}

	return nullptr;
}

#include "EnemyBossStateAttack3.h"
#include "Objects/Projectile/EnemyBossBressProjectile.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/Geometry/Quaternion.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Statistics.h"
#include "Library/System/SoundManager.h"
#include "Library/System/AttackRangeIndicator.h"
#include <DxLib.h>

namespace {
	// 発射を開始するアニメーション進行度
	constexpr float kFireStartRate = 0.15f;

	// ブレス弾の発射間隔(秒)
	constexpr float kBressFireInterval = 0.1f;

	// ブレス弾の発射回数
	constexpr int kBressFireCount = 16;
	// フェーズ2時の発射回数増加量
	constexpr float kPhase2ExtraFireMul = 1.5f;

	// ブレスの許容する最大角度差
	const float kMaxAngleDiff = Calc::ToRadian(10.0f);
	// 0とみなす閾値
	const float kAngleZeroTolerance = 0.0001f;

	// 攻撃範囲
	constexpr float kIndicatorWidth = 200.0f;
	constexpr float kIndicatorLength = 2400.0f;
	const Vector3 kRangeIndicatorOffset = Vector3(0, -720, 0);
	// 弾ごとの攻撃範囲のフェード時間(秒)
	constexpr float kIndicatorFadeDuration = 0.15f;
}

EnemyBossStateAttack3::EnemyBossStateAttack3(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent),
	_fireTimer(0.0f),
	_remainFireCount(kBressFireCount),
	_firingDone(false),
	_fireDir(Vector3(0,0,0))
{
}

void EnemyBossStateAttack3::OnEnter()
{
	// 履歴をリセット
	_projectileList.clear();
	_rangeIndicators.clear();

	// 移動量をなくす
	Stop();

	// アニメーション再生
	GetAnimator()->ChangeAnim(BossAnimData::kAnimNameBressAttack, false, 1.0f);

	// プレイヤーの方へ向く
	RotateToPlayer();

	// フェーズ2なら発射回数を増やす
	if (GetPhase() >= 2) {
		_remainFireCount *= kPhase2ExtraFireMul;
	}
}

void EnemyBossStateAttack3::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// ブレス関連の更新
	UpdateBress();

	// 発射済みの攻撃範囲の不透明度を更新する
	UpdateIndicators();
}

void EnemyBossStateAttack3::OnExit()
{
	// 攻撃インターバルを設定する
	float interval = kBossAttack3Interval;
	if (GetPhase() >= 2) {
		interval *= kPhase2IntervalRate;
	}
	SetAttackInterval(interval);
}

void EnemyBossStateAttack3::DrawAttackRangeIndicator() const
{
	for (const auto& entry : _rangeIndicators) {
		entry.indicator->Draw();
	}
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateAttack3::CheckStateTransition()
{
	// 発射が完了しているかつ
	// アニメーションが終了していれば
	if (_firingDone && 
		GetAnimator()->GetCurrentAnimData()->isEnd) {
		// 遷移する
		return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
	}

	return nullptr;
}

void EnemyBossStateAttack3::UpdateBress()
{
	const std::shared_ptr<AnimationModel::AnimData> animData =
		GetAnimator()->GetCurrentAnimData();
	const float rate = animData->frame / animData->totalFrame;

	// 発射フェーズが終わっていればreturn
	if (_firingDone) return;

	// 発射開始進行度に達していなければ発射しない
	if (rate < kFireStartRate) {
		RotateToPlayer(kBossTurnSpeed);
		return;
	}

	// ジャスト回避中は攻撃判定を無効化する
	if (GetEnemyManager().lock()->ShouldBeDisableAttackCol()) {
		for (auto& projectile : _projectileList) {
			if (projectile.expired()) continue;
			if (projectile.lock()->CanDelete()) continue;
			if (projectile.lock()->GetCollisionState()) {
				projectile.lock()->SetCollisionState(false);
			}
		}
	}
	else {
		for (auto& projectile : _projectileList) {
			if (projectile.expired()) continue;
			if (projectile.lock()->CanDelete()) continue;
			projectile.lock()->SetCollisionState(true);
		}
	}

	// 発射インターバルを加算する
	float timeScale = GetParentPtr()->GetCurrentTimeScale();
	_fireTimer += timeScale / Statistics::kFPS;

	// 発射間隔に達したら弾を発射する
	if (_fireTimer >= kBressFireInterval) {
		// 初弾であれば
		if (_remainFireCount >= kBressFireCount) {
			// 発射方向をプレイヤーの方向に向ける
			Vector3 momentDir = GetMomentBressDir();
			_fireDir = momentDir;
		}
		else {
			// momentDirの角度をfireDirの一定範囲内に収める
			Vector3 momentDir = GetMomentBressDir();

			// _fireDirとmomentDirの内積から現在の角度差を求める
			Vector3 normFireDir = _fireDir.Normalize();
			Vector3 normMomentDir = momentDir.Normalize();
			float dotVal = Dot(normFireDir, normMomentDir);

			// 数値誤差対策でクランプ
			if (dotVal > 1.0f) dotVal = 1.0f;
			else if (dotVal < -1.0f) dotVal = -1.0f;

			float angleDiff = std::acos(dotVal);

			// 角度差が許容範囲内であればそのまま使う
			if (angleDiff <= kMaxAngleDiff) {
				_fireDir = momentDir;
			}
			else {
				// _fireDirからmomentDir方向へ最大角度だけ回転させる
				// 回転軸 = _fireDir * momentDir
				Vector3 rotAxis = Cross(normFireDir, normMomentDir).Normalize();

				// 回転軸がゼロベクトルになる場合(逆方向)
				if (rotAxis.SqrMagnitude() < kAngleZeroTolerance) {
					// 任意の直交軸を使う
					rotAxis = Cross(normFireDir, Vector3Up()).Normalize();
					if (rotAxis.SqrMagnitude() < kAngleZeroTolerance) {
						rotAxis = Cross(normFireDir, Vector3Right()).Normalize();
					}
				}

				// 回転軸、角度からクォータニオンを生成し、_fireDirを回転させる
				Quaternion rotQ = AngleAxis(rotAxis, kMaxAngleDiff);
				_fireDir = (rotQ * normFireDir).Normalize();
			}
		}

		// 発射間隔タイマーを更新
		_fireTimer -= kBressFireInterval;

		// 弾を発射する
		std::weak_ptr<EnemyBossBressProjectile> firedProjectile = FireBressProjectile(_fireDir);
		_projectileList.emplace_back(firedProjectile);
		// 発射した弾用の攻撃範囲を生成する
		SpawnIndicatorForProjectile(firedProjectile, _fireDir);
		SoundManager::GetInstance().PlaySoundType(SEType::AttackSwing);

		--_remainFireCount;

		// 指定回数発射し終えたら発射フェーズを終了する
		if (_remainFireCount <= 0) {
			_firingDone = true;
		}
	}
}

void EnemyBossStateAttack3::SpawnIndicatorForProjectile(const std::weak_ptr<EnemyBossBressProjectile>& projectile, Vector3 fireDir)
{
	if (projectile.expired()) return;

	// 発射位置と発射方向から攻撃範囲を生成する
	Position3 spawnPos = projectile.lock()->GetPos() + kRangeIndicatorOffset;
	float rotY = atan2f(fireDir.x, fireDir.z);

	BreathIndicatorEntry entry;
	entry.indicator = std::make_shared<AttackRangeIndicator>();
	entry.indicator->InitAsRect(spawnPos, kIndicatorWidth, kIndicatorLength, rotY);
	entry.elapsedTime = 0.0f;
	_rangeIndicators.emplace_back(entry);
}

void EnemyBossStateAttack3::UpdateIndicators()
{
	float timeScale = GetParentPtr()->GetCurrentTimeScale();
	float deltaTime = timeScale / Statistics::kFPS;

	for (auto& entry : _rangeIndicators) {
		entry.elapsedTime += deltaTime;
		entry.indicator->UpdateAlphaByTime(entry.elapsedTime, kIndicatorFadeDuration);
	}
}

#include "EnemyWeakStateIdle.h"
#include "EnemyWeakStateChase.h"
#include "EnemyWeakStateAttack.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/TimeScaleManager.h"

namespace {
	// 攻撃予告エフェクト生成タイミング
	constexpr float kAttackNoticeEffectTime = 0.4f;
}

EnemyWeakStateIdle::EnemyWeakStateIdle(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent)
{
}

void EnemyWeakStateIdle::OnEnter()
{
	// ステート遷移時間を設定
	SetStateTransitionTime(kStateTransitionTime);

	// 移動量をなくす
	Stop();

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameIdle, true, 1.0f);

	// プレイヤーの方向を向く
	RotateToPlayer(kTurnSpeed);
}

void EnemyWeakStateIdle::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// プレイヤーの方向を向く
	RotateToPlayer(kTurnSpeed);

	// エフェクトが生成されていない場合
	if (!_currentEffect.lock()) {
		float attackNotiEffTime = kAttackNoticeEffectTime * TimeScaleManager::GetInstance().GetOtherCurrentScale();
		// 攻撃待機時間とステート遷移時間が指定秒を切っているかつ
		// 攻撃権があれば
		if (GetAttackInterval() <= attackNotiEffTime &&
			GetStateTransitionTime() <= attackNotiEffTime &&
			GetParentPtr()->HasAttackAuthority()) {
			// 攻撃予告エフェクトを生成
			Position3 generatePos = GetParentPtr()->GetCenterPos();
			generatePos.y += 150;	// 頭部に寄せる
			_currentEffect = EffectManager::GetInstance().GenerateEffect(
				"Atk_Breakdown.efkefc", generatePos);
			_currentEffect.lock()->SetPlaySpeed(0.5f);
			_currentEffect.lock()->SetScale(Vector3(0.5f, 0.5f, 0.5f));
		}
	}

	// 待機
}

void EnemyWeakStateIdle::OnExit()
{
	// 処理なし
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateIdle::CheckStateTransition()
{
	// 攻撃が可能なら
	if (CanAttack()) {
		// 攻撃ステートに入る
		return std::make_shared<EnemyWeakStateAttack>(GetParentPtr(), GetAttackCol());
	}

	// 一定範囲内に別の敵がいるなら(正常な敵が帰ってきたなら)
	if (!(GetEnemyManager().lock()->IsExistEnemyWithinRange(GetParentPtr()->GetCenterPos(), kRangeEnemyDist)
		.expired())) {
		// 追跡ステートには入らない
		return nullptr;
	}

	// 遷移待機時間がないなら
	if (IsNothingStateTransitionTime()) {

		// 攻撃権を持っている場合は
		if (GetParentPtr()->HasAttackAuthority()) {
			// 攻撃範囲内にプレイヤーがいない場合は
			if (!CanAttackRangeInPlayer(kAttackRangeInPlayer)) {
				// 追跡ステートに入る
				return std::make_shared<EnemyWeakStateChase>(GetParentPtr());
			}
		}
		// 攻撃権を持っていない場合は
		else {
			// 待機範囲内にプレイヤーがいない場合は
			if (!CanAttackRangeInPlayer(kIdleRangeInPlayer)) {
				// 追跡ステートに入る
				return std::make_shared<EnemyWeakStateChase>(GetParentPtr());
			}
		}
	}

	return nullptr;
}

#include "EnemyBossStateBase.h"

EnemyBossStateBase::EnemyBossStateBase(std::weak_ptr<EnemyBoss> parent) :
	_parent(parent)
{
}

bool EnemyBossStateBase::UpdateStateTransition()
{
	std::shared_ptr<EnemyBossStateBase> nextState = CheckStateTransition();
	if (nextState != nullptr) {
		ChangeState(nextState);
		return true;
	}
	return false;
}

void EnemyBossStateBase::ChangeState(std::shared_ptr<EnemyBossStateBase> nextState)
{
	// 現在のステートの終了処理を行う
	OnExit();
	// 次のステートの開始処理を行う
	nextState->OnEnter();
	// ステートを切り替える
	_parent.lock()->_currentState = nextState;
	// これ以降は処理を書かない
	return;
}

std::weak_ptr<Physics> EnemyBossStateBase::GetPhysics() const
{
	return _parent.lock()->_physics;
}

const std::shared_ptr<AnimationModel> EnemyBossStateBase::GetAnimator() const
{
	return _parent.lock()->GetAnimator();
}

const std::weak_ptr<EnemyManager> EnemyBossStateBase::GetEnemyManager() const
{
	return _parent.lock()->_manager;
}

Matrix4x4 EnemyBossStateBase::GetForwardMatrix() const
{
	return _parent.lock()->GetForwardMatrix();
}

std::shared_ptr<AttackCol> EnemyBossStateBase::GetAttackColLight() const
{
	return _parent.lock()->_attackColLight;
}

std::shared_ptr<AttackCol> EnemyBossStateBase::GetAttackColHeavy() const
{
	return _parent.lock()->_attackColHeavy;
}

void EnemyBossStateBase::ReleasePhysics()
{
	_parent.lock()->ReleasePhysics();
}

bool EnemyBossStateBase::CanAttack() const
{
	return _parent.lock()->CanAttack();
}

float EnemyBossStateBase::GetAttackInterval() const
{
	return _parent.lock()->_attackInterval;
}

float EnemyBossStateBase::GetStateTransitionTime() const
{
	return _parent.lock()->_stateTransitionTime;
}

int EnemyBossStateBase::GetPhase() const
{
	return _parent.lock()->GetPhaseNum();
}

void EnemyBossStateBase::AddTransformForward(float add) const
{
	_parent.lock()->AddTransformForward(add);
}

void EnemyBossStateBase::Stop() const
{
	_parent.lock()->Stop();
}

bool EnemyBossStateBase::CanAttackRangeInPlayer(float range)
{
	return _parent.lock()->CanAttackRangeInPlayer(range);
}

bool EnemyBossStateBase::IsNothingStateTransitionTime() const
{
	return _parent.lock()->IsNothingStateTransitionTime();
}

bool EnemyBossStateBase::IsNothingAttackInterval() const
{
	return _parent.lock()->IsNothingAttackInterval();
}

void EnemyBossStateBase::SetStateTransitionTime(float time)
{
	_parent.lock()->SetStateTransitionTime(time);
}

void EnemyBossStateBase::SetAttackInterval(float interval)
{
	_parent.lock()->SetAttackInterval(interval);
}

std::weak_ptr<EnemyBossBressProjectile> EnemyBossStateBase::FireBressProjectile(Vector3 dir)
{
	return _parent.lock()->FireBressProjectile(dir);
}

Vector3 EnemyBossStateBase::GetMomentBressDir()
{
	return _parent.lock()->GetMomentBressDir();
}

bool EnemyBossStateBase::IsGround()
{
	return _parent.lock()->IsGround();
}

#include "EnemyWeakStateBase.h"

EnemyWeakStateBase::EnemyWeakStateBase(std::weak_ptr<EnemyWeak> parent) :
	_parent(parent)
{
}

bool EnemyWeakStateBase::UpdateStateTransition()
{
	// ステート遷移が可能か確認
	std::shared_ptr<EnemyWeakStateBase> nextState = CheckStateTransition();
	// ステート遷移が可能な場合
	if (nextState != nullptr) {
		// ステート変更後は処理を書かない
		ChangeState(nextState);
		return true;
	}
	return false;
}

void EnemyWeakStateBase::ChangeState(std::shared_ptr<EnemyWeakStateBase> nextState)
{
	// このステートの処理を行う
	OnExit();
	// 次のステートの開始直後の処理を行う
	nextState->OnEnter();
	_parent.lock()->_currentState = nextState;

	// 上記の処理を行う事でこのステートは存在しないものとなったので
	// これ以降は処理を書かない
	return;
}

std::weak_ptr<Physics> EnemyWeakStateBase::GetPhysics() const
{
	return _parent.lock()->_physics;
}

const std::shared_ptr<AnimationModel> EnemyWeakStateBase::GetAnimator() const
{
	return _parent.lock()->GetAnimator();
}

const std::weak_ptr<EnemyManager> EnemyWeakStateBase::GetEnemyManager() const
{
	return _parent.lock()->_manager;
}

Matrix4x4 EnemyWeakStateBase::GetForwardMatrix() const
{
	return _parent.lock()->GetForwardMatrix();
}

std::shared_ptr<AttackCol> EnemyWeakStateBase::GetAttackCol() const
{
	return _parent.lock()->_attackCol;
}

void EnemyWeakStateBase::ReleasePhysics()
{
	return _parent.lock()->ReleasePhysics();
}

bool EnemyWeakStateBase::CanAttack() const
{
	return _parent.lock()->CanAttack();
}

float EnemyWeakStateBase::GetAttackInterval() const
{
	return _parent.lock()->_attackInterval;
}

float EnemyWeakStateBase::GetStateTransitionTime() const
{
	return _parent.lock()->_stateTransitionTime;
}

void EnemyWeakStateBase::AddTransformForward(float add) const
{
	return _parent.lock()->AddTransformForward(add);
}

void EnemyWeakStateBase::Stop() const
{
	return _parent.lock()->Stop();
}

bool EnemyWeakStateBase::CanAttackRangeInPlayer(float attackRangeInPlayer)
{
	return _parent.lock()->CanAttackRangeInPlayer(attackRangeInPlayer);
}

bool EnemyWeakStateBase::IsNothingStateTransitionTime() const
{
	return _parent.lock()->IsNothingStateTransitionTime();
}

bool EnemyWeakStateBase::IsNothingAttackInterval() const
{
	return _parent.lock()->IsNothingAttackInterval();
}

void EnemyWeakStateBase::SetStateTransitionTime(float time)
{
	_parent.lock()->SetStateTransitionTime(time);
}

void EnemyWeakStateBase::SetAttackInterval(float interval)
{
	_parent.lock()->SetAttackInterval(interval);
}

bool EnemyWeakStateBase::IsGround()
{
	return _parent.lock()->IsGround();
}

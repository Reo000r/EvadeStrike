#pragma once
#include "EnemyWeakStateBase.h"

class EnemyWeakStateRetreatWait : public EnemyWeakStateBase
{
public:
	EnemyWeakStateRetreatWait(std::weak_ptr<EnemyWeak> parent);
	~EnemyWeakStateRetreatWait() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyWeakStateBase> CheckStateTransition() override;
};

#pragma once
#include "EnemyWeakStateBase.h"

class EnemyWeakStateRetreatMove : public EnemyWeakStateBase
{
public:
	EnemyWeakStateRetreatMove(std::weak_ptr<EnemyWeak> parent);
	~EnemyWeakStateRetreatMove() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyWeakStateBase> CheckStateTransition() override;

	// ‘Þ”ðˆÚ“®‚ÌŽc‚èŽžŠÔ
	float _timer;
};

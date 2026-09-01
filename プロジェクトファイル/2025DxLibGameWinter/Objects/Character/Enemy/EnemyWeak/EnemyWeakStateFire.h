#pragma once
#include "EnemyWeakStateBase.h"

class EnemyWeakStateFire : public EnemyWeakStateBase
{
public:
	EnemyWeakStateFire(std::weak_ptr<EnemyWeak> parent);
	~EnemyWeakStateFire() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyWeakStateBase> CheckStateTransition() override;

	// ‚±‚ÌŽËŒ‚‚ÅŠù‚É’e‚ð”­ŽË‚µ‚½‚©
	bool _hasFired;
};

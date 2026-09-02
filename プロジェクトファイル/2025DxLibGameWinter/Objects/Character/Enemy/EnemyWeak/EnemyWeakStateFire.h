#pragma once
#include "EnemyWeakStateBase.h"

class AttackRangeIndicator;

class EnemyWeakStateFire : public EnemyWeakStateBase
{
public:
	EnemyWeakStateFire(std::weak_ptr<EnemyWeak> parent);
	~EnemyWeakStateFire() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

	/// <summary>
	/// UŒ‚”ÍˆÍƒCƒ“ƒWƒP[ƒ^[‚ğ•`‰æ‚·‚é
	/// </summary>
	void DrawAttackRangeIndicator() const override;

private:
	std::shared_ptr<EnemyWeakStateBase> CheckStateTransition() override;

	// ‚±‚ÌËŒ‚‚ÅŠù‚É’e‚ğ”­Ë‚µ‚½‚©
	bool _hasFired;

	// UŒ‚”ÍˆÍ
	// ”­Ë‚·‚é‚Ü‚Å’Ç]‚·‚é
	std::shared_ptr<AttackRangeIndicator> _rangeIndicator;
};

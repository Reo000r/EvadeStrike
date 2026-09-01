#include "EnemyBossBressProjectile.h"

namespace {
	constexpr float kRadius = 90.0f;
	constexpr float kSpeed = 30.0f;
	constexpr float kMaxLifeTime = 2.0f;
	constexpr float kAttackPower = 5.0f;
	constexpr float kEffectPlaySpeed = 0.85f / kMaxLifeTime;
	constexpr float kEffectScale = kRadius * 0.02f;
}

EnemyBossBressProjectile::EnemyBossBressProjectile(
	std::weak_ptr<Physics> physics,
	const Position3& startPos,
	const Vector3& direction) :
	EnemyProjectile(physics, startPos, direction, kSpeed, kMaxLifeTime)
{
}

void EnemyBossBressProjectile::Init()
{
	// 攻撃判定設定
	AttackColStats stats;
	stats.colRad = kRadius;
	stats.attackPower = kAttackPower;
	SetAttackData(stats);

	// 追従エフェクト設定
	SetTrailEffect("AtkOmen_Salamander14.efkefc", kEffectPlaySpeed, kEffectScale);

	// 当たり判定登録・エフェクト生成などの共通処理は基底に任せる
	EnemyProjectile::Init();
}

#include "EnemyBossStateSpawn.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"

namespace
{
	constexpr float kSpawnEffectScale = 3.0f;
}

EnemyBossStateSpawn::EnemyBossStateSpawn(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent)
{
}

void EnemyBossStateSpawn::OnEnter()
{
	// 移動量をゼロにする
	Stop();

	// 登場アニメーションを再生する
	// ボス専用アニメーション名を使用する
	// （ここでは SpecialAttack2 を流用しているが、本来はボス専用名を定義すること）
	GetAnimator()->ChangeAnim(kAnimNameSpecialAttack2, false, 1.0f);

	// プレイヤーの方向を向く
	RotateToPlayer();

	// 出現エフェクトを生成する
	_spawnEffect = EffectManager::GetInstance().GenerateEffect(
		"BossSpawn.efkefc", GetParentPtr()->GetPos());
	if (_spawnEffect.lock())
	{
		_spawnEffect.lock()->SetScale(
			Vector3(kSpawnEffectScale, kSpawnEffectScale, kSpawnEffectScale));
	}
}

void EnemyBossStateSpawn::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 出現エフェクトの位置を追従させる
	if (_spawnEffect.lock() && _spawnEffect.lock()->IsPlaying())
	{
		_spawnEffect.lock()->SetPos(GetParentPtr()->GetPos());
	}
}

void EnemyBossStateSpawn::OnExit()
{
	// 出現エフェクトを停止する
	if (_spawnEffect.lock())
	{
		_spawnEffect.lock()->Stop();
	}
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateSpawn::CheckStateTransition()
{
	// アニメーションが終了していなければ遷移しない
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 終了していれば待機ステートへ
	return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
}

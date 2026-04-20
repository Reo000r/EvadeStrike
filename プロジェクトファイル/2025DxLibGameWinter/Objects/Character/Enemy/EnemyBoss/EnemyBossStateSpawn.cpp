#include "EnemyBossStateSpawn.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/Geometry/Easing.h"
#include <DxLib.h>

namespace {
	constexpr float kSpawnEffectScale = 3.0f;

	// 拡縮アニメーション進行度の範囲
	constexpr float kStartScaleRate = 0.0f;
	constexpr float kEndScaleRate = 0.6f;
}

EnemyBossStateSpawn::EnemyBossStateSpawn(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent),
	_targetScale(1.0f)
{
}

void EnemyBossStateSpawn::OnEnter()
{
	// 移動量をゼロにする
	Stop();

	// 登場アニメーションを再生する
	GetAnimator()->ChangeAnim(BossAnimData::kAnimNameSpawn, false, 1.0f);

	// プレイヤーの方向を向く
	RotateToPlayer(DX_PI_F);

	// 出現エフェクトを生成する
	_spawnEffect = EffectManager::GetInstance().GenerateEffect(
		"BossSpawn.efkefc", GetParentPtr()->GetPos());
	if (!_spawnEffect.expired()) {
		_spawnEffect.lock()->SetScale(
			Vector3(kSpawnEffectScale, kSpawnEffectScale, kSpawnEffectScale));
	}

	// 開始時の拡縮を保存
	Vector3 scale = MV1GetScale(GetAnimator()->GetHandle());
	_targetScale = scale.x;
	MV1SetScale(GetAnimator()->GetHandle(), Vector3(0, 0, 0));
}

void EnemyBossStateSpawn::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// プレイヤーの方向を向く
	RotateToPlayer(DX_PI_F);

	const std::shared_ptr<AnimationModel::AnimData> animData =
		GetAnimator()->GetCurrentAnimData();
	float rate = animData->frame / animData->totalFrame;

	// 拡縮アニメーション
	if (rate > kStartScaleRate &&
		rate < kEndScaleRate) {
		float scaleRate = rate / (kEndScaleRate - kStartScaleRate);
		scaleRate = Easing::Get(scaleRate, EasingType::EaseOutBack);
		MV1SetScale(GetAnimator()->GetHandle(),
			Vector3(1, 1, 1) * _targetScale * scaleRate);
		if (!_spawnEffect.expired()) {
			_spawnEffect.lock()->SetScale(
				Vector3(1,1,1) * kSpawnEffectScale * scaleRate);
		}
	}

	// 出現エフェクトの位置を追従させる
	if (!_spawnEffect.expired()) {
		if (_spawnEffect.lock()->IsPlaying()) {
			_spawnEffect.lock()->SetPos(GetParentPtr()->GetPos());
		}
	}
}

void EnemyBossStateSpawn::OnExit()
{
	// 出現エフェクトを停止する
	if (!_spawnEffect.expired()) {
		_spawnEffect.lock()->DeleteEffect();
	}
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateSpawn::CheckStateTransition()
{
	// アニメーションが終了していなければ遷移しない
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 終了していれば待機ステートへ
	return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
}

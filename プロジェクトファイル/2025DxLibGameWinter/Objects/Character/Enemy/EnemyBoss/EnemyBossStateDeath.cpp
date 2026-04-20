#include "EnemyBossStateDeath.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/Statistics.h"
#include "Scene/ResultDataHolder.h"
#include <DxLib.h>

namespace {
	// フェードアウト開始タイミング（アニメーション進行度）
	constexpr float kOpacityStartRate = 0.6f;

	// 死亡エフェクトのスケール
	constexpr float kDeathEffectScale = 5.0f;

	// エフェクト再生速度
	constexpr float kDefaultEffectSpeed = 0.7f;
	constexpr float kOpacityEffectSpeed = 0.7f;

	// 獲得スコア
	constexpr int kBossDeathScore = 1000;
}

EnemyBossStateDeath::EnemyBossStateDeath(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent)
{
}

void EnemyBossStateDeath::OnEnter()
{
	// スコア加算
	ResultDataHolder::GetInstance().AddScore(kBossDeathScore);

	// アニメーション変更
	GetAnimator()->ChangeAnim(kAnimNameDeath, false, 1.0f);

	// ロックオン解除
	GetParentPtr()->ReleaseCameraTarget();

	// 攻撃判定を無効化する
	GetParentPtr()->DisableAttackCol();

	// 死亡エフェクトを生成する
	_defeatEffect = EffectManager::GetInstance().GenerateEffect(
		"BossDeath.efkefc", GetParentPtr()->GetPos());
	if (!_defeatEffect.expired()) {
		_defeatEffect.lock()->SetScale(
			Vector3(kDeathEffectScale, kDeathEffectScale, kDeathEffectScale));
		_defeatEffect.lock()->SetPlaySpeed(kDefaultEffectSpeed);
	}

	// リザルト用の画面を取得する予定を立てる
	ResultDataHolder::GetInstance().ReserveCopyResultScreen();
}

void EnemyBossStateDeath::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	std::shared_ptr<AnimationModel::AnimData> data =
		GetAnimator()->GetCurrentAnimData();
	float rate = data->frame / data->totalFrame;
	// 開始条件を満たしていれば
	if (rate >= kOpacityStartRate) {
		// 0除算しなさそうなら
		if (kOpacityStartRate < 1.0f) {
			rate /= (1.0f - kOpacityStartRate);
		}
	}
	// 0.0f-1.0fから1.0f-0.0fに変換する
	rate = (rate * -1.0f) + 1.0f;
	// 1.0不透明 0.0透明
	MV1SetOpacityRate(GetAnimator()->GetHandle(), rate);

	float timeScale = GetParentPtr()->GetCurrentTimeScale();
	// エフェクト再生速度変更
	float effectPlaySpeed = kDefaultEffectSpeed * timeScale;
	if (rate >= kOpacityStartRate) {
		effectPlaySpeed = kOpacityEffectSpeed* timeScale;
	}
	if (!_defeatEffect.expired()) {
		_defeatEffect.lock()->SetPlaySpeed(effectPlaySpeed);
	}

	// アニメーションが終了しているなら
	if (data->isEnd) {
		// 削除可能
		GetParentPtr()->SetDeleteState(true);
		ReleasePhysics();
	}

	// エフェクト位置更新
	if (_defeatEffect.lock() &&
		_defeatEffect.lock()->IsPlaying()) {
		_defeatEffect.lock()->SetPos(GetParentPtr()->GetPos());
	}
}

void EnemyBossStateDeath::OnExit()
{
	// 処理なし
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateDeath::CheckStateTransition()
{
	// 死亡ステートから他ステートへは遷移しない
	return nullptr;
}

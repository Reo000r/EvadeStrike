#include "EnemyBossStateDeath.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/Statistics.h"
#include "Scene/ResultDataHolder.h"
#include <DxLib.h>

namespace
{
	// フェードアウト開始タイミング（アニメーション進行度）
	constexpr float kOpacityStartRate = 0.6f;

	// 死亡エフェクトのスケール
	constexpr float kDeathEffectScale = 5.0f;

	// エフェクト再生速度
	constexpr float kDefaultEffectSpeed = 0.7f;
	constexpr float kOpacityEffectSpeed  = 0.7f;

	// 獲得スコア
	constexpr int kBossDeathScore = 1000;
}

EnemyBossStateDeath::EnemyBossStateDeath(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent)
{
}

void EnemyBossStateDeath::OnEnter()
{
	// スコアを大量加算する
	ResultDataHolder::GetInstance().AddScore(kBossDeathScore);

	// 死亡アニメーションを再生する
	GetAnimator()->ChangeAnim(kAnimNameDeath, false, 1.0f);

	// カメラのロックオンを解除する
	GetParentPtr()->ReleaseCameraTarget();

	// 全攻撃判定を無効化する
	GetParentPtr()->DisableAttackCol();

	// 大規模な死亡エフェクトを生成する
	_defeatEffect = EffectManager::GetInstance().GenerateEffect(
		"BossDeath.efkefc", GetParentPtr()->GetPos());
	if (_defeatEffect.lock())
	{
		_defeatEffect.lock()->SetScale(
			Vector3(kDeathEffectScale, kDeathEffectScale, kDeathEffectScale));
		_defeatEffect.lock()->SetPlaySpeed(kDefaultEffectSpeed);
	}

	// リザルト画面のスクリーンコピーを予約する
	ResultDataHolder::GetInstance().ReserveCopyResultScreen();

	// ボス討伐イベントを通知する（扉開放・BGM変化などのトリガー）
	if (!GetEnemyManager().expired())
	{
		// EnemyManager 経由でイベントを発火する
		// （ActiveEventId が設定されていれば全滅イベントが呼ばれる）
		// ボス専用のイベントが必要な場合は別途 EventManager を参照すること
	}

	printf("Boss Defeated!\n");
}

void EnemyBossStateDeath::Update()
{
	// ステートが変わっていた場合return（死亡ステートからは遷移しないが念のため）
	if (UpdateStateTransition()) return;

	std::shared_ptr<AnimationModel::AnimData> data =
		GetAnimator()->GetCurrentAnimData();
	float rate = data->frame / data->totalFrame;

	// フェードアウト開始タイミングを超えたらモデルの透明度を下げる
	if (rate >= kOpacityStartRate)
	{
		// kOpacityStartRate〜1.0 の範囲を 1.0〜0.0 に変換する
		float fadeRate = (rate - kOpacityStartRate) / (1.0f - kOpacityStartRate);
		float opacity = 1.0f - fadeRate;	// 1.0不透明 → 0.0透明

		MV1SetOpacityRate(GetAnimator()->GetHandle(), opacity);

		// フェード中はエフェクト再生速度を変更する
		if (_defeatEffect.lock())
		{
			float timeScale = GetParentPtr()->GetCurrentTimeScale();
			_defeatEffect.lock()->SetPlaySpeed(kOpacityEffectSpeed * timeScale);
		}
	}
	else
	{
		float timeScale = GetParentPtr()->GetCurrentTimeScale();
		if (_defeatEffect.lock())
		{
			_defeatEffect.lock()->SetPlaySpeed(kDefaultEffectSpeed * timeScale);
		}
	}

	// エフェクト位置をボスに追従させる
	if (_defeatEffect.lock() && _defeatEffect.lock()->IsPlaying())
	{
		_defeatEffect.lock()->SetPos(GetParentPtr()->GetPos());
	}

	// アニメーション終了後に削除フラグをセットし、物理登録を解除する
	if (data->isEnd) {
		GetParentPtr()->SetDeleteState(true);
		ReleasePhysics();
	}
}

void EnemyBossStateDeath::OnExit()
{
	// 死亡ステートから他ステートへは遷移しないため処理なし
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateDeath::CheckStateTransition()
{
	// 死亡ステートから他ステートへは遷移しない
	return nullptr;
}

#include "EnemyWeakStateDeath.h"
#include "EnemyWeakStateBase.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/Statistics.h"
#include "Scene/ResultDataHolder.h"
#include <DxLib.h>

namespace {
	// 透明度変更を開始するフレーム
	constexpr float kOpacityStartRate = 0.5f;

	constexpr float kEffectScale = 2.0f;
	constexpr float kDefaultEffectSpeed = 0.9f;
	constexpr float kOpacityEffectSpeed = 0.9f;
}

EnemyWeakStateDeath::EnemyWeakStateDeath(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent)
{
}

void EnemyWeakStateDeath::OnEnter()
{
	ResultDataHolder::GetInstance().AddScore(100);

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameDeath, false, 1.0f);

	// ロックオン解除
	GetParentPtr()->ReleaseCameraTarget();

	_defeatEffect = EffectManager::GetInstance().GenerateEffect(
		"AtkOmen_Salamander14.efkefc", GetParentPtr()->GetPos());
	_defeatEffect.lock()->SetScale(Vector3(kEffectScale,kEffectScale,kEffectScale));
	_defeatEffect.lock()->SetPlaySpeed(kDefaultEffectSpeed);

	// リザルト用の画面を取得する予定を立てる
	ResultDataHolder::GetInstance().ReserveCopyResultScreen();
}

void EnemyWeakStateDeath::Update()
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
	float timeScale = GetParentPtr()->GetCurrentTimeScale();
	// エフェクト再生速度変更
	if (rate >= kOpacityStartRate) {
		if (_defeatEffect.lock()) {
			_defeatEffect.lock()->SetPlaySpeed(kOpacityEffectSpeed * timeScale);
		}
	}
	else {
		_defeatEffect.lock()->SetPlaySpeed(kDefaultEffectSpeed * timeScale);
	}
	// 0.0f-1.0fから1.0f-0.0fに変換する
	rate = (rate * -1.0f) + 1.0f;

	// 1.0不透明 0.0透明
	MV1SetOpacityRate(GetAnimator()->GetHandle(), rate);

	// アニメーションが終了しているなら
	if (data->isEnd) {
		// 削除可能
		GetParentPtr()->SetDeleteState(true);
		ReleasePhysics();
	}

	// エフェクト位置更新
	if (_defeatEffect.lock()) {
		if (_defeatEffect.lock()->IsPlaying()) {
			_defeatEffect.lock()->SetPos(GetParentPtr()->GetPos());
		}
	}
}

void EnemyWeakStateDeath::OnExit()
{
	// 処理なし
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateDeath::CheckStateTransition()
{
	// ステートは変わらない
	return nullptr;
}

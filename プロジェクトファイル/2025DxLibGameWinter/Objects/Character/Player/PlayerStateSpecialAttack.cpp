#include "PlayerStateSpecialAttack.h"
#include "PlayerStateDodge.h"
#include "PlayerStateDodgeFail.h"
#include "PlayerStateIdle.h"
#include "PlayerStateJump.h"
#include "Objects/Character/Enemy/EnemyBase.h"
#include "Objects/Camera/Camera.h"
#include "Library/Physics/ColliderDataCapsule.h"
#include "Library/Objects/AttackCol.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Input.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/SoundManager.h"
#include "Library/DebugTools/DebugDiffTracker.h"
#include <cassert>

PlayerStateSpecialAttack::PlayerStateSpecialAttack(std::weak_ptr<Player> player, 
	PlayerAttackAnimData data, std::shared_ptr<AttackCol> attackCol) :
	PlayerStateAttack(player, data, attackCol, PlayerActionKind::Special)
{
}

void PlayerStateSpecialAttack::OnEnter()
{
	// 攻撃判定行列をモデルの前にする
	_attackColParentMatrix = GetPlayerPtr()->GetModelMatrix();

	// 敵が指定範囲内にいるか
	// 範囲内にいれば有効な敵が返ってくる
	std::weak_ptr<EnemyBase> enemy =
		IsExistEnemyWithinRange(kSpecialAttackLockOnDist);
	// 敵が範囲内にいた場合は
	if (!enemy.expired()) {
		// 敵の方向を向かせる
		Position3 enemyPos = enemy.lock()->GetCenterPos();
		RotateToPos(enemyPos);
		// すでに注視点に登録されていたオブジェクトを解除
		GetCamera().lock()->ResetOtherTarget();
		// 注視点に敵を追加する
		GetCamera().lock()->AddOtherTarget(enemy);

		// 基準行列作成
		constexpr float kBackDist = 50.0f;
		_attackColParentMatrix = GetLookAtMatrixY(
			GetPlayerPtr()->GetPos(), enemyPos, kBackDist);
	}
	// 敵が範囲内にいないかつ
	// 方向入力があった場合は
	else if (CanDashInput()) {
		// 入力方向に向ける

		// カメラの向きを考慮しつつ目標の角度を計算
		const float cameraRot = GetCameraRotAngle().y;
		Vector2 stick = Input::GetInstance().GetPadLeftStick();
		float targetAngle = atan2f(stick.y, stick.x) + -cameraRot + DX_PI_F * -0.5f;
		// 正規化
		targetAngle = Calc::RadianNormalize(targetAngle);

		// 入力方向に向かせる
		SetRotAngleY(targetAngle);
	}

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(_data.animName, false, 1.0f);

	// 攻撃判定に沿ってエフェクト生成
	_currentEffect =
		EffectManager::GetInstance().GenerateEffect(
			"Atk_Lance5.efkefc", _attackCol->GetPos());
	_currentEffect.lock()->SetPlaySpeed(1.1f);
}

void PlayerStateSpecialAttack::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	const std::shared_ptr<AnimationModel::AnimData> animData = GetAnimator()->GetCurrentAnimData();
	// 再生されていてほしいアニメーション名と
	// 現在のアニメーション名が異なっていた場合
	if (animData->animName != _data.animName) {
		printf("プレイヤーの攻撃アニメーション名の不一致：\n");
		printf("想定：%ls\n", _data.animName.c_str());
		printf("実際：%ls\n", animData->animName.c_str());
		assert(false && "想定のアニメーション名と不一致");
	}
	// アニメーション進行度
	const float currentAnimFrameRate = animData->frame / animData->totalFrame;

	// 攻撃時の移動が行えるなら
	if (_data.CanAttackTransform(currentAnimFrameRate)) {
		// 向いている方向へ前進する
		AddTransformForward(kEnterForwardVel);
	}

	// 攻撃判定を出していいかつ
	// 攻撃判定が出ていないなら
	if (_data.CanAttackColActive(currentAnimFrameRate) &&
		!_attackCol->GetCollisionState()) {
		// 武器の攻撃判定を生成する
		_attackCol->Enable();
		// カメラを少し揺らす
		GetCamera().lock()->Shake(60, 10, 40);
		SoundManager::GetInstance().PlaySoundType(SEType::AttackSP);
	}
	// 攻撃判定を消すタイミングになっているかつ
	// 攻撃判定が出ているなら
	else if (currentAnimFrameRate > _data.endAttackColActiveFrameRate &&
		_attackCol->GetCollisionState()) {
		// 攻撃判定を消す
		_attackCol->Disable();
	}

	// 攻撃判定の更新
	_attackCol->PositionUpdate(_attackColParentMatrix);
	_attackCol->Update();

	// エフェクト位置更新
	if (_currentEffect.lock()) {
		if (_currentEffect.lock()->IsPlaying()) {
			_currentEffect.lock()->SetPos(_attackCol->GetPos());
		}
	}
}

void PlayerStateSpecialAttack::OnExit()
{
	// 攻撃判定を消す
	_attackCol->Disable();
}

std::shared_ptr<PlayerStateBase> PlayerStateSpecialAttack::CheckStateTransition()
{
	// 入力が効く状態でないかつ
	// アニメーションが終わっていなければreturn
	if (!CanInputAllowance(kCanInputProgressSpecialAttack) &&
		!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 回避入力があれば
	if (CanDodgeInput()) {
		// 回避成功していれば
		if (IsSuccessDodge()) {
			// 回避ステートに入る
			return std::make_shared<PlayerStateDodge>(GetPlayerPtr());
		}
		// 回避失敗であれば
		else {
			// 回避失敗ステートに入る
			return std::make_shared<PlayerStateDodgeFail>(GetPlayerPtr());
		}
	}
	// ジャンプ入力があるかつ
	// ジャンプ可能であるなら
	if (CanJumpInput()) {
		// ジャンプステートに入る
		return std::make_shared<PlayerStateJump>(GetPlayerPtr());
	}

	// アニメーションが終了しているかつ
	// 先行入力がなかったなら
	if (GetAnimator()->GetCurrentAnimData()->isEnd) {
		// コンボを途切れさせる
		ResetCombo();
		// 待機ステートに入る
		return std::make_shared<PlayerStateIdle>(GetPlayerPtr());
	}

	return nullptr;
}

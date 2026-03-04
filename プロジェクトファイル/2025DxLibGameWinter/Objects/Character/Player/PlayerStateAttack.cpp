#include "PlayerStateAttack.h"
#include "PlayerStateDodge.h"
#include "PlayerStateDodgeFail.h"
#include "PlayerStateIdle.h"
#include "PlayerStateJump.h"
#include "Library/Objects/Weapon/WeaponPlayer.h"
#include "PlayerComboHolder.h"
#include "Objects/Character/Enemy/EnemyBase.h"
#include "Objects/Camera/Camera.h"
#include "Library/Objects/AttackCol.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Input.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/SoundManager.h"
#include <cassert>

PlayerStateAttack::PlayerStateAttack(std::weak_ptr<Player> player,
	PlayerAttackAnimData data, std::shared_ptr<AttackCol> attackCol,
	PlayerActionKind actionKind) :
	PlayerStateBase(player),
	_data(data),
	_attackCol(attackCol)
{
	// 攻撃を行ったらコンボ登録する
	GetComboHolder()->AddAction(actionKind);
}

void PlayerStateAttack::OnEnter()
{
	// 敵が指定範囲内にいるか
	// 範囲内にいれば有効な敵が返ってくる
	std::weak_ptr<EnemyBase> enemy = 
		IsExistEnemyWithinRange(kNormalAttackLockOnDist);
	// 敵が範囲内にいた場合は
	if (!enemy.expired()) {
		// 敵の方向を向かせる
		Position3 enemyPos = enemy.lock()->GetCenterPos();
		RotateToPos(enemyPos);
		// すでに注視点に登録されていたオブジェクトを解除
		GetCamera().lock()->ResetOtherTarget();
		// 注視点に敵を追加する
		GetCamera().lock()->AddOtherTarget(enemy);
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

	if (_data.animName == L"Armature|LightAttack_1" ||
		_data.animName == L"Armature|LightAttack_2" ||
		_data.animName == L"Armature|LightAttack_3" ||
		_data.animName == L"Armature|LightAttack_4") {
		SoundManager::GetInstance().PlaySoundType(SEType::AttackPunch);
	}
	else {
		SoundManager::GetInstance().PlaySoundType(SEType::AttackKick);
	}
}

void PlayerStateAttack::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 攻撃時の移動と
	// 攻撃判定の切り替えを行う

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
		GetCamera().lock()->Shake(30, 5, 20);
		_currentEffect = EffectManager::GetInstance().GenerateEffect("Atk_Salamander6.efkefc", _attackCol->GetCenterPos());
	}
	// 攻撃判定を消すタイミングになっているかつ
	// 攻撃判定が出ているなら
	else if(currentAnimFrameRate > _data.endAttackColActiveFrameRate &&
		_attackCol->GetCollisionState()) {
		// 攻撃判定を消す
		_attackCol->Disable();
	}

	// 攻撃判定の更新
	_attackCol->PositionUpdate(
		GetPlayerPtr()->GetModelMatrix());
	_attackCol->Update();

	// エフェクト位置更新
	if (_currentEffect.lock()) {
		if (_currentEffect.lock()->IsPlaying()) {
			_currentEffect.lock()->SetPos(_attackCol->GetCenterPos());
		}
	}
}

void PlayerStateAttack::OnExit()
{
	// 攻撃判定を消す
	_attackCol->Disable();
}

std::shared_ptr<PlayerStateBase> PlayerStateAttack::CheckStateTransition()
{
	// 入力が効く状態でないかつ
	// アニメーションが終わっていなければreturn
	if (!CanInputAllowance(kCanInputProgress) &&
		!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 攻撃の最低限の動作が終わっているかつ
	// 入力があったなら
	if (CanInputAllowance(kCanInputProgress)) {
		PlayerActionKind actionKind = PlayerActionKind::None;
		if (CanPunchAttackInput()) {
			actionKind = PlayerActionKind::Punch;
		}
		else if (CanKickAttackInput()) {
			actionKind = PlayerActionKind::Kick;
		}

		// 攻撃種別がはっきりしているなら
		if (actionKind != PlayerActionKind::None) {
			// コンボ遷移判定を行い、返ってきたステートに遷移させる
			std::shared_ptr<PlayerStateBase> state = CheckComboTransition(actionKind, _data);
			// 遷移すべきなら
			if (state != nullptr) {
				// ステート遷移
				return state;
			}
		}
	}

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
		// コンボを途切れさせる
		ResetCombo();
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

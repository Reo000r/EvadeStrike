#include "PlayerStateDodge.h"
#include "PlayerStateFall.h"
#include "PlayerStateIdle.h"
#include "PlayerStateDash.h"
#include "PlayerStateAttack.h"
//#include "PlayerStateAirAttack.h"
#include "PlayerAnimationData.h"
#include "Objects/Character/Player/JustDodge/JustDodgeCollider.h"
#include "Objects/Character/Player/JustDodge/JustDodgeManager.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Input.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/SoundManager.h"

namespace {
	constexpr float kStartTransformFrameRate = 0.0f;
	constexpr float kEndTransformFrameRate = 0.7f;

	// ジャスト回避
	constexpr float kStartJustDodgeFrameRate = 0.0f;	// ジャスト回避開始
	constexpr float kEndJustDodgeFrameRate = 0.5f;		// ジャスト回避終了
	constexpr float kJustDodgeColRad = 300.0f;			// ジャスト回避判定半径
	const Vector3 kJustDodgeColPosOffset = 
		Vector3(0.0f, kJustDodgeColRad * 0.5f, 0.0f);	// 判定位置補正
}

PlayerStateDodge::PlayerStateDodge(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateDodge::OnEnter()
{
	// 方向入力があった場合は
	if (CanDashInput()) {
		// 入力方向の反対に向ける
		// (後ろに移動するため)
		
		// カメラの向きを考慮しつつ目標の角度を計算
		const float cameraRot = GetCameraRotAngle().y;
		Vector2 stick = Input::GetInstance().GetPadLeftStick();
		float targetAngle = atan2f(stick.y, stick.x) + -cameraRot + DX_PI_F * -0.5f;
		// 入力方向を反転させる
		targetAngle += Calc::ToRadian(180.0f);
		// 正規化
		targetAngle = Calc::RadianNormalize(targetAngle);

		// 入力方向の反対に向かせる
		SetRotAngleY(targetAngle);
	}

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameDodge, false, 1.0f);

	// ジャスト回避判定作成
	_justDodgeCollider = std::make_shared<JustDodgeCollider>(GetPhysics());
	_justDodgeCollider->SetData(kJustDodgeColRad, kJustDodgeColPosOffset);
	// プレイヤーの位置に判定を設置
	_justDodgeCollider->UpdatePosition(GetPlayerPtr()->GetCenterPos());

#ifdef _DEBUG
	// debug
	if (Input::GetInstance().IsPress("Debug:JustDodge")) {
		// ジャスト回避処理を行う
		StartJustDodge();
	}
#endif // _DEBUG

	// 回避待機時間追加
	SetDodgeInterval();
	// 回避回数追加
	AddDodgeCount();
}

void PlayerStateDodge::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// ジャスト回避が行えていたかつ
	// ジャスト回避が行えるなら
	if (_justDodgeCollider->IsHit() &&
		GetJustDodgeManager().lock()->CanJustDodge()) {
		// ジャスト回避処理を行う
		StartJustDodge();
	}

	// アニメーション進行度
	const std::shared_ptr<AnimationModel::AnimData> animData = GetAnimator()->GetCurrentAnimData();
	const float currentAnimFrameRate = animData->frame / animData->totalFrame;

	// 移動が行えるなら
	if (currentAnimFrameRate > kStartTransformFrameRate &&
		currentAnimFrameRate < kEndTransformFrameRate) {
		// 後方へ回避移動 
		AddTransformForward(kDodgeSpeed);
	}

#ifdef _DEBUG
	// debug
	if (GetJustDodgeManager().lock()->CanJustDodge() &&
		Input::GetInstance().IsTrigger("Debug:JustDodge")) {
		// ジャスト回避処理を行う
		StartJustDodge();
	}
#endif // _DEBUG

	// ジャスト回避を行えるなら
	if (GetJustDodgeManager().lock()->CanJustDodge()) {
		// ジャスト回避判定を出していいかつ
		// ジャスト回避判定が出ていないなら
		if (CanJustDodgeColActive(currentAnimFrameRate) &&
			!_justDodgeCollider->GetCollisionState()) {
			// ジャスト回避判定を生成する
			_justDodgeCollider->Enable();
		}
		// ジャスト回避判定を消すタイミングになっているかつ
		// ジャスト回避判定が出ているなら
		else if (currentAnimFrameRate > kEndJustDodgeFrameRate &&
			_justDodgeCollider->GetCollisionState()) {
			// ジャスト回避判定を消す
			_justDodgeCollider->Disable();
		}

		// ジャスト回避判定が出ているなら
		if (_justDodgeCollider->GetCollisionState()) {
			// プレイヤーの位置に判定を設置
			_justDodgeCollider->UpdatePosition(GetPlayerPtr()->GetCenterPos());
		}
	}

	// エフェクト位置更新
	if (_currentEffect.lock()) {
		if (_currentEffect.lock()->IsPlaying()) {
			_currentEffect.lock()->SetPos(GetPlayerPtr()->GetCenterPos());
		}
	}
}

void PlayerStateDodge::OnExit()
{
	// ジャスト回避判定を消す
	_justDodgeCollider->Disable();
}

std::shared_ptr<PlayerStateBase> PlayerStateDodge::CheckStateTransition()
{
	// 入力が効く状態でないかつ
	// アニメーションが終わっていなければreturn
	if (!CanInputAllowance(kCanDodgeInputProgress) &&
		!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 攻撃入力があったら
	if (CanAttackInput()) {
		PlayerActionKind actionKind = PlayerActionKind::None;
		if (CanPunchAttackInput()) {
			actionKind = PlayerActionKind::Punch;
		}
		else if (CanKickAttackInput()) {
			actionKind = PlayerActionKind::Kick;
		}
		// 攻撃種別がはっきりしているなら
		if (actionKind != PlayerActionKind::None) {
			// 接地していれば
			if (IsGround()) {
				// 遷移先のデータのみが入った
				PlayerAttackAnimData data = GetDataLoader()->GetBeforeAtackAnimData();
				// コンボ遷移判定を行い、返ってきたステートに遷移させる
				std::shared_ptr<PlayerStateBase> state = CheckComboTransition(actionKind, data);
				// 遷移すべきなら
				if (state != nullptr) {
					// ステート遷移
					return state;
				}
			}
			// 接地していなければ
			else {
				// 遷移先のデータのみが入った
				PlayerAttackAnimData data = GetDataLoader()->GetBeforeAtackAnimData();
				// コンボ遷移判定を行い、返ってきたステートに遷移させる
				std::shared_ptr<PlayerStateBase> state = CheckAirComboTransition(actionKind, data);
				// 遷移すべきなら
				if (state != nullptr) {
					// ステート遷移
					return state;
				}
			}
		}
	}
	
	// 移動入力があれば
	if (CanDashInput()) {
		// コンボを途切れさせる
		ResetCombo();
		// 移動ステートに入る
		return std::make_shared<PlayerStateDash>(GetPlayerPtr());
	}
	// アニメーションが終了しているかつ
	// 先行入力がなかったなら
	if (GetAnimator()->GetCurrentAnimData()->isEnd) {
		// 接地していなければ
		if (!IsGround()) {
			// コンボを途切れさせる
			ResetCombo();
			// 落下ステートに入る
			return std::make_shared<PlayerStateFall>(GetPlayerPtr());
		}
		else {
			// コンボを途切れさせる
			ResetCombo();
			// 待機ステートに入る
			return std::make_shared<PlayerStateIdle>(GetPlayerPtr());
		}
	}

	return nullptr;
}

bool PlayerStateDodge::CanJustDodgeColActive(float currentFrameRate)
{
	return (currentFrameRate > kStartJustDodgeFrameRate &&
		currentFrameRate < kEndJustDodgeFrameRate);
}

void PlayerStateDodge::StartJustDodge()
{
	// ジャスト回避通知
	GetJustDodgeManager().lock()->Start();
	// ジャスト回避判定を消す
	_justDodgeCollider->Disable();
	_currentEffect = EffectManager::GetInstance().GenerateEffect("Buff_Magic2_holy_RedForBlue.efkefc", GetPlayerPtr()->GetCenterPos());
	_currentEffect.lock()->SetPlaySpeed(0.5f);
	StartDodgeEffect();

	SoundManager::GetInstance().PlaySoundType(SEType::JustDodge);
}

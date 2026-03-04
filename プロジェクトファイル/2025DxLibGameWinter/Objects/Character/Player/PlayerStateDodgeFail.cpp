#include "PlayerStateDodgeFail.h"
#include "PlayerStateDash.h"
#include "PlayerStateFall.h"
#include "PlayerStateIdle.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Input.h"

namespace {
	constexpr float kStartTransformFrameRate = 0.0f;
	constexpr float kEndTransformFrameRate = 0.2f;
}

PlayerStateDodgeFail::PlayerStateDodgeFail(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateDodgeFail::OnEnter()
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
	GetAnimator()->ChangeAnim(kAnimNameDodgeFail, false, 1.0f);

	// コンボを途切れさせる
	ResetCombo();
	// 回避待機時間を0にする
	ResetDodgeInterval();
}

void PlayerStateDodgeFail::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// アニメーション進行度
	const std::shared_ptr<AnimationModel::AnimData> animData = GetAnimator()->GetCurrentAnimData();
	const float currentAnimFrameRate = animData->frame / animData->totalFrame;

	// 移動が行えるなら
	if (currentAnimFrameRate > kStartTransformFrameRate &&
		currentAnimFrameRate < kEndTransformFrameRate) {
		// 後方へ回避移動 
		AddTransformForward(kDodgeFailSpeed);
	}
}

void PlayerStateDodgeFail::OnExit()
{
	// 処理なし
}

std::shared_ptr<PlayerStateBase> PlayerStateDodgeFail::CheckStateTransition()
{
	// 入力が効く状態でないかつ
	// アニメーションが終わっていなければreturn
	if (!CanInputAllowance(kCanDodgeFailInputProgress) &&
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

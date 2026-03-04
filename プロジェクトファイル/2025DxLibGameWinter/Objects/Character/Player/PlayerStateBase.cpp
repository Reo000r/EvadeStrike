#include "PlayerStateBase.h"
#include "Library/System/Input.h"
#include "PlayerStateAttack.h"
#include "PlayerStateAirAttack.h"
#include "Library/Objects/AttackCol.h"
#include "PlayerStateSpecialAttack.h"
#include "PlayerStateAirSpecialAttack.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Objects/Character/Enemy/EnemyBase.h"
#include "Library/System/UI/UIGameScoreDrawer.h"
#include <cassert>

PlayerStateBase::PlayerStateBase(std::weak_ptr<Player> player) :
	_player(player)
{
}

bool PlayerStateBase::UpdateStateTransition()
{
	// ステート遷移が可能か確認
	std::shared_ptr<PlayerStateBase> nextState = CheckStateTransition();
	// ステート遷移が可能な場合
	if (nextState != nullptr) {
		// ステート変更後は処理を書かない
		ChangeState(nextState);
		return true;
	}
	return false;
}

void PlayerStateBase::ChangeState(std::shared_ptr<PlayerStateBase> nextState)
{
	// このステートの処理を行う
	OnExit();
	// 次のステートの開始直後の処理を行う
	nextState->OnEnter();
	_player.lock()->_currentState = nextState;

	// 上記の処理を行う事でこのステートは存在しないものとなったので
	// これ以降は処理を書かない
	return;
}

bool PlayerStateBase::CanInputAllowance(float canInputProgressRate) const
{
	const std::shared_ptr<AnimationModel::AnimData> animData = GetAnimator()->GetCurrentAnimData();
	return (animData->frame > animData->totalFrame * canInputProgressRate);
}

std::weak_ptr<Physics> PlayerStateBase::GetPhysics() const
{
	return _player.lock()->GetPhysics();
}

const std::shared_ptr<AnimationModel>& PlayerStateBase::GetAnimator() const
{
	return _player.lock()->_animator;
}

std::shared_ptr<PlayerComboHolder> PlayerStateBase::GetComboHolder() const
{
	return _player.lock()->_comboHolder;
}

std::shared_ptr<UIGameScoreDrawer> PlayerStateBase::GetScoreDrawer() const
{
	return _player.lock()->_scoreDrawer.lock();
}

std::shared_ptr<PlayerDataLoader> PlayerStateBase::GetDataLoader() const
{
	return _player.lock()->_dataLoader;
}

std::weak_ptr<JustDodgeManager> PlayerStateBase::GetJustDodgeManager() const
{
	return _player.lock()->GetJustDodgeManager();
}

std::weak_ptr<EnemyManager> PlayerStateBase::GetEnemyManager() const
{
	return _player.lock()->GetEnemyManager();
}

std::shared_ptr<Collider> PlayerStateBase::GetCollider() const
{
	return _player.lock()->_collider;
}

Matrix4x4 PlayerStateBase::GetForwardMatrix() const
{
	return _player.lock()->GetForwardMatrix();
}

std::weak_ptr<Camera> PlayerStateBase::GetCamera() const
{
	return _player.lock()->GetCamera();
}

Vector3 PlayerStateBase::GetCameraRotAngle() const
{
	return _player.lock()->GetCameraRotAngle();
}

void PlayerStateBase::ResetDodgeInterval()
{
	_player.lock()->_dodgeInterval = 0.0f;
	_player.lock()->_dodgeCount = 0;
}

void PlayerStateBase::StartDodgeEffect()
{
	_player.lock()->StartDodgeEffect();
}

std::shared_ptr<PlayerStateBase> PlayerStateBase::CheckComboTransition(PlayerActionKind kind, PlayerAttackAnimData data) const
{
	if (kind == PlayerActionKind::Punch) {
		// 特殊コンボが成立していたら
		if (GetComboHolder()->CheckComboConsist(kind)) {
			printf("コンボ成立");
			// 特殊攻撃クラスを返す
			PlayerAttackAnimData retdata = GetDataLoader()->GetAttackAnimData(GetDataLoader()->GetSpecialAttackID());
			std::shared_ptr<AttackCol> col = GetAttackCol(PlayerActionKind::Special);
			return std::make_shared<PlayerStateSpecialAttack>(GetPlayerPtr(), retdata, col);
		}
		if (data.punchId != -1) {
			PlayerAttackAnimData retdata = GetDataLoader()->GetAttackAnimData(data.punchId);
			std::shared_ptr<AttackCol> col = GetAttackCol(kind);
			return std::make_shared<PlayerStateAttack>(GetPlayerPtr(), retdata,
				col, kind);
		}
	}
	else if (kind == PlayerActionKind::Kick) {
		// 特殊コンボが成立していたら
		if (GetComboHolder()->CheckComboConsist(kind)) {
			printf("コンボ成立");
			// 特殊攻撃クラスを返す
			PlayerAttackAnimData retdata = GetDataLoader()->GetAttackAnimData(GetDataLoader()->GetSpecialAttackID());
			std::shared_ptr<AttackCol> col = GetAttackCol(PlayerActionKind::Special);
			return std::make_shared<PlayerStateSpecialAttack>(GetPlayerPtr(), retdata, col);
		}
		if (data.kickId != -1) {
			PlayerAttackAnimData retdata = GetDataLoader()->GetAttackAnimData(data.kickId);
			std::shared_ptr<AttackCol> col = GetAttackCol(kind);
			return std::make_shared<PlayerStateAttack>(GetPlayerPtr(), retdata,
				col, kind);
		}
	}
	else {
		assert(false && "不明なコンボ遷移時のアクション");
		return nullptr;
	}
	// 新規の攻撃ステートには遷移しない
	return nullptr;
}

std::shared_ptr<PlayerStateBase> PlayerStateBase::CheckAirComboTransition(PlayerActionKind kind, PlayerAttackAnimData data) const
{
	if (kind == PlayerActionKind::Punch) {
		// 特殊コンボが成立していたら
		if (GetComboHolder()->CheckComboConsist(kind)) {
			printf("コンボ成立");
			// 特殊攻撃クラスを返す
			PlayerAttackAnimData retdata = GetDataLoader()->GetAttackAnimData(GetDataLoader()->GetSpecialAttackID());
			std::shared_ptr<AttackCol> col = GetAttackCol(PlayerActionKind::Special);
			return std::make_shared<PlayerStateAirSpecialAttack>(GetPlayerPtr(), retdata, col);
		}
		if (data.punchId != -1) {
			PlayerAttackAnimData retdata = GetDataLoader()->GetAttackAnimData(data.punchId);
			std::shared_ptr<AttackCol> col = GetAttackCol(kind);
			return std::make_shared<PlayerStateAirAttack>(GetPlayerPtr(), retdata,
				col, kind);
		}
	}
	else if (kind == PlayerActionKind::Kick) {
		// 特殊コンボが成立していたら
		if (GetComboHolder()->CheckComboConsist(kind)) {
			printf("コンボ成立");
			// 特殊攻撃クラスを返す
			PlayerAttackAnimData retdata = GetDataLoader()->GetAttackAnimData(GetDataLoader()->GetSpecialAttackID());
			std::shared_ptr<AttackCol> col = GetAttackCol(PlayerActionKind::Special);
			return std::make_shared<PlayerStateAirSpecialAttack>(GetPlayerPtr(), retdata, col);
		}
		if (data.kickId != -1) {
			PlayerAttackAnimData retdata = GetDataLoader()->GetAttackAnimData(data.kickId);
			std::shared_ptr<AttackCol> col = GetAttackCol(kind);
			return std::make_shared<PlayerStateAirAttack>(GetPlayerPtr(), retdata,
				col, kind);
		}
	}
	else {
		assert(false && "不明なコンボ遷移時のアクション");
		return nullptr;
	}
	// 新規の攻撃ステートには遷移しない
	return nullptr;
}

bool PlayerStateBase::IsSuccessDodge() const
{
	int count = GetDodgeCount();
	float interval = GetDodgeInterval();
	printf("回避:%d回 時間:%.02f\n", count, interval);
	// 失敗条件を満たしていれば
	if (count >= kMaxDodgeCount - 1 &&
		interval > 0.0f) {
		return false;
	}
	// 成功していれば
	return true;
}

int PlayerStateBase::GetPunchAttackID() const
{
	return GetDataLoader()->GetBeforeAtackAnimData().punchId;
}

int PlayerStateBase::GetKickAttackID() const
{
	return GetDataLoader()->GetBeforeAtackAnimData().kickId;
}

void PlayerStateBase::Rotate() const
{
	// 回転
	_player.lock()->Rotate();
}

void PlayerStateBase::RotateToPos(Position3 pos)
{
	// 指定位置の方向を向く
	Position3 playerPos = GetPlayerPtr()->GetPos();
	// 指定位置への方向ベクトル
	Vector3 toPos = pos - playerPos;
	if (toPos.SqrMagnitude() > 0.0f) {
		// Y軸回転角度を計算
		SetRotAngleY(atan2f(toPos.x, toPos.z));
	}
}

std::shared_ptr<AttackCol> PlayerStateBase::GetAttackCol(PlayerActionKind kind)const
{
	return _player.lock()->GetAttackCol(kind);
}

void PlayerStateBase::AddTransformForward(float add) const
{
	// 向いている方向に移動
	_player.lock()->AddTransformForward(add);
}

void PlayerStateBase::AddTransformStick(float add) const
{
	_player.lock()->AddTransformStick(add);
}

void PlayerStateBase::AddTransformUp(float add) const
{
	// 上方向に移動
	_player.lock()->AddTransformUp(add);
}

void PlayerStateBase::Stop() const
{
	// 移動停止
	_player.lock()->Stop();
}

bool PlayerStateBase::IsGround()
{
	return _player.lock()->IsGround();
}

bool PlayerStateBase::CanAttackInput()
{
	return (CanPunchAttackInput() || CanKickAttackInput());
}

bool PlayerStateBase::CanPunchAttackInput()
{
	Input& input = Input::GetInstance();
	return (input.IsTrigger("Gameplay:Punch"));
}

bool PlayerStateBase::CanKickAttackInput()
{
	Input& input = Input::GetInstance();
	return (input.IsTrigger("Gameplay:Kick"));
}

bool PlayerStateBase::CanWalkInput()
{
	Input& input = Input::GetInstance();

	// スティック入力があるか
	bool stickInputState = (input.GetPadLeftStick().Magnitude() >= 0.005f);

	// スティック入力か移動キー入力があればtrue
	return (stickInputState ||
		input.IsPress("Gameplay:Up") ||
		input.IsPress("Gameplay:Down") ||
		input.IsPress("Gameplay:Left") ||
		input.IsPress("Gameplay:Right"));
}

bool PlayerStateBase::CanDashInput()
{
	Input& input = Input::GetInstance();

	// 一定以上のスティック入力があるか
	bool stickInputState = (input.GetPadLeftStick().Magnitude() >= 1000 * 0.8f);

	// スティック入力か移動キー入力があればtrue
	return (stickInputState ||
		input.IsPress("Gameplay:Up") ||
		input.IsPress("Gameplay:Down") ||
		input.IsPress("Gameplay:Left") ||
		input.IsPress("Gameplay:Right"));
}

bool PlayerStateBase::CanJumpInput()
{
	return Input::GetInstance().IsPress("Gameplay:Jump");
}

bool PlayerStateBase::CanDodgeInput()
{
	return Input::GetInstance().IsPress("Gameplay:Dodge");
}

void PlayerStateBase::ResetCombo()
{
	// コンボ履歴をリセット
	GetComboHolder()->ResetActionData();
	// スコア描画のコンボ表示をリセット
	GetScoreDrawer()->ResetActionList();
}

std::weak_ptr<EnemyBase> PlayerStateBase::IsExistEnemyWithinRange(float range)
{
	Position3 pos = GetPlayerPtr()->GetCenterPos();
	std::weak_ptr<EnemyBase> enemy = GetEnemyManager().lock()->GetNearestEnemy(pos);
	// 不正なポインタであれば
	if (enemy.expired()) {
		return std::weak_ptr<EnemyBase>();
	}
	Vector3 toEnemy = pos - enemy.lock()->GetCenterPos();
	// 敵が近くにいた場合は
	if (toEnemy.Magnitude() <= range) {
		return enemy;
	}
	// 範囲内にいない場合は
	return std::weak_ptr<EnemyBase>();
}

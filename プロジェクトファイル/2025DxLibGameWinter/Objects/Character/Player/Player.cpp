#include "Player.h"
#include "Library/Physics/ColliderData.h"
#include "Library/Physics/Rigidbody.h"
#include "Library/Objects/AttackableGameObject.h"
#include "Library/Objects/AnimationModel.h"
#include "Library/Objects/Weapon/WeaponPlayer.h"
#include "Library/Objects/AttackCol.h"
#include "Library/System/Input.h"
#include "Library/System/InputStateHolder.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Loader/PlayerDataLoader.h"
#include "PlayerComboHolder.h"
#include "Objects/Camera/Camera.h"
#include "PlayerAnimationData.h"
#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include "PlayerStateReact.h"
#include "PlayerStateDeath.h"
#include "Library/DebugTools/DebugDiffTracker.h"
#include "Library/System/UI/UIPlayerHP.h"
#include "Library/System/UI/UIManager.h"
#include "Scene/ResultDataHolder.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>

namespace {
	constexpr float kHitPoint = 100.0f;

	// 当たり判定のパラメータ
	constexpr float kColRadius = 70.0f;		// 半径
	constexpr float kColHeight = 350.0f;	// 身長

	// モデル描画時の回転補正
	// モデルがz-を向いている為
	constexpr float kModelDrawOffsetAngle = DX_PI_F;

	// カプセルの始点(足元)から終点(頭頂部)までのベクトル
	const Vector3 kColStartToEnd = Vector3Up() * (kColHeight - kColRadius * 2.0f);

	constexpr float kTurnSpeed = 0.35f;	// 回転速度(ラジアン)
	// 武器を追従させるフレーム名
	const std::wstring kWeaponFrameName = L"mixamorig:RightHandThumb3";

	// ノックバック減衰量
	constexpr float kKnockbackDecayAmount = 0.0f;
}

Player::Player(std::weak_ptr<Physics> physics) :
	CharacterBase(
		physics,
		PhysicsData::Priority::Middle,
		PhysicsData::GameObjectTag::Player,
		PhysicsData::ColliderKind::Capsule,
		false,
		true,
		true),
	_currentState(),
	_animator(std::make_shared<AnimationModel>()),
	_camera(),
	_dataLoader(std::make_shared<PlayerDataLoader>()),
	_comboHolder(std::make_shared<PlayerComboHolder>()),
	_attackCol(),
	_rotAngleY(0.0f),
	_modelRotAngleY(0.0f),
	_quaternion(),
	_lockonDurationTime(0.0f),
	_dodgeInterval(0.0f),
	_dodgeCount(0),
	_isAlive(true)
{
	// 体力設定
	SetHitPoint(kHitPoint);

	// 当たり判定データ設定
	_collider->CreateColliderDataCapsule(
		kColRadius,		// 半径
		kColStartToEnd,	// 始点から終点のベクトル
		false,			// isTrigger
		true			// isCollision
	);
	// 自身の武器とは当たり判定を行わない
	_collider->AddThroughTag(PhysicsData::GameObjectTag::PlayerAttack);
	
	// physicsに登録する
	EntryPhysics(_physics);

	// モデルの読み込み
	_animator->Init(MV1LoadModel(L"Data/Model/Character/PlayerModel.mv1"));
	MV1SetScale(_animator->GetHandle(), Vector3(1, 1, 1) * 2.0f);
	MV1SetRotationXYZ(_animator->GetHandle(), 
		Vector3(0, _modelRotAngleY + kModelDrawOffsetAngle, 0));
	_collider->SetPosY(kColRadius);
	_animator->SetDrawOffset(Vector3(0,-kColRadius,0));

	// データのロード
	_dataLoader->Load();
	_comboHolder->LoadCombo();

	// ロードしたアニメーションを登録する
	auto dataList = _dataLoader->GetAttackAnimDataList();
	for (const auto& data : dataList) {
		_animator->SetAnimData(data.second.animName, data.second.animSpeed, false);
	}
	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(kAnimNameIdle, kBaseAnimSpeed, true);
	_animator->SetAnimData(kAnimNameDash, kBaseAnimSpeed, true);
	_animator->SetAnimData(kAnimNameJump, kJumpingAnimSpeed, false);
	_animator->SetAnimData(kAnimNameFalling, kBaseAnimSpeed, true);
	_animator->SetAnimData(kAnimNameLanding, kLandingAnimSpeed, false);

	_animator->SetAnimData(kAnimNameDodge, kDodgeAnimSpeed, false);
	_animator->SetAnimData(kAnimNameDodgeFail, kDodgeFailAnimSpeed, false);
	_animator->SetAnimData(kAnimNameReact, kBaseAnimSpeed, false);
	_animator->SetAnimData(kAnimNameDeath, kBaseAnimSpeed, false);

	// 最初のアニメーションを設定する
	_animator->SetStartAnim(kAnimNameIdle);
}

void Player::Init()
{
	// Colliderと自身を紐づける
	SetColliderParent();

	// ステート作成
	_currentState = std::make_shared<PlayerStateIdle>(GetPlayerPtr());
	// ステートの初めの処理を行う
	_currentState->OnEnter();

	// それぞれの攻撃を作成する
	std::unordered_map<PlayerActionKind, AttackColStats> attackStats = 
		_dataLoader->GetAttackStatsList();
	for (const auto& stats : attackStats) {
		std::shared_ptr<AttackCol> ptr = 
			std::make_shared<AttackCol>(_physics, PhysicsData::GameObjectTag::PlayerAttack);
		ptr->SetAttackData(stats.second);
		ptr->SetOwnerStatus(GetPlayerPtr());
		_attackCol[stats.first] = ptr;
	}
}

void Player::Update()
{
	// 無敵時間の更新
	UpdateInvTime();

	// ノックバック量更新
	UpdateKnockback(kKnockbackDecayAmount);
	
	// アニメーションの更新を行う
	float scale = GetCurrentTimeScale();
	_animator->Update(scale);

	// ステートに応じた更新を行う
	_currentState->Update();

	// 回避更新
	UpdateDodge();

	// ロックオン持続時間が残っているなら
	if (_lockonDurationTime > 0.0f) {
		// 持続時間を減らす
		_lockonDurationTime -= GetCurrentTimeScale();
	}

	//DebugDiffTracker::UpdateValue("PlayerVelY", GetVel().y);
}

void Player::Draw() const
{
	// 当たり判定を行ってからモデルの位置を設定する
	_animator->AttachPos(GetPos());
	// モデルの描画
	_animator->Draw();
}

void Player::OnCollide(const std::weak_ptr<Collider> collider)
{
	// ダメージを与える事ができるCollider側からTakeDamageを呼ぶ為
	// 処理なし
}

void Player::TakeDamage(std::shared_ptr<AttackableGameObject> attacker)
{
	// 無敵状態ならreturn
	if (IsInv()) return;

	// ダメージを受ける
	Damage(attacker->GetAttackPower());

	// カメラを少し揺らす
	GetCamera().lock()->Shake(45, 15, 20);
	
	// 削り値だけノックバック耐久値を削る
	_breakPoint -= attacker->GetBreakPower();

	// ノックバック耐久値がないなら
	if (_breakPoint <= 0.0f) {
		// 指定量だけノックバックする
		// attackerからのknockBackForce
		Vector3 knockbackForce = attacker->GetKnockbackForce();
		// ノックバック
		Position3 enemyPos = attacker->GetOwnerStatus().lock()->GetCenterPos();
		Vector3 enemyToPlayer = GetCenterPos() - enemyPos;
		enemyToPlayer.Normalized();
		AddKnockback(knockbackForce, enemyToPlayer);
	}

	// 生存していないなら
	if (_hitPoint <= 0.0f) {
		ResultDataHolder::GetInstance().ReserveCopyResultScreen();
		// 死亡ステートに遷移
		_currentState->ChangeState(std::make_shared<PlayerStateDeath>(GetPlayerPtr()));
		return;
	}

	// でなければ被弾ステートに遷移
	_currentState->ChangeState(std::make_shared<PlayerStateReact>(GetPlayerPtr()));
}

void Player::GeneratePlayerUI()
{
	// HPUI
	std::shared_ptr<UIPlayerHP> uiHp;
	int uiHpBase = LoadGraph(L"Data/Graph/EvadeStrike_HPGauge.png");
	int uiHpGauge = LoadGraph(L"Data/Graph/EvadeStrike_HPGauge_Another.png");
	int uiHpDecGauge = LoadGraph(L"Data/Graph/EvadeStrike_HPGauge_Another.png");
	uiHp = std::make_shared<UIPlayerHP>(
		uiHpBase, uiHpGauge, uiHpDecGauge);
	uiHp->SetPlayer(GetPlayerPtr());
	uiHp->SetMaxHP(GetMaxHitPoint());
	UIManager::GetInstance().RegisterUI(uiHp);
}

std::weak_ptr<PlayerComboHolder> Player::GetComboHolder()
{
	return _comboHolder;
}

float Player::GetMaxHitPoint()
{
	return kHitPoint;
}

float Player::GetHitPoint()
{
	return _hitPoint;
}

bool Player::IsAlive()
{
	return _isAlive;
}

void Player::UpdateAttackCol()
{
	// 位置更新
	Matrix4x4 forward = GetModelMatrix();
	for (auto& pair : _attackCol) {
		// 判定を行っていないなら飛ばす
		if (!pair.second->GetCollisionState()) continue;
		pair.second->PositionUpdate(forward);
		pair.second->Update();
	}
}

void Player::UpdateDodge()
{
	// エフェクト位置更新
	if (_dodgeLimitEffect.lock()) {
		if (_dodgeLimitEffect.lock()->IsPlaying()) {
			_dodgeLimitEffect.lock()->SetPos(GetCenterPos());
		}
	}
	if (_dodgeFootEffect.lock()) {
		if (_dodgeFootEffect.lock()->IsPlaying()) {
			_dodgeFootEffect.lock()->SetPos(GetPos() + Vector3(0, -kColRadius, 0));
		}
	}
	// 足元のエフェクトが消えているが
	// リミットエフェクトが出ているば場合
	else if (_dodgeLimitEffect.lock()) {
		_dodgeLimitEffect.lock()->SetPlaySpeed(0.4f);
	}

	if (_dodgeInterval == 0.0f) return;
	const float decValue = (1.0f / 60.0f);
	_dodgeInterval -= decValue;	// 更新

	// 更新後に0を下回っていれば
	if (_dodgeInterval <= 0.0f) {
		// 回避カウントをリセット
		_dodgeInterval = 0.0f;
		_dodgeCount = 0;
	}
}

Matrix4x4 Player::GetModelMatrix() const
{
	Matrix4x4 ret = MV1GetMatrix(_animator->GetHandle());
	Matrix4x4 offset = MatRotateY(kModelDrawOffsetAngle);
	return MatMultiple(offset, ret);
}

Matrix4x4 Player::GetForwardMatrix() const
{
	// プレイヤー行列作成
	Matrix4x4 rotY = MatRotateY(_rotAngleY);
	Matrix4x4 pos = MatTranslate(GetPos());
	return MatMultiple(rotY, pos);	// 位置とY軸回転情報
}

Vector3 Player::GetCameraRotAngle() const
{
	return _camera.lock()->GetRotAngle();
}

std::shared_ptr<AttackCol> Player::GetAttackCol(PlayerActionKind kind)
{
	return _attackCol[kind];
}

void Player::AddTransformForward(float add)
{
	_collider->AddVel(VTransformSR({ 0,0,add }, GetModelMatrix()));
}

void Player::AddTransformStick(float add)
{
	_collider->AddVel(VTransformSR({ 0,0,add }, GetForwardMatrix()));
}

void Player::AddTransformUp(float add)
{
	_collider->AddVel(VTransformSR({ 0,add,0 }, GetModelMatrix()));
}

void Player::Rotate()
{
	Input& input = Input::GetInstance();
	// スティックによる平面移動
	Vector2 stick = input.GetPadLeftStick();
	// スティック入力があるか
	bool stickInputState = (stick.Magnitude() >= 0.005f);

	// スティック入力がないかつ
	// スティックではない入力があった場合、そちらを優先する
	if (!stickInputState && (
		input.IsPress("Gameplay:Up") ||
		input.IsPress("Gameplay:Down") ||
		input.IsPress("Gameplay:Left") ||
		input.IsPress("Gameplay:Right")))
	{
		// xが横、yが縦
		stick = Vector2();
		if (input.IsPress("Gameplay:Up"))		stick += Vector2(0, -1);
		if (input.IsPress("Gameplay:Down"))		stick += Vector2(0, +1);
		if (input.IsPress("Gameplay:Left"))		stick += Vector2(-1, 0);
		if (input.IsPress("Gameplay:Right"))	stick += Vector2(+1, 0);

		stick.Normalized();
	}

	// 入力があった場合のみキャラクターの向きを変更
	if (stick.x != 0.0f || stick.y != 0.0f) {
		// カメラの向きを考慮しつつ目標の角度を計算
		const float cameraRot = _camera.lock()->GetRotAngleY();
		float targetAngle = atan2f(stick.y, stick.x) + -cameraRot + DX_PI_F * -0.5f;

		// 角度更新
		_rotAngleY = targetAngle;
		// 角度の正規化
		_rotAngleY = Calc::RadianNormalize(_rotAngleY);

		// 現在の角度から目標角度までの最短差分を計算
		float diff = targetAngle - _modelRotAngleY;

		// 角度の正規化
		diff = Calc::RadianNormalize(diff);
		// 1フレームで回転できる最大量に制限する
		float turnAmount = std::clamp(diff, -kTurnSpeed, kTurnSpeed);

		// モデル描画用の角度
		_modelRotAngleY += turnAmount;

		// 正規化しておく
		Calc::RadianNormalize(_modelRotAngleY);
		// 適用
		MV1SetRotationXYZ(_animator->GetHandle(), 
			Vector3(0, _modelRotAngleY + kModelDrawOffsetAngle, 0));
	}
}

void Player::SetRotAngleY(float angle)
{
	// 更新
	_rotAngleY = angle;

	// 現在の角度も正規化しておく
	_rotAngleY = Calc::RadianNormalize(_rotAngleY);
	_modelRotAngleY = _rotAngleY;
	// 適用
	MV1SetRotationXYZ(_animator->GetHandle(),
		Vector3(0, _modelRotAngleY + kModelDrawOffsetAngle, 0));
}

void Player::AddRotAngleY(float rad)
{
	// 更新
	SetRotAngleY(_rotAngleY + rad);
}

void Player::StartDodgeEffect()
{
	if (_dodgeLimitEffect.lock()) {
		_dodgeLimitEffect.lock()->DeleteEffect();
	}
	_dodgeLimitEffect = EffectManager::GetInstance().GenerateEffect(
		"Atk_Breakdown.efkefc", GetCenterPos());
	_dodgeLimitEffect.lock()->SetPlaySpeed(0.001f);

	if (_dodgeFootEffect.lock()) {
		_dodgeFootEffect.lock()->DeleteEffect();
	}
	_dodgeFootEffect = EffectManager::GetInstance().GenerateEffect(
		"Buff_MagicArea.efkefc", GetPos()+Vector3(0, -kColRadius, 0));
	_dodgeFootEffect.lock()->SetPlaySpeed(0.4f);
}

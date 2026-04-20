#include "EnemyBossBressProjectile.h"
#include "Library/Physics/Physics.h"
#include "Library/Physics/Collider.h"
#include "Library/Physics/ProjectSettings.h"
#include "Library/System/Statistics.h"
#include "Library/Objects/AttackCol.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Objects/Character/Player/Player.h"
#include <DxLib.h>
#include <cassert>

namespace {
	constexpr float kRadius = 90.0f;		// 弾の半径
	constexpr float kSpeed = 30.0f;			// 速度
	constexpr float kMaxLifeTime = 2.0f;	// 最大寿命
	constexpr float kAttackPower = 5.0f;	// 攻撃力
	constexpr float kBreakPower = 0.0f;		// ブレイク力

	// エフェクト再生速度
	constexpr float kEffectPlaySpeed = 0.85f / kMaxLifeTime;
	constexpr float kEffectScale = kRadius * 0.02f;
}

EnemyBossBressProjectile::EnemyBossBressProjectile(
	std::weak_ptr<Physics> physics,
	const Position3& startPos,
	const Vector3& direction) :
	AttackCol(
		physics,
		PhysicsData::GameObjectTag::EnemyAttack),
	_direction(direction),
	_canDelete(false),
	_lifeTime(kMaxLifeTime)
{
	// 方向ベクトルを正規化
	_direction.Normalized();
	SetPos(startPos);
}

EnemyBossBressProjectile::~EnemyBossBressProjectile()
{
}

void EnemyBossBressProjectile::Init()
{
	if (_physics.expired()) return;

	AttackColStats stats;
	stats.colRad = kRadius;
	stats.attackPower = kAttackPower;
	SetAttackData(stats);
	// 自分自身のポインタとコライダーを紐づける
	_collider->SetParent(shared_from_this());

	// 敵やプレイヤーの攻撃とは当たり判定を行わない
	_collider->AddThroughTag(PhysicsData::GameObjectTag::EnemyMinion);
	_collider->AddThroughTag(PhysicsData::GameObjectTag::EnemyBoss);
	_collider->AddThroughTag(PhysicsData::GameObjectTag::EnemyAttack);
	_collider->AddThroughTag(PhysicsData::GameObjectTag::PlayerAttack);

	// 登録した情報などを基に当たり判定を活性化
	Enable();

	// エフェクト生成
	_currentEffect = EffectManager::GetInstance().
		GenerateEffect("AtkOmen_Salamander14.efkefc", GetPos());
	float timeScale = GetCurrentTimeScale();
	_currentEffect.lock()->SetPlaySpeed(kEffectPlaySpeed * timeScale);
	_currentEffect.lock()->SetScale(Vector3(kEffectScale, kEffectScale, kEffectScale));
}

void EnemyBossBressProjectile::Update()
{
	// 削除済みならreturn
	if (_canDelete) return;

	float timeScale = GetCurrentTimeScale();
	// 寿命を減らす
	_lifeTime -= 1.0f / Statistics::kFPS * timeScale;

	// 寿命切れなら削除
	if (_lifeTime <= 0.0f) {
		_canDelete = true;
		ReleaseFromPhysics();
		if (!_currentEffect.expired()) _currentEffect.lock()->DeleteEffect();
		return;
	}

	Position3 pos = GetPos();
	// 移動
	pos += _direction * kSpeed * timeScale;

	// 位置更新
	if (_collider) {
		_collider->SetPos(pos);
	}

	// エフェクト位置更新
	if (!_currentEffect.expired()) {
		if (_currentEffect.lock()->IsPlaying()) {
			_currentEffect.lock()->SetPos(GetPos());
			float timeScale = GetCurrentTimeScale();
			_currentEffect.lock()->SetPlaySpeed(kEffectPlaySpeed * timeScale);
		}
	}
}

void EnemyBossBressProjectile::Draw() const
{
}

void EnemyBossBressProjectile::OnCollide(const std::weak_ptr<Collider> collider)
{
	if (collider.expired()) return;
	if (_canDelete) return;

	// 衝突相手のタグを取得する
	PhysicsData::GameObjectTag tag = collider.lock()->GetTag();

	// ステージに当たった場合は消滅する
	if (tag == PhysicsData::GameObjectTag::SystemWall ||
		tag == PhysicsData::GameObjectTag::StepGround) {
		_canDelete = true;
		ReleaseFromPhysics();
		if (!_currentEffect.expired()) _currentEffect.lock()->SetPlaySpeed(kEffectPlaySpeed);
		return;
	}

	// プレイヤーに当たった場合は
	// ダメージ依頼と消滅処理を行う
	if (tag == PhysicsData::GameObjectTag::Player) {
		std::shared_ptr<Player> player = std::static_pointer_cast<Player>(collider.lock()->GetParent());
		std::shared_ptr<AttackableGameObject> attackableObj =
			std::static_pointer_cast<AttackableGameObject>(shared_from_this());
		// 相手にダメージ処理を依頼する
		player->TakeDamage(attackableObj, false);

		// ダメージを与えた相手をリストに追加
		_attackedColliders.emplace_back(collider);

		// 一度ダメージを与えたら、連続ヒットを防ぐため当たり判定を無効にする
		SetCollisionState(false);
		
		// プレイヤーと当たらないようにする
		_collider->AddThroughTag(PhysicsData::GameObjectTag::Player);

		return;
	}
}

void EnemyBossBressProjectile::ReleaseFromPhysics()
{
	if (_physics.expired()) return;
	if (!_collider) return;

	_physics.lock()->Release(_collider);
	_collider = nullptr;
}

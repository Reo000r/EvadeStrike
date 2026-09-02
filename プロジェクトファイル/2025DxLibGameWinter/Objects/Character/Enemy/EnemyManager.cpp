#include "EnemyManager.h"
#include "Objects/Character/Enemy/EnemyBase.h"
#include "Objects/Character/Enemy/EnemyWeak/EnemyWeak.h"
#include "Objects/Character/Enemy/EnemyBoss/EnemyBoss.h"
#include "Objects/Character/Player/Player.h"
#include "Objects/Character/Player/JustDodge/JustDodgeManager.h"
#include "Library/System/GameObjectManager.h"
#include "Library/Geometry/Calculation.h"
#include "Objects/Camera/Camera.h"
#include "Library/System/ObjectHandleHolder.h"
#include "Loader/ObjectDataLoader.h"
#include "Library/System/Event/EventManager.h"
#include "Library/System/UI/UIGameScoreDrawer.h"
#include <cassert>
#include <algorithm>

namespace {
	// 敵の数
	constexpr int kEnemyWeakNum = 6;
	// 権限を与える敵の数
	constexpr int kMaxAttackAuthorityNum = 2;
	// 退避+射撃行動権を与える数
	constexpr int kMaxRetreatShooterNum = 2;


	const std::wstring kModelPathWeak = L"Data/Model/Character/PlayerModel.mv1";

	const std::string kStageDataPath = "Data/CSV/StagePlaceData.csv";
}

EnemyManager::EnemyManager() :
	_enemys(),
	_activeEventId(-1),
	_gameScoreDrawer()
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init(std::weak_ptr<Physics> physics,
	std::weak_ptr<Player> player,
	std::weak_ptr<Camera> camera,
	std::weak_ptr<ObjectHandleHolder> handleHolder,
	std::weak_ptr<EventManager> eventManager,
	std::weak_ptr<GameObjectManager> manager,
	std::weak_ptr<JustDodgeManager> justDodgeManager,
	std::weak_ptr<UIGameScoreDrawer> gameScoreDrawer)
{
	_physics = physics;
	_player = player;
	_camera = camera;
	_handleHolder = handleHolder;
	_eventManager = eventManager;
	_objectManager = manager;
	_justDodgeManager = justDodgeManager;
	_gameScoreDrawer = gameScoreDrawer;

	//GenerateEnemys();
}

void EnemyManager::Update()
{
	// 敵数
	size_t prevCount = _enemys.size();

	for (auto& enemy : _enemys) {
		// 敵が無効ならcontinue
		if (!enemy) continue;
		if (enemy->CanDelete()) {
			// オブジェクト管理側の解放処理
			_objectManager.lock()->Release(enemy);
		}
	}
	// 削除
	size_t count = std::erase_if(
		_enemys,
		[](std::shared_ptr<EnemyBase> target) { return target->CanDelete(); });
	
	// 敵が0体になった瞬間であれば
	if (prevCount > 0 && _enemys.empty()) {
		// 不正なIDでなければ
		if (_activeEventId != -1) {
			// 全滅イベントを通知
			_eventManager.lock()->CallEvent(_activeEventId, "DefeatEnemy");
			// IDをリセット
			_activeEventId = -1;
		}
	}

	UpdateAttackAuthority();
}

void EnemyManager::Draw() const
{
}

Position3 EnemyManager::GetPlayerPos() const
{
	return _player.lock()->GetCenterPos();
}

std::shared_ptr<EnemyBase> EnemyManager::SpawnEnemy(EnemyType type, const Position3& pos)
{
	int handle = -1;
	std::shared_ptr<EnemyBase> enemy;

	switch (type) {
	case EnemyType::Weak:
		handle = _handleHolder.lock()->GetModelHandle("EnemyWeak");
		enemy = std::make_shared<EnemyWeak>(_physics, handle);
		break;

	case EnemyType::Boss:
		handle = _handleHolder.lock()->GetModelHandle("EnemyBoss");
		enemy = std::make_shared<EnemyBoss>(_physics, handle);
		break;

	default:
		assert(false && "不明な敵種");
		return nullptr;
	}

	if (handle < 0) {
		assert(false && "モデルが読み込めなかった");
		return nullptr;
	}

	enemy->SetManager(shared_from_this());
	enemy->SetPos(pos);
	enemy->Init();
	_enemys.emplace_back(enemy);
	_objectManager.lock()->Entry(enemy);

	return enemy;
}

std::weak_ptr<EnemyBase> EnemyManager::GetNearestEnemy(Position3 pos)
{
	std::weak_ptr<EnemyBase> ret;

	for (auto& enemy : _enemys) {
		// 生きていなければreturn
		if (!enemy->IsAlive()) continue;
		// 返す敵がいない場合
		if (ret.lock() == nullptr) {
			// 先頭の敵を登録
			ret = enemy;
			continue;
		}
		Position3 retPos = ret.lock()->GetCenterPos();
		Position3 enemyPos = enemy->GetCenterPos();
		// 登録されているものより距離が近ければ
		if ((retPos - pos).SqrMagnitude() >
			(enemyPos - pos).SqrMagnitude()) {
			// もし位置が一致なかった場合は
			if (enemyPos != pos) {
				// 更新
				ret = enemy;
			}
		}
	}

	// 返す敵が不正なものなら
	if (ret.lock() == nullptr) {
		printf("敵がいない\n");
	}

	return ret;
}

bool EnemyManager::ShouldBeDisableAttackCol()
{
	return _justDodgeManager.lock()->IsJustDodge();
}

void EnemyManager::ReleaseCameraTarget(std::shared_ptr<CharacterBase> character)
{
	_camera.lock()->ReleaseOtherTarget(character);
}

std::weak_ptr<EnemyBase> EnemyManager::IsExistEnemyWithinRange(Position3 pos, float range)
{
	std::weak_ptr<EnemyBase> enemy = GetNearestEnemy(pos);
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

void EnemyManager::HitPlayerAttack()
{
	// 現在のプレイヤーの攻撃を描画に追加
	_gameScoreDrawer.lock()->AddDrawComboScore();
}

void EnemyManager::DrawAttackRangeIndicators() const
{
	for (const auto& enemy : _enemys) {
		if (!enemy) continue;
		// 生存中の敵の攻撃範囲のみ描画
		if (!enemy->IsAlive()) continue;
		enemy->DrawAttackRangeIndicator();
	}
}

void EnemyManager::UpdateAttackAuthority()
{
	// ボスを除く、生存中の雑魚敵のみを対象にする
	std::vector<std::shared_ptr<EnemyBase>> targets;
	for (auto& enemy : _enemys) {
		if (enemy->GetTag() == PhysicsData::GameObjectTag::EnemyBoss) continue;
		if (!enemy->IsAlive()) continue;
		targets.emplace_back(enemy);
	}

	// プレイヤーとの距離が近い順にソートする
	Position3 playerPos = GetPlayerPos();
	std::sort(targets.begin(), targets.end(),
		[&playerPos](const std::shared_ptr<EnemyBase>& a, const std::shared_ptr<EnemyBase>& b) {
			float distA = (a->GetCenterPos() - playerPos).SqrMagnitude();
			float distB = (b->GetCenterPos() - playerPos).SqrMagnitude();
			return distA < distB;
		});

	// 近い順にAttack、RetreatShoot、Noneを割り当てる
	int index = 0;
	for (auto& enemy : targets) {
		if (index < kMaxAttackAuthorityNum) {
			enemy->SetAuthorityType(EnemyAuthorityType::Attack);
		}
		else if (index < kMaxAttackAuthorityNum + kMaxRetreatShooterNum) {
			enemy->SetAuthorityType(EnemyAuthorityType::RetreatShoot);
		}
		else {
			enemy->SetAuthorityType(EnemyAuthorityType::None);
		}
		++index;
	}
}

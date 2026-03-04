#include "EnemyManager.h"
#include "Objects/Character/Enemy/EnemyBase.h"
#include "Objects/Character/Enemy/EnemyWeak/EnemyWeak.h"
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

namespace {
	// 敵の数
	constexpr int kEnemyWeakNum = 6;
	// 権限を与える敵の数
	constexpr int kAttackAuthorityNum = 2;

	const std::wstring kModelPathWeak = L"Data/Model/Character/PlayerModel.mv1";

	const std::string kStageDataPath = "Data/StagePlaceData.csv";
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

void EnemyManager::GenerateEnemys()
{
	// 敵の配置データをファイルから取得
	// kStageDataPathの情報をObjectDataLoaderを用いて取得
	std::vector<ObjectData> objData =
		ObjectDataLoader::LoadData(kStageDataPath.c_str());
	for (const auto& data : objData) {
		if (data.transData.name == "EnemySpawn") {
			// 敵の生成
			if (data.statusData.groupName == "EnemyWeak") {
				//SpawnEnemy(EnemyType::Weak, data.transData.pos);
			}
		}
	}

	//// 生成中心点
	//Position3 centerPos = Position3(2000.0f, 0.0f, 10000.0f);
	//// 半径
	//float radius = 1400.0f;
	//for (int i = 0; i < kEnemyWeakNum; ++i) {
	//	// 角度計算
	//	float rad = Calc::ToRadian((360.0f / kEnemyWeakNum) * i);
	//	// 円周上の座標を計算
	//	Position3 spawnPos = Position3(
	//		centerPos.x + radius * cosf(rad), 
	//		centerPos.y, 
	//		centerPos.z + radius * sinf(rad)
	//	);
	//	SpawnEnemy(EnemyType::Weak, spawnPos);
	//}
}

void EnemyManager::UpdateAttackAuthority()
{
	int authorityNum = 0;
	std::vector<std::shared_ptr<EnemyBase>> notAuthority;
	for (auto& enemy : _enemys) {
		if (enemy->HasAttackAuthority()) {
			++authorityNum;
		}
		else {
			notAuthority.emplace_back(enemy);
		}
	}
	// 攻撃権がない敵数
	int notAuthorityNum = notAuthority.size();
	// 攻撃権が余っているかつ
	// 攻撃権がない敵数が1以上なら
	while (authorityNum < kAttackAuthorityNum && 
		notAuthorityNum > 0) {
		// 権限を与えたい番号
		int r = GetRand(notAuthorityNum - 1);
		std::shared_ptr<EnemyBase> addTarget = notAuthority[r];
		addTarget->SetAttackState(true);
		// 登録解除
		size_t count = std::erase_if(
			notAuthority,
			[addTarget](std::shared_ptr<EnemyBase> target) { return target == addTarget; });
		if (count <= 0) {
			assert(false && "指定の敵が見つからなかった");
			return;
		}
		// 与えた攻撃権数と攻撃権がない敵数を更新
		--notAuthorityNum;
		++authorityNum;
	}
}

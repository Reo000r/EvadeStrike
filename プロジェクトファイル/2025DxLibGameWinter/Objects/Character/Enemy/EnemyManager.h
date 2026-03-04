#pragma once
#include "Library/Geometry/Vector3.h"
#include <memory>
#include <vector>

class EnemyBase;
class CharacterBase;
class Player;
class Camera;
class ObjectHandleHolder;
class EventManager;
class GameObjectManager;
class JustDodgeManager;
class Physics;
class UIGameScoreDrawer;

enum class EnemyType {
	Weak,
};

/// <summary>
/// 敵を管理
/// </summary>
class EnemyManager : public std::enable_shared_from_this<EnemyManager>
{
public:
	EnemyManager();
	~EnemyManager();

	void Init(std::weak_ptr<Physics> physics, 
		std::weak_ptr<Player> player,
		std::weak_ptr<Camera> camera,
		std::weak_ptr<ObjectHandleHolder> handleHolder,
		std::weak_ptr<EventManager> eventManager,
		std::weak_ptr<GameObjectManager> manager,
		std::weak_ptr<JustDodgeManager> justDodgeManager,
		std::weak_ptr<UIGameScoreDrawer> gameScoreDrawer);
	void Update();
	void Draw() const;

	Position3 GetPlayerPos() const;

	/// <summary>
	/// 全滅時に呼ぶID
	/// </summary>
	/// <param name="id"></param>
	void SetActiveEventId(int id) { _activeEventId = id; }

	/// <summary>
	/// <para> 指定された型の敵を召喚する </para>
	/// <para> 不明な敵種であったりモデルが読み込めなければnullptrを返す </para>
	/// </summary>
	/// <param name="type"></param>
	/// <param name="pos"></param>
	/// <returns></returns>
	std::shared_ptr<EnemyBase> SpawnEnemy(EnemyType type, const Position3& pos);

	/// <summary>
	/// 指定された位置から最も近い敵を返す
	/// 敵がいなければ
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	std::weak_ptr<EnemyBase> GetNearestEnemy(Position3 pos);

	/// <summary>
	/// 攻撃判定を無効化するべきかどうか
	/// </summary>
	/// <returns></returns>
	bool ShouldBeDisableAttackCol();

	void ReleaseCameraTarget(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// <para> 指定範囲内に敵がいるか </para>
	/// <para> いれば有効な敵を返す </para>
	/// <para> いなければ不正な敵を返す </para>
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="range"></param>
	/// <returns></returns>
	std::weak_ptr<EnemyBase> IsExistEnemyWithinRange(Position3 pos, float range);

	/// <summary>
	/// プレイヤーの攻撃が当たったことを通知
	/// </summary>
	void HitPlayerAttack();

private:
	void GenerateEnemys();

	void UpdateAttackAuthority();

	std::vector<std::shared_ptr<EnemyBase>> _enemys;
	std::weak_ptr<Player> _player;
	std::weak_ptr<Physics> _physics;
	std::weak_ptr<Camera> _camera;
	std::weak_ptr<ObjectHandleHolder> _handleHolder;
	std::weak_ptr<EventManager> _eventManager;
	std::weak_ptr<GameObjectManager> _objectManager;
	std::weak_ptr<JustDodgeManager> _justDodgeManager;
	std::weak_ptr<UIGameScoreDrawer> _gameScoreDrawer;

	int _activeEventId;
};


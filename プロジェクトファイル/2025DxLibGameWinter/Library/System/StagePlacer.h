#pragma once
#include "Loader/ObjectDataLoader.h"
#include <memory>
#include <vector>

class ObjectHandleHolder;
class EventManager;
class Collider;
class Physics;

/// <summary>
/// ゲーム上にオブジェクトを配置する
/// </summary>
class StagePlacer
{
public:
	StagePlacer();
	virtual ~StagePlacer();

	void SetObjectHandleHolder(std::shared_ptr<ObjectHandleHolder> holder);

	/// <summary>
	/// オブジェクト配置
	/// </summary>
	std::vector<ObjectData> Place(
		std::weak_ptr<Physics> physics, 
		std::weak_ptr<EventManager> eventManager);

	void Draw();

	std::vector<std::shared_ptr<Collider>> GetStageColliders() { return _colliders; }

private:

	std::weak_ptr<ObjectHandleHolder> _holder;
	// 配置オブジェクトのハンドル
	//std::vector<int> _objectHandles;
	std::vector<std::shared_ptr<Collider>> _colliders;
};


#pragma once
#include "ProjectSettings.h"
#include <forward_list>

class Collider;
class Physics;

class ColliderData abstract {
public:
	// コンストラクタ
	ColliderData(PhysicsData::ColliderKind kind, bool isTrigger, bool isCollision) :
		_kind(kind),
		_isTrigger(isTrigger),
		_isCollision(isCollision)
	{
	}

	virtual ~ColliderData() = default;

	// getter
	PhysicsData::ColliderKind GetKind() const { return _kind; }
	bool IsTrigger() const { return _isTrigger; }
	bool IsCollision() const { return _isCollision; }

	/// <summary>
	/// 当たり判定を無視するタグの追加
	/// </summary>
	/// <param name="tag"></param>
	void AddThroughTag(PhysicsData::GameObjectTag tag);
	/// <summary>
	/// 当たり判定を無視するタグの削除
	/// </summary>
	/// <param name="tag"></param>
	void RemoveThroughTag(PhysicsData::GameObjectTag tag);

	/// <summary>
	/// 当たり判定を無視するタグかどうか
	/// </summary>
	/// <param name="target">タグ</param>
	/// <returns>無視する場合はtrue</returns>
	bool IsThroughTarget(const PhysicsData::GameObjectTag target) const;

	/// <summary>
	/// 当たり判定を行うか切り替える
	/// </summary>
	/// <param name="isCollision"></param>
	void SetCollisionState(bool isCollision) { _isCollision = isCollision; }

private:
	// 当たり判定を無視するタグのリスト
	std::forward_list<PhysicsData::GameObjectTag>	_throughTags;

	// 以降の変数のアクセス権を渡す
	friend Collider;
	friend Physics;

	PhysicsData::ColliderKind	_kind;
	bool	_isTrigger;
	bool	_isCollision;
};


#include "ColliderData.h"

#include <cassert>

void ColliderData::AddThroughTag(PhysicsData::GameObjectTag tag)
{
	// なければendが帰ってくる
	auto it = std::find(
		_throughTags.begin(),
		_throughTags.end(),
		tag);
	// endであれば(登録されていなければ)
	if (it == _throughTags.end()) {
		_throughTags.emplace_front(tag);
	}
	else {
		assert(false && "指定のタグは登録済");
	}
}

void ColliderData::RemoveThroughTag(PhysicsData::GameObjectTag tag)
{
	// 登録解除(eraseif 要C++20)
	size_t count = std::erase_if(
		_throughTags,
		[tag](PhysicsData::GameObjectTag target) { return target == tag; });
	// 登録されてなかったらassert
	if (count <= 0) {
		assert(false && "指定のタグは未登録");
	}
}

bool ColliderData::IsThroughTarget(const PhysicsData::GameObjectTag target) const
{
	auto it = std::find(_throughTags.begin(), _throughTags.end(), target);
	// endでなければ(見つかれば)true、そうでなければfalse
	return (it != _throughTags.end());
}

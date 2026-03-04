#include "ColliderDataCapsule.h"

ColliderDataCapsule::ColliderDataCapsule(bool isTrigger, bool isCollision, float rad, Vector3 startToEnd) :
	ColliderData(PhysicsData::ColliderKind::Capsule, isTrigger, isCollision),
	_radius(rad),
	_startToEnd(startToEnd)
{
}

Vector3 ColliderDataCapsule::GetStartPos(Position3 pos)
{
	return pos;
}

Vector3 ColliderDataCapsule::GetEndPos(Position3 pos)
{
	return (pos + _startToEnd);
}

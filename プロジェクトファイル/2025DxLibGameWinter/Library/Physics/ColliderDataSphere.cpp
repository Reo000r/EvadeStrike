#include "ColliderDataSphere.h"

ColliderDataSphere::ColliderDataSphere(bool isTrigger, bool isCollision, float rad) :
	ColliderData(PhysicsData::ColliderKind::Sphere, isTrigger, isCollision),
	_radius(rad)
{
}

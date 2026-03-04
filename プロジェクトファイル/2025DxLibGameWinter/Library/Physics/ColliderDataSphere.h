#pragma once
#include "ColliderData.h"

class ColliderDataSphere : public ColliderData{
public:
	ColliderDataSphere(bool isTrigger, bool isCollision, float rad);

	float GetRad() { return _radius; }

private:
	// 以降の変数のアクセス権を渡す
	friend Collider;

	// 半径
	float _radius;
};


#pragma once
#include "ColliderData.h"

class Collider;
class Physics;

class ColliderDataCapsule : public ColliderData {
public:
	ColliderDataCapsule(bool isTrigger, bool isCollision, 
		float rad, Vector3 startToEnd = Vector3Up());
	
	/// <summary>
	/// 始点を返す(原点)
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	Vector3 GetStartPos(Position3 pos);
	/// <summary>
	/// 終点を返す(pos+offset)
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	Vector3 GetEndPos(Position3 pos);
	float GetRad() { return _radius; }
	Vector3 GetAngle() { return _startToEnd.Normalize(); }
	float GetDist() { return _startToEnd.Magnitude(); }

	Vector3 GetStartToEnd() { return _startToEnd; }
	void SetStartToEnd(const Vector3& startToEnd) { _startToEnd = startToEnd; }

	// 以降の変数のアクセス権を渡す
	friend Collider;

private:
	// 半径
	float _radius;
	// 純粋なstartからendまでのベクトル
	// 位置情報は含まない
	Vector3 _startToEnd;
};

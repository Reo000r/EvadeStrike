#pragma once
#include "ColliderData.h"
#include <memory>
#include <DxLib.h>

class Collider;
class Physics;
class Model;

class ColliderDataPolygon : public ColliderData
{
public:
	ColliderDataPolygon(bool isTrigger, bool isCollision,
		int modelHandle);
	virtual ~ColliderDataPolygon();

	MV1_COLL_RESULT_POLY_DIM& GetHitDim() { return _hitDim; }
	void SetHitDim(MV1_COLL_RESULT_POLY_DIM& dim) { _hitDim = dim; }
	MV1_COLL_RESULT_POLY GetLineHit()const { return _lineHit; }
	void SetLineHit(MV1_COLL_RESULT_POLY lineHit) { _lineHit = lineHit; }
	
	// 壁と床の最近接点
	Vector3 GetNearWallHitPos() const { return _nearWallHitPos; }
	Vector3 GetNearFloorHitPos() const { return _nearFloorHitPos; }
	
	int GetModelHandle() const { return _modelHandle; }
	void SetModelHandle(int modelHandle) { _modelHandle = modelHandle; }
	
	// 連続衝突が起こったか
	bool IsContinuousCollision()const { return _isContinuousCollision; };
	void SetContinuousCollision(bool isContinuousCollision) { _isContinuousCollision = isContinuousCollision; };

private:
	// 以降の変数のアクセス権を渡す
	friend Collider;

	// 当たり判定をするモデル
	int _modelHandle;
	// 当たったポリゴンの情報
	MV1_COLL_RESULT_POLY_DIM _hitDim;
	// 線分とポリゴンの当たり判定情報
	MV1_COLL_RESULT_POLY _lineHit;
	// 当たった最も近い壁ポリゴンの座標
	Vector3 _nearWallHitPos;
	// 当たった最も近い床ポリゴンの座標
	Vector3 _nearFloorHitPos;
	// 連続衝突が行われているか
	bool _isContinuousCollision;
};

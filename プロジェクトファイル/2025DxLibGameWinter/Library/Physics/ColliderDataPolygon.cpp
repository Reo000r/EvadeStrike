#include "ColliderDataPolygon.h"
#include "Library/Objects/Model.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	// 当たり判定用のポリゴンメッシュを収めているフレーム名
	const std::wstring kCollisionFrameName = L"CollisionFrame";
}

ColliderDataPolygon::ColliderDataPolygon(bool isTrigger, bool isCollision,
	int modelHandle) :
	ColliderData(PhysicsData::ColliderKind::Polygon, isTrigger, isCollision),
	_modelHandle(modelHandle),
	_nearWallHitPos(),
	_nearFloorHitPos(),
	_hitDim(),
	_lineHit(),
	_isContinuousCollision(false)
{


	//// 参考:
	//// https://dxlib.xsrv.jp/function/dxfunc_3d_model_3.html#R9N1

	//// 当たり判定用のフレームを検索
	//_collisionFrameIndex = MV1SearchFrame(GetModelHadle(), kCollisionFrameName.c_str());
	//// -1:エラー -2:無かった
	//assert(_collisionFrameIndex != -1 && "検索失敗");
	//assert(_collisionFrameIndex != -2 && "専用名のフレームがない");
	//
	//// 当たり判定の準備
	//constexpr int divNum = 8;	// 空間分割数
	//int ret = MV1SetupCollInfo(GetModelHadle(), _collisionFrameIndex, 
	//	divNum, divNum, divNum);
	//assert(ret != -1 && "準備失敗");

	//// 当たり判定用のポリゴンメッシュの描画を切る
	//MV1SetFrameVisible(GetModelHadle(), _collisionFrameIndex, false);
}

ColliderDataPolygon::~ColliderDataPolygon()
{
}

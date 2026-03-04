#pragma once
#include "Library/Geometry/Vector3.h"
#include <memory>
#include <vector>
#include "Library/Physics/Collider.h"
#include "Library/Physics/ColliderDataSphere.h"
#include "Library/Physics/ColliderDataCapsule.h"
#include "Library/Physics/ColliderDataPolygon.h"

// 当たり判定系の処理を行う関数をまとめたファイル


/// <summary>
/// 球同士の接触判定を行う
/// </summary>
/// <param name="sphereA"></param>
/// <param name="sphereB"></param>
/// <param name="nextAPos"></param>
/// <param name="nextBPos"></param>
/// <returns></returns>
bool CheckHitSphereSphere(
	const std::shared_ptr<ColliderDataSphere> sphereA,
	const std::shared_ptr<ColliderDataSphere> sphereB,
	Position3 nextAPos, Position3 nextBPos);

/// <summary>
/// カプセル同士の接触判定を行う
/// </summary>
/// <param name="capsuleA"></param>
/// <param name="capsuleB"></param>
/// <param name="nextAPos"></param>
/// <param name="nextBPos"></param>
/// <returns></returns>
bool CheckHitCapsuleCapsule(
	const std::shared_ptr<ColliderDataCapsule> capsuleA,
	const std::shared_ptr<ColliderDataCapsule> capsuleB,
	Position3 nextAPos, Position3 nextBPos);

/// <summary>
/// 球とカプセルの接触判定を行う
/// </summary>
/// <param name="sphere"></param>
/// <param name="capsule"></param>
/// <param name="nextAPos"></param>
/// <param name="nextBPos"></param>
/// <returns></returns>
bool CheckHitSphereCapsule(
	const std::shared_ptr<ColliderDataSphere> sphere,
	const std::shared_ptr<ColliderDataCapsule> capsule,
	Position3 nextSpherePos, Position3 nextCapsulePos);

bool CheckHitCapsulePolygon(
	const std::shared_ptr<Collider> capsule,
	const std::shared_ptr<Collider> polygon);


/// <summary>
/// 球同士の押し戻し処理を行う
/// </summary>
void FixNextPosSphereSphere(
	const std::shared_ptr<Collider> priSphere, 
	const std::shared_ptr<Collider> secSphere, 
	bool isMutualPushback);

/// <summary>
/// カプセル同士の押し戻し処理を行う
/// </summary>
void FixNextPosCapsuleCapsule(
	const std::shared_ptr<Collider> priCapsule, 
	const std::shared_ptr<Collider> secCapsule,
	bool isMutualPushback);

/// <summary>
/// 球とカプセルの押し戻し処理を行う
/// </summary>
void FixNextPosSphereCapsule(
	const std::shared_ptr<Collider> primary,
	const std::shared_ptr<Collider> secondary,
	bool isMutualPushback);

/// <summary>
/// <para> カプセルとポリゴンの押し戻し処理を行う </para>
/// <para> ポリゴンは押し戻し処理を行わない </para>
/// </summary>
void FixNextPosCapsulePolygon(
	const std::shared_ptr<Collider> capsule,
	const std::shared_ptr<Collider> polygon,
	bool isMutualPushback);


/// <summary>
/// 床ポリゴンと壁ポリゴンに分ける
/// </summary>
void AnalyzeWallAndFloor(MV1_COLL_RESULT_POLY_DIM hitDim, Vector3 nextPos,
	std::vector<MV1_COLL_RESULT_POLY>& wall,
	std::vector<MV1_COLL_RESULT_POLY>& floorAndRoof);

/// <summary>
/// 壁と当たった時の処理 カプセル
/// </summary>
Vector3 HitWallCP(const Vector3& headPos, const Vector3& legPos, float shortDis,
	std::vector<MV1_COLL_RESULT_POLY>& wall);
/// <summary>
/// 床の高さに合わせる カプセル
/// </summary>
bool HitFloorCP(const std::shared_ptr<Collider> other, 
	const Vector3& legPos, const Vector3& headPos, float shortDis,
	std::vector<MV1_COLL_RESULT_POLY>& floorAndRoof);
/// <summary>
/// 天井に当たった時の処理 カプセル
/// </summary>
void HitRoofCP(const std::shared_ptr<Collider> other, 
	const Vector3& headPos, float shortDis,
	std::vector<MV1_COLL_RESULT_POLY>& floorAndRoof);





/// <summary>
/// 点と線分の最近接点を求める
/// </summary>
/// <param name="point">点</param>
/// <param name="start">線分の始点</param>
/// <param name="end">線分の終点</param>
/// <returns>線分上の最近接点</returns>
Position3 ClosestPointPointAndSegment(const Position3& point, 
	const Position3& start, const Position3& end);

/// <summary>
/// 2つの線分の最近接点を求める
/// </summary>
/// <param name="startA">線分A始点</param>
/// <param name="endA">線分A終点</param>
/// <param name="startB">線分B始点</param>
/// <param name="endB">線分B終点</param>
/// <param name="closestPointA">A上の最近接点</param>
/// <param name="closestPointB">B上の最近接点</param>
void ClosestPointSegments(
	const Position3& startA, const Position3& endA,
	const Position3& startB, const Position3& endB,
	Position3& closestPointA, Position3& closestPointB);

/// <summary>
/// 2つの線分の最近接点を求めるためのパラメータを計算する
/// (ClosestPointSegmentsの補助関数)
/// </summary>
/// <param name="lenSq1">線分1の長さの2乗</param>
/// <param name="lenSq2">線分2の長さの2乗</param>
/// <param name="dot12">線分ベクトル同士の内積</param>
/// <param name="dot1r">線分1とオフセットベクトルの内積</param>
/// <param name="dot2r">線分2とオフセットベクトルの内積</param>
/// <param name="param1">出力用のパラメータ1</param>
/// <param name="param2">出力用のパラメータ2</param>
void CalculateClosestSegmentParameters(
	float lenSq1, float lenSq2, float dot12,
	float dot1r, float dot2r,
	float& param1, float& param2);

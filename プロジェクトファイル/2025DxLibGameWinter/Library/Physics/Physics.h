#pragma once
#include "Library/Geometry/Vector3.h"
#include <DxLib.h>
#include <vector>
#include <memory>
#include <forward_list>

class Collider;

/// <summary>
/// 物理挙動を司る
/// </summary>
class Physics final {
public:
	Physics();
	~Physics() = default;

	/// <summary>
	/// オブジェクト登録
	/// </summary>
	void Entry(std::shared_ptr<Collider> collider);

	/// <summary>
	/// オブジェクト登録解除
	/// </summary>
	void Release(std::shared_ptr<Collider> collider);

	void Update();

	void DebugCollisionDraw(std::shared_ptr<Collider>& collider, Position3& pos);

	/// <summary>
	/// 指定のColliderが登録されているか確認
	/// </summary>
	/// <param name="collider"></param>
	/// <returns></returns>
	bool IsRegistered(std::shared_ptr<Collider> collider);

private:

	// OnCollideの遅延通知のためのデータ
	struct OnCollideInfo {
		std::shared_ptr<Collider> owner;
		std::shared_ptr<Collider> colider;

		// ペアとして一致するか判定する比較演算子
		bool operator==(const OnCollideInfo& other) const {
			return owner == other.owner && colider == other.colider;
		}
	};

	/// <summary>
	/// <para> オブジェクトが当たっているかを確認し、 </para>
	/// <para> 当たっているオブジェクト情報を返す </para>
	/// <para> owner:Wall,col:Playerなどのパターンは省く </para>
	/// </summary>
	/// <returns></returns>
	std::forward_list<OnCollideInfo> CheckCollide();

	/// <summary>
	/// 当たっているかどうかだけ判定
	/// </summary>
	bool IsCollide(const std::shared_ptr<Collider> objA, const std::shared_ptr<Collider> objB) const;

	/// <summary>
	/// <para> 第一引数のColliderを動かないものとして、 </para>
	/// <para> 第二引数に入ったColliderの位置を補正する </para>
	/// <para> 第三引数にtrueが入っていた場合はそれらを無視し両方を押し戻す </para>
	/// </summary>
	/// <param name="primary">動かないCollider</param>
	/// <param name="secondary">補正を行うCollider</param>
	/// <param name="isMutualPushback">両方を押し戻すか</param>
	void FixNextPosition(std::shared_ptr<Collider> primary, std::shared_ptr<Collider> secondary, bool isMutualPushback);
	/// <summary>
	/// 位置決定
	/// </summary>
	void FixPosition();

	/// <summary>
	///	重力を適用
	/// </summary>
	void ApplyGravity();

	/// <summary>
	/// 床ポリゴンと壁ポリゴンに分ける
	/// </summary>
	void AnalyzeWallAndFloor(MV1_COLL_RESULT_POLY_DIM hitDim, Vector3 nextPos);

	/// <summary>
	/// 壁と当たった時の処理 カプセル
	/// </summary>
	Vector3 HitWallCP(const Vector3& headPos, const Vector3& legPos, float shortDis);
	/// <summary>
	/// 床の高さに合わせる カプセル
	/// </summary>
	bool HitFloorCP(const std::shared_ptr<Collider> other, const Vector3& legPos, const Vector3& headPos, float shortDis);
	/// <summary>
	/// 天井に当たった時の処理 カプセル
	/// </summary>
	void HitRoofCP(const std::shared_ptr<Collider> other, const Vector3& headPos, float shortDis);

	// 登録されたColliderのリスト
	std::forward_list<std::shared_ptr<Collider>> _colliders;

	// 壁
	std::vector<MV1_COLL_RESULT_POLY> _wall;
	// 床と屋根
	std::vector<MV1_COLL_RESULT_POLY> _floorAndRoof;
};


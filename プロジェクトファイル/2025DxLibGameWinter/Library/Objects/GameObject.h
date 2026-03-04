#pragma once
#include "Library/Physics/Collider.h"
#include "Library/Physics/Rigidbody.h"

/// <summary>
/// ゲームに登場するオブジェクトの基底クラス
/// </summary>
class GameObject abstract : public std::enable_shared_from_this<GameObject>
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="physics"></param>
	/// <param name="priority">位置補正の優先度</param>
	/// <param name="tag">タグ</param>
	/// <param name="colliderKind">当たり判定種別</param>
	/// <param name="isTrigger">トリガー</param>
	/// <param name="isCollision">当たり判定可否</param>
	/// <param name="useGravity">重力使用可否</param>
	GameObject(
		std::weak_ptr<Physics> physics,
		PhysicsData::Priority priority,
		PhysicsData::GameObjectTag tag,
		PhysicsData::ColliderKind colliderKind,
		bool isTrigger,
		bool isCollision,
		bool useGravity);
	virtual ~GameObject() = default;

	virtual void Init() abstract;
	virtual void Update() abstract;
	virtual void Draw() const abstract;

	void EntryPhysics(std::weak_ptr<Physics> physics);
	void ReleasePhysics();
	
	void SetCollider(std::shared_ptr<Collider> collider) { _collider = collider; }
	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	virtual void OnCollide(const std::weak_ptr<Collider> collider) abstract;

	/// <summary>
	/// <para> タイムスケール更新 </para>
	/// <para> 通常速度に戻るフレーム更新 </para>
	/// </summary>
	void UpdateTimeScale();

	/// <summary>
	/// このオブジェクトのタイムスケールを返す
	/// </summary>
	/// <returns></returns>
	float GetCurrentTimeScale() const;
	/// <summary>
	/// 通常のタイムスケールに戻す期間を設定
	/// </summary>
	/// <returns></returns>
	float SetToDefaultScaleFrame(int frame);
	
	/// <summary>
	/// ヒットストップを設定
	/// </summary>
	/// <param name="time">フレーム数</param>
	/// <param name="isShake">モデルの振動を行わせるか</param>
	/// <param name="magnitude">振動量</param>
	void SetHitStop(int frame, bool isShake, float magnitude);
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool IsHitStop() const { return (_hitStopFrame > 0.0f); }
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Vector3 GetHitStopShakeVec() const { return _currentHitStopShakeVec; }

	PhysicsData::GameObjectTag GetTag() const { return _collider->GetTag(); }
	// 位置補正優先度情報を返す
	PhysicsData::Priority GetPriority() const { return _collider->GetPriority(); }

	Vector3 GetPos() const { return _collider->GetPos(); }
	Vector3 GetCenterPos() const { return _collider->GetCenterPos(); }
	Vector3 CalcNextPos() const { return _collider->CalcNextPos(); }
	Vector3 GetVel() const { return _collider->GetVel(); }
	Vector3 GetDir() const { return _collider->GetDir(); }
	bool UseGravity() const { return _collider->UseGravity(); }
	void SetPos(const Position3& set) const { _collider->SetPos(set); }
	void SetVel(const Vector3& set) const { _collider->SetVel(set); }
	void AddPos(const Position3& add) const { return _collider->AddPos(add); }
	void AddVel(const Vector3& add) const { return _collider->AddVel(add); }

protected:

	/// <summary>
	/// 振動ベクトルを返す
	/// </summary>
	/// <param name="magnitude"></param>
	/// <returns></returns>
	Vector3 CalcShakeVec(float magnitude);

	// 当たり判定
	std::shared_ptr<Collider> _collider;

	std::weak_ptr<Physics> _physics;

	// 通常タイムスケールに戻っている期間
	int _toDefaultScaleFrame;

	// ヒットストップ
	int _hitStopFrame;
	bool _isHitStopShake;
	float _hitStopMagnitude;
	Vector3 _currentHitStopShakeVec;
};


#pragma once
#include "Library/Objects/Character/CharacterBase.h"

class EnemyManager;

// 攻撃権の種類
enum class EnemyAuthorityType {
	None,
	Attack,
	RetreatShoot,
};

class EnemyBase : public CharacterBase
{
public:
	EnemyBase(std::weak_ptr<Physics> physics, int modelHandle,
		PhysicsData::Priority priority = PhysicsData::Priority::Middle,
		PhysicsData::GameObjectTag tag = PhysicsData::GameObjectTag::EnemyMinion);
	virtual ~EnemyBase();
	
	void SetManager(std::shared_ptr<EnemyManager> manager) { _manager = manager; }

	/// <summary>
	/// 攻撃権を持っているか返す
	/// </summary>
	bool HasAttackAuthority() const { return _authorityType == EnemyAuthorityType::Attack; }
	/// <summary>
	/// 退避+射撃行動権を持っているか返す
	/// </summary>
	bool HasRetreatShootAuthority() const { return _authorityType == EnemyAuthorityType::RetreatShoot; }
	/// <summary>
	/// 現在の権限種別を返す
	/// </summary>
	EnemyAuthorityType GetAuthorityType() const { return _authorityType; }
	/// <summary>
	/// 権限種別を設定する(EnemyManagerからのみ呼ばれる想定)
	/// </summary>
	void SetAuthorityType(EnemyAuthorityType type) { _authorityType = type; }


	/// <summary>
	/// 攻撃可否を返す
	/// </summary>
	/// <returns></returns>
	virtual bool CanAttack() abstract;
	/// <summary>
	/// 削除可否を返す
	/// </summary>
	/// <returns></returns>
	bool CanDelete();
	/// <summary>
	/// 攻撃可否を設定
	/// </summary>
	/// <param name="canDelete"></param>
	void SetDeleteState(bool canDelete);

	/// <summary>
	/// カメラのロックオンを解除する
	/// </summary>
	void ReleaseCameraTarget();

	/// <summary>
	/// 攻撃判定を無効化する
	/// </summary>
	virtual void DisableAttackCol() abstract;

protected:
	/// <summary>
	/// 自身の当たり判定設定を行う
	/// </summary>
	/// <param name="rad"></param>
	/// <param name="startToEnd"></param>
	void SetColliderData(float rad, Vector3 startToEnd);

	/// <summary>
	/// Physicsから当たり判定登録を解除する
	/// </summary>
	void ReleasePhysics();

protected:

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<AnimationModel> GetAnimator() const { return _animator; }
	/// <summary>
	/// モデルの行列を返す
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetModelMatrix() const;

	/// <summary>
	/// <para> 正面に補正した行列を返す </para>
	/// <para> 厳密なモデルの行列ではない </para>
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetForwardMatrix() const;

	/// <summary>
	/// <para> プレイヤーの方向を向く </para>
	/// <para> 速度制限なし </para>
	/// </summary>
	void RotateToPlayer();
	/// <summary>
	/// <para> プレイヤーの方向を向く </para>
	/// <para> 速度制限あり </para>
	/// </summary>
	/// <param name="speed"></param>
	void RotateToPlayer(float speed);
	/// <summary>
	/// <para> プレイヤーの反対方向を向く </para>
	/// <para> 速度制限あり </para>
	/// </summary>
	/// <param name="speed"></param>
	void RotateOppositeToPlayer(float speed);

	/// <summary>
	/// プレイヤーが攻撃範囲にいるか
	/// </summary>
	/// <returns></returns>
	/// <param name="attackRangeInPlayer"></param>
	bool CanAttackRangeInPlayer(float attackRangeInPlayer);
	/// <summary>
	/// ステート変更待機時間がないか
	/// </summary>
	/// <returns></returns>
	bool IsNothingStateTransitionTime() const;
	/// <summary>
	/// 攻撃待機時間がないか
	/// </summary>
	/// <returns></returns>
	bool IsNothingAttackInterval() const;
	/// <summary>
	/// ステート変更待機時間を設定
	/// </summary>
	/// <returns></returns>
	void SetStateTransitionTime(float time);
	/// <summary>
	/// 攻撃待機時間を設定
	/// </summary>
	/// <returns></returns>
	void SetAttackInterval(float interval);

	/// <summary>
	/// 向いている方へ指定の移動量加算する
	/// </summary>
	/// <param name="add"></param>
	void AddTransformForward(float add);
	/// <summary>
	/// 移動量をなくす
	/// </summary>
	/// <returns></returns>
	void Stop() { _collider->SetVel(Vector3Zero()); }

	/// <summary>
	/// managerへプレイヤーからの攻撃が当たった事を通知
	/// </summary>
	void HitPlayerAttack();

	std::shared_ptr<AnimationModel> _animator;	// モデルとアニメーションを管理

	// 敵管理 
	std::weak_ptr<EnemyManager> _manager;

	float _rotAngleY;		// Y軸回転量
	float _targetRotAngleY;	// 目標となる回転量
	float _stateTransitionTime;	// ステート変更待機時間
	float _attackInterval;		// 攻撃待機時間

	// 攻撃権種
	EnemyAuthorityType _authorityType;
	bool _canDelete;
};


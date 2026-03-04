#pragma once
#include "Library/Objects/Character/CharacterBase.h"

class EnemyManager;

class EnemyBase : public CharacterBase
{
public:
	EnemyBase(std::weak_ptr<Physics> physics, int modelHandle);
	~EnemyBase();
	
	void SetManager(std::shared_ptr<EnemyManager> manager) { _manager = manager; }

	/// <summary>
	/// 攻撃権を持っているか返す
	/// </summary>
	/// <returns></returns>
	bool HasAttackAuthority();
	/// <summary>
	/// 攻撃可否を返す
	/// </summary>
	/// <returns></returns>
	virtual bool CanAttack() abstract;
	/// <summary>
	/// 攻撃可否を返す
	/// </summary>
	/// <returns></returns>
	bool CanDelete();
	/// <summary>
	/// 攻撃可否を設定
	/// </summary>
	/// <param name="canAttack"></param>
	void SetAttackState(bool canAttack);
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

	bool _canAttack;
	bool _canDelete;
};


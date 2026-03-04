#pragma once
#include "EnemyWeak.h"

// 各ステートで使用したい定数
namespace {
	// 追跡速度
	constexpr float kChaseSpeed = 5.0f;
	// 攻撃時の移動
	constexpr float kEnterForwardVel = 7.0f;
	// 特定の状態に遷移する範囲
	constexpr float kAttackRangeInPlayer = 800.0f;	// 攻撃遷移範囲
	constexpr float kIdleRangeInPlayer = 1200.0f;	// 待機遷移範囲
	constexpr float kChaseRangeInPlayer = kIdleRangeInPlayer + 2000.0f;	// 追跡遷移範囲
	// 回転速度
	constexpr float kTurnSpeed = 0.2f;

	constexpr float kStateTransitionTime = 1.0f;// ステート遷移時間
	constexpr float kAttackInterval = 4.0f;		// 攻撃待機時間

	constexpr float kAttackAnimSpeed = 0.5f;
	constexpr float kDeathAnimSpeed = 2.0f;		// 死亡アニメーション速度

	constexpr int kReactDefaultScaleFrame = 4;	// 被弾時タイムスケールを戻すフレーム

	constexpr float kRangeEnemyDist = 10.0f;
}

class AttackCol;

class EnemyWeakStateBase
{
public:
	EnemyWeakStateBase(std::weak_ptr<EnemyWeak> parent);
	virtual ~EnemyWeakStateBase() = default;

	/// <summary>
	/// ステート開始直後の処理
	/// </summary>
	virtual void OnEnter() abstract;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() abstract;
	/// <summary>
	/// ステート終了直前の処理
	/// </summary>
	virtual void OnExit() abstract;

	/// <summary>
	/// <para> ステート遷移確認を行う </para>
	/// <para> trueが帰ってきた場合、ステートが変わった後の為即returnすること </para>
	/// </summary>
	/// <returns></returns>
	virtual bool UpdateStateTransition();

	/// <summary>
	/// <para> ステートの切り替えを行う </para>
	/// <para> 既存のステートは廃棄される </para>
	/// </summary>
	void ChangeState(std::shared_ptr<EnemyWeakStateBase> nextState);

protected:

	// 関数を派生先で使えるようにしたもの
	// これを行うことでこのクラスがfriendになっていれば派生先で使用可能になる

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<Physics> GetPhysics() const;
	/// <summary>
	/// アニメーター取得
	/// </summary>
	/// <returns></returns>
	const std::shared_ptr<AnimationModel> GetAnimator() const;
	/// <summary>
	/// マネージャー取得
	/// </summary>
	/// <returns></returns>
	const std::weak_ptr<EnemyManager> GetEnemyManager() const;

	/// <summary>
	/// <para> 正面に補正した行列を返す </para>
	/// <para> 厳密なモデルの行列ではない </para>
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetForwardMatrix() const;

	/// <summary>
	/// 攻撃種別に応じた攻撃判定を返す
	/// </summary>
	/// <param name="kind"></param>
	/// <returns></returns>
	std::shared_ptr<AttackCol> GetAttackCol() const;

	/// <summary>
	/// Physicsから当たり判定登録を解除する
	/// </summary>
	void ReleasePhysics();

	/// <summary>
	/// 攻撃可否を返す
	/// </summary>
	/// <returns></returns>
	bool CanAttack() const;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	float GetAttackInterval() const;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	float GetStateTransitionTime() const;

	/// <summary>
	/// 向いている方へ指定の移動量加算する
	/// </summary>
	/// <param name="add"></param>
	void AddTransformForward(float add) const;
	/// <summary>
	/// 移動量をなくす
	/// </summary>
	/// <returns></returns>
	void Stop() const;

	/// <summary>
	/// <para> プレイヤーの方向を向く </para>
	/// <para> 速度制限なし </para>
	/// </summary>
	void RotateToPlayer() { GetParentPtr()->RotateToPlayer(); }
	/// <summary>
	/// <para> プレイヤーの方向を向く </para>
	/// <para> 速度制限あり </para>
	/// </summary>
	/// <param name="speed"></param>
	void RotateToPlayer(float speed) { GetParentPtr()->RotateToPlayer(speed); }
	/// <summary>
	/// プレイヤーが攻撃範囲にいるか
	/// </summary>
	/// <param name="attackRangeInPlayer"></param>
	/// <returns></returns>
	bool CanAttackRangeInPlayer(float attackRangeInPlayer);
	/// <summary>
	/// ステート変更待機時間がないなら
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
	/// <param name="time"></param>
	void SetStateTransitionTime(float time);
	/// <summary>
	/// 攻撃待機時間を設定
	/// </summary>
	/// <returns></returns>
	void SetAttackInterval(float interval);

protected:

	/// <summary>
	/// <para> ステートの遷移条件を確認する </para>
	/// <para> 変更可能なステートがあればそのポインタを返す </para>
	/// <para> 変更がなければnullptrを返す </para>
	/// </summary>
	virtual std::shared_ptr<EnemyWeakStateBase> CheckStateTransition() abstract;

	/// <summary>
	/// ポインタを返す
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<EnemyWeak> GetParentPtr() const { return _parent.lock(); }

	/// <summary>
	/// 接地しているか
	/// </summary>
	/// <returns>接地していればtrue</returns>
	bool IsGround();

private:
	// baseからのみアクセス可能
	std::weak_ptr<EnemyWeak> _parent;
};


#pragma once
#include "Player.h"
#include "PlayerAnimationData.h"
#include "PlayerComboHolder.h"
#include "Loader/PlayerDataLoader.h"

// 各ステートで使用したい定数
namespace {
	// 移動速度
	constexpr float kDashSpeed = 30.0f;
	// ダッシュから攻撃に遷移するときの前進量
	constexpr float kDashToAttackVel = 100.0f;
	// ジャンプ力
	constexpr float kJumpForce = 25.0f;
	// 空中移動速度
	constexpr float kAirMoveSpeed = kDashSpeed * 0.7f;
	// 回避時の移動速度
	constexpr float kDodgeSpeed = kDashSpeed * -0.5f;
	constexpr float kDodgeFailSpeed = kDodgeSpeed * 0.8f;
	// 
	constexpr float kCanInputProgress = 0.3f;			// 入力を受け付ける進行度
	constexpr float kCanInputProgressSpecialAttack = 0.6f;// 入力を受け付ける進行度(特殊攻撃)
	constexpr float kCanDodgeInputProgress = 0.9f;		// 回避入力を受け付ける進行度
	constexpr float kCanDodgeFailInputProgress = 0.95f;	// 回避失敗入力を受け付ける進行度
	constexpr int kMaxDodgeCount = 5;					// 回避可能回数
	// この時間が経つまでに回避が行われたら回避カウントを増やす
	constexpr float kDodgeInterval = 0.85f;
	// 攻撃時の移動
	constexpr float kEnterForwardVel = 20.0f;
	// ロックオン距離
	constexpr float kNormalAttackLockOnDist = 700.0f;	// 通常攻撃ロックオン距離
	constexpr float kSpecialAttackLockOnDist = 1500.0f;	// 特殊攻撃ロックオン距離
}

class JustDodgeManager;
class EnemyBase;

/// <summary>
/// プレイヤーステートの基底クラス
/// </summary>
class PlayerStateBase abstract
{
public:
	PlayerStateBase(std::weak_ptr<Player> player);
	virtual ~PlayerStateBase() = default;

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
	void ChangeState(std::shared_ptr<PlayerStateBase> nextState);

	/// <summary>
	/// <para> 入力を受け付けるか </para>
	/// <para> 現在のアニメーションに対して進行度が引数の割合より進んでいた場合true </para>
	/// <para> trueが帰ってきたらプレイヤーの入力を受け付ける想定 </para>
	/// </summary>
	/// <param name="canInputProgressRate">入力を受け付け始めるアニメーションの進行度</param>
	/// <returns></returns>
	virtual bool CanInputAllowance(float canInputProgressRate) const;

protected:

	// プレイヤーの関数を派生先で使えるようにしたもの
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
	const std::shared_ptr<AnimationModel>& GetAnimator() const;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<PlayerComboHolder> GetComboHolder() const;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<UIGameScoreDrawer> GetScoreDrawer() const;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<PlayerDataLoader> GetDataLoader() const;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<JustDodgeManager> GetJustDodgeManager() const;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<EnemyManager> GetEnemyManager() const;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Collider> GetCollider() const;

	/// <summary>
	/// <para> 正面に補正した行列を返す </para>
	/// <para> 厳密なモデルの行列ではない </para>
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetForwardMatrix() const;

	/// <summary>
	/// カメラを返す
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<Camera> GetCamera() const;
	/// <summary>
	/// カメラの軸回転情報を返す
	/// </summary>
	/// <returns></returns>
	Vector3 GetCameraRotAngle() const;

	/// <summary>
	/// 攻撃種別に応じた攻撃判定を返す
	/// </summary>
	/// <param name="kind"></param>
	/// <returns></returns>
	std::shared_ptr<AttackCol> GetAttackCol(PlayerActionKind kind) const;

	/// <summary>
	/// プレイヤーの向いている方へ指定の移動量加算する
	/// </summary>
	/// <param name="add"></param>
	void AddTransformForward(float add) const;
	/// <summary>
	/// 入力方向へ指定の移動量加算する
	/// </summary>
	/// <param name="add"></param>
	void AddTransformStick(float add) const;
	/// <summary>
	/// プレイヤーの上方へ指定の移動量加算する
	/// </summary>
	/// <param name="add"></param>
	void AddTransformUp(float add) const;
	/// <summary>
	/// プレイヤーの移動量をなくす
	/// </summary>
	/// <returns></returns>
	void Stop() const;

	/// <summary>
	/// 入力に応じてプレイヤーを回転させる
	/// </summary>
	void Rotate() const;
	/// <summary>
	/// <para> 指定の位置にプレイヤーを回転させる </para>
	/// <para> 回転速度制限なし </para>
	/// </summary>
	void RotateToPos(Position3 pos);

	/// <summary>
	/// 回避待機時間を取得
	/// </summary>
	/// <returns></returns>
	float GetDodgeInterval() const { return _player.lock()->_dodgeInterval; }
	/// <summary>
	/// 回避待機時間を設定
	/// </summary>
	/// <returns></returns>
	void SetDodgeInterval() { _player.lock()->_dodgeInterval = kDodgeInterval; }
	/// <summary>
	/// 回避回数を取得
	/// </summary>
	/// <returns></returns>
	int GetDodgeCount() const { return _player.lock()->_dodgeCount; }
	/// <summary>
	/// 回避回数を追加
	/// </summary>
	/// <returns></returns>
	void AddDodgeCount() { _player.lock()->_dodgeCount++; }
	/// <summary>
	/// 回避待機時間をリセット
	/// </summary>
	/// <returns></returns>
	void ResetDodgeInterval();
	/// <summary>
	/// 回避時のエフェクトを再生
	/// </summary>
	/// <returns></returns>
	void StartDodgeEffect();
	/// <summary>
	/// ロックオン持続時間を設定
	/// </summary>
	/// <param name="lockonDurationTime"></param>
	void SetLockonDurationTime(float lockonDurationTime) { _player.lock()->SetLockonDurationTime(lockonDurationTime); }

protected:

	/// <summary>
	/// <para> ステートの遷移条件を確認する </para>
	/// <para> 変更可能なステートがあればそのポインタを返す </para>
	/// <para> 変更がなければnullptrを返す </para>
	/// </summary>
	virtual std::shared_ptr<PlayerStateBase> CheckStateTransition() abstract;

	/// <summary>
	///	<para> コンボの成立状況を見て遷移すべき攻撃ステートを返す </para>
	/// <para> 何も満たしていなければnullptr </para>
	/// <para> 次の攻撃が存在するなら攻撃ステート </para>
	/// <para> 特殊攻撃が行えるなら特殊攻撃ステート </para>
	/// </summary>
	/// <param name="kind">どの攻撃を行ったか</param>
	/// <param name="data">自分のIDデータ</param>
	/// <returns>遷移すべきステート</returns>
	std::shared_ptr<PlayerStateBase> CheckComboTransition(PlayerActionKind kind, PlayerAttackAnimData data) const;
	/// <summary>
	///	<para> コンボの成立状況を見て遷移すべき空中攻撃ステートを返す </para>
	/// <para> 何も満たしていなければnullptr </para>
	/// <para> 次の攻撃が存在するなら空中攻撃ステート </para>
	/// <para> 特殊攻撃が行えるなら特殊攻撃ステート </para>
	/// </summary>
	/// <param name="kind">どの攻撃を行ったか</param>
	/// <param name="data">自分のIDデータ</param>
	/// <returns>遷移すべきステート</returns>
	std::shared_ptr<PlayerStateBase> CheckAirComboTransition(PlayerActionKind kind, PlayerAttackAnimData data) const;
	/// <summary>
	/// <para> 連続回避回数を見て成功したか判定する </para>
	/// <para> 指定回数以内ならtrue </para>
	/// <para> 指定回数以上回避しようとしていた場合はfalse </para>
	/// </summary>
	/// <returns></returns>
	bool IsSuccessDodge() const;

	/// <summary>
	/// プレイヤーのポインタを返す
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Player> GetPlayerPtr() const { return _player.lock()->GetPlayerPtr(); }

	/// <summary>
	/// 通常攻撃IDを取得
	/// </summary>
	/// <returns></returns>
	int GetPunchAttackID() const;
	/// <summary>
	/// 強攻撃IDを取得
	/// </summary>
	/// <returns></returns>
	int GetKickAttackID() const;

	/// <summary>
	/// <para> プレイヤーを指定方向分回転させる </para>
	/// <para> 回転速度制限なし </para>
	/// </summary>
	/// <param name="rad"></param>
	/// <returns></returns>
	void SetRotAngleY(float targetAngle) { _player.lock()->SetRotAngleY(targetAngle); }
	/// <summary>
	/// <para> プレイヤーを指定方向分回転させる </para>
	/// <para> 回転速度制限なし </para>
	/// </summary>
	/// <param name="rad"></param>
	/// <returns></returns>
	void AddRotAngleY(float targetAngle) { _player.lock()->AddRotAngleY(targetAngle); }
	/// <summary>
	/// <para> プレイヤーの向きを取得する </para>
	/// <para> 厳密なモデルの向きではない </para>
	/// </summary>
	/// <param name="rad"></param>
	/// <returns></returns>
	float GetRotAngleY() const { return _player.lock()->GetRotAngleY(); }

	/// <summary>
	/// 接地しているか
	/// </summary>
	/// <returns>接地していればtrue</returns>
	bool IsGround();
	/// <summary>
	/// 攻撃可能な入力であるか
	/// </summary>
	/// <returns>可能であればtrue</returns>
	bool CanAttackInput();
	/// <summary>
	/// 通常攻撃が可能な入力であるか
	/// </summary>
	/// <returns>可能であればtrue</returns>
	bool CanPunchAttackInput();
	/// <summary>
	/// 強攻撃が可能な入力であるか
	/// </summary>
	/// <returns>可能であればtrue</returns>
	bool CanKickAttackInput();
	/// <summary>
	/// 歩ける入力であるか
	/// </summary>
	/// <returns>スティック入力か移動キー入力があればtrue</returns>
	bool CanWalkInput();
	/// <summary>
	/// 走れる入力であるか
	/// </summary>
	/// <returns>走れる程のスティック入力か移動キー入力があればtrue</returns>
	bool CanDashInput();
	/// <summary>
	/// ジャンプ可能な入力であるか
	/// </summary>
	/// <returns>ジャンプ入力があればtrue</returns>
	bool CanJumpInput();
	/// <summary>
	/// 回避可能な入力であるか
	/// </summary>
	/// <returns>回避入力があればtrue</returns>
	bool CanDodgeInput();

	/// <summary>
	/// コンボリセットを行う
	/// </summary>
	void ResetCombo();

	/// <summary>
	/// <para> 指定範囲内に敵がいるか </para>
	/// <para> いれば有効な敵を返す </para>
	/// <para> いなければ不正な敵を返す </para>
	/// </summary>
	/// <param name="range"></param>
	/// <returns></returns>
	std::weak_ptr<EnemyBase> IsExistEnemyWithinRange(float range);

private:
	// baseからのみアクセス可能
	std::weak_ptr<Player> _player;
};


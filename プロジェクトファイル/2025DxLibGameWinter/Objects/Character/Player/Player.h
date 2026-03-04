#pragma once
#include "Library/Objects/Character/CharacterBase.h"
#include "Library/Geometry/Quaternion.h"
#include "Library/Geometry/Matrix4x4.h"
#include "PlayerComboHolder.h"
#include <DxLib.h>
#include <unordered_map>

class PlayerStateBase;
class Physics;
class Camera;
class EnemyManager;
class AttackableGameObject;
class PlayerDataLoader;
class PlayerComboHolder;
class UIPlayerHP;
class AttackCol;
class JustDodgeManager;
class UIGameScoreDrawer;
class EffekseerEffect;

/// <summary>
/// 
/// </summary>
class Player final : public CharacterBase
{
public:
	Player(std::weak_ptr<Physics> physics);
	~Player() = default;

	void Init() override;
	void Update() override;
	void Draw() const override;

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	/// <summary>
	/// ダメージを受ける処理
	/// </summary>
	/// <param name="attacker">攻撃を行った相手</param>
	void TakeDamage(std::shared_ptr<AttackableGameObject> attacker) override;
	
	/// <summary>
	/// プレイヤーに関係するUIを生成
	/// </summary>
	void GeneratePlayerUI();

	std::weak_ptr<PlayerComboHolder> GetComboHolder();
	void SetCamera(std::weak_ptr<Camera> camera) { _camera = camera; }
	void SetEnemyManager(std::weak_ptr<EnemyManager> manager) { _enemyManager = manager; }
	void SetJustDodgeManager(std::weak_ptr<JustDodgeManager> manager) { _justDodgeManager = manager; }
	void SetUIGameScoreDrawer(std::weak_ptr<UIGameScoreDrawer> scoreDrawer) { _scoreDrawer = scoreDrawer; }
	/// <summary>
	/// 最大体力値を返す
	/// </summary>
	/// <returns></returns>
	float GetMaxHitPoint() override;
	/// <summary>
	/// 体力値を返す
	/// </summary>
	/// <returns></returns>
	float GetHitPoint();
	/// <summary>
	/// 生存状態を返す
	/// </summary>
	/// <returns></returns>
	bool IsAlive() override;
	void SetAliveState(bool isAlive) { _isAlive = isAlive; }

private:

	/// <summary>
	/// 攻撃判定更新
	/// </summary>
	void UpdateAttackCol();
	/// <summary>
	/// 回避更新
	/// </summary>
	void UpdateDodge();

	// ステートが以下のメンバを変更可能にする
	friend PlayerStateBase;

	/// <summary>
	/// プレイヤーのポインタを返す
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Player> GetPlayerPtr() { return std::static_pointer_cast<Player>(shared_from_this()); }
	
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<Physics> GetPhysics() { return _physics; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<PlayerComboHolder> GetComboHolder() const { return _comboHolder; }
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<PlayerDataLoader> GetDataLoader() const { return _dataLoader; }
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<JustDodgeManager> GetJustDodgeManager() const { return _justDodgeManager; }
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<EnemyManager> GetEnemyManager() const { return _enemyManager; }
public:
	/// <summary>
	/// モデルの行列を返す
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetModelMatrix() const;
private:
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
	std::weak_ptr<Camera> GetCamera() const { return _camera; }
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
	std::shared_ptr<AttackCol> GetAttackCol(PlayerActionKind kind);

	/// <summary>
	/// 向いている方へ指定の移動量加算する
	/// </summary>
	/// <param name="add"></param>
	void AddTransformForward(float add);
	/// <summary>
	/// 入力方向へ指定の移動量加算する
	/// </summary>
	/// <param name="add"></param>
	void AddTransformStick(float add);
	/// <summary>
	/// 上方へ指定の移動量加算する
	/// </summary>
	/// <param name="add"></param>
	void AddTransformUp(float add);
	/// <summary>
	/// 移動量をなくす
	/// </summary>
	/// <returns></returns>
	void Stop() { _collider->SetVel(Vector3Zero()); }

	/// <summary>
	/// <para> プレイヤーを入力方向に回転させる </para>
	/// <para> モデルの回転速度制限あり </para>
	/// </summary>
	/// <returns></returns>
	void Rotate();
	/// <summary>
	/// <para> 指定方向に向ける </para>
	/// <para> 回転速度制限なし </para>
	/// </summary>
	/// <param name="angle"></param>
	/// <returns></returns>
	void SetRotAngleY(float angle);
	/// <summary>
	/// <para> 指定方向分回転させる </para>
	/// <para> 回転速度制限なし </para>
	/// </summary>
	/// <param name="rad"></param>
	/// <returns></returns>
	void AddRotAngleY(float rad);
	/// <summary>
	/// <para> 向きを取得する </para>
	/// <para> 厳密なモデルの向きではない </para>
	/// </summary>
	/// <returns></returns>
	float GetRotAngleY() const { return _rotAngleY; }
	/// <summary>
	/// ロックオン持続時間を設定
	/// </summary>
	/// <returns></returns>
	void SetLockonDurationTime(float lockonDurationTime) { _lockonDurationTime = lockonDurationTime; }

	/// <summary>
	/// 回避時のエフェクトを再生
	/// </summary>
	void StartDodgeEffect();

	// MEMO:AnimatorはCharacterBaseに置きたいが、そうするとStateから見えない
	std::shared_ptr<PlayerStateBase> _currentState;	// 現在のステート
	std::shared_ptr<AnimationModel> _animator;		// モデルとアニメーションを管理
	std::weak_ptr<Camera> _camera;					// カメラ
	std::weak_ptr<EnemyManager> _enemyManager;
	std::weak_ptr<JustDodgeManager> _justDodgeManager;	// ジャスト回避管理
	std::weak_ptr<UIGameScoreDrawer> _scoreDrawer;

	std::shared_ptr<PlayerDataLoader> _dataLoader;
	std::shared_ptr<PlayerComboHolder> _comboHolder;

	std::unordered_map<PlayerActionKind, std::shared_ptr<AttackCol>> _attackCol;

	// 回避残り時間表示エフェクト
	std::weak_ptr<EffekseerEffect> _dodgeLimitEffect;
	// 回避足元エフェクト
	std::weak_ptr<EffekseerEffect> _dodgeFootEffect;

	float _rotAngleY;
	float _modelRotAngleY;
	Quaternion _quaternion;
	float _lockonDurationTime;

	float _dodgeInterval;	// 回避待機時間(秒)
	int _dodgeCount;		// 回避回数
	bool _isAlive;
};


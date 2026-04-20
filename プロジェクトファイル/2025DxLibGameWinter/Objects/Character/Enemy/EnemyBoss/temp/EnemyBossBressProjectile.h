#pragma once
#include "Library/Objects/AttackableGameObject.h"
#include "Library/Geometry/Vector3.h"
#include <memory>

class Physics;
class Collider;

/// <summary>
/// ブレス攻撃の弾クラス
/// 球体の当たり判定を持ち、一定方向へ一定速度で飛翔する
/// 一定時間経過か床/壁との衝突で消滅する
/// </summary>
class EnemyBossBressProjectile : public AttackableGameObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="physics">物理システム</param>
	/// <param name="startPos">発射開始位置（頭部位置）</param>
	/// <param name="direction">飛翔方向（正規化済みベクトル）</param>
	/// <param name="ownerStatus">攻撃オーナー（EnemyBoss）</param>
	EnemyBossBressProjectile(
		std::weak_ptr<Physics> physics,
		const Position3& startPos,
		const Vector3& direction);

	~EnemyBossBressProjectile();

	/// <summary>
	/// 初期化（物理システムへの登録）
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// 毎フレーム飛翔方向へ移動し、寿命を減算する
	/// </summary>
	void Update();

	/// <summary>
	/// 衝突コールバック
	/// 床/壁（Stageタグ）またはプレイヤーに当たると消滅フラグを立てる
	/// </summary>
	/// <param name="collider">衝突相手のコライダー</param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	/// <summary>
	/// 削除可能かどうかを返す
	/// 寿命切れまたは衝突で消滅した場合にtrue
	/// </summary>
	bool CanDelete() const { return _canDelete; }

	/// <summary>
	/// 物理登録を解除する
	/// </summary>
	void ReleaseFromPhysics();

private:
	// 進行方向
	Vector3 _direction;
	// 削除フラグ
	bool _canDelete;
	// 残り寿命
	float _lifeTime;
};

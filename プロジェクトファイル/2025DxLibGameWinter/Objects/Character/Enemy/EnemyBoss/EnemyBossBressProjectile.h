#pragma once
#include "Library/Objects/AttackCol.h"
#include "Library/Geometry/Vector3.h"
#include <memory>

class Physics;
class Collider;
class EffekseerEffect;

/// <summary>
/// ブレス攻撃の弾クラス
/// 球体の当たり判定を持ち、一定方向へ一定速度で飛翔する
/// 一定時間経過か床/壁との衝突で消滅する
/// </summary>
class EnemyBossBressProjectile : public AttackCol
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="physics">Physics</param>
	/// <param name="startPos">発射開始位置（頭部位置）</param>
	/// <param name="direction">飛翔方向</param>
	EnemyBossBressProjectile(
		std::weak_ptr<Physics> physics,
		const Position3& startPos,
		const Vector3& direction);

	~EnemyBossBressProjectile();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const override;

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

	// 攻撃時に発生する追従させるエフェクト
	std::weak_ptr<EffekseerEffect> _currentEffect;
};

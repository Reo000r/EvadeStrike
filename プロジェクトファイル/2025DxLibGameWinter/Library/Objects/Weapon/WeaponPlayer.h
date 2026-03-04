#pragma once
#include "Weapon.h"

/// <summary>
/// プレイヤー属性を持った武器
/// </summary>
class WeaponPlayer final :  public Weapon
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WeaponPlayer(std::weak_ptr<Physics> physics);

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;
};


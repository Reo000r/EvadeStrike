#pragma once
#include "Weapon.h"

/// <summary>
/// 敵属性を持った武器
/// </summary>
class WeaponEnemy final : public Weapon
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WeaponEnemy(std::weak_ptr<Physics> physics);
};


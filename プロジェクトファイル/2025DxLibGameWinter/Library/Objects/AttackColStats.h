#pragma once
#include "Library/Geometry/Vector3.h"
#include "Objects/Character/Player/PlayerComboHolder.h"
#include <string>

struct AttackColStats {
	PlayerActionKind kind = PlayerActionKind::None;	// 攻撃種別

	float colRad = 0.0f;		// 半径
	float colHeight = 0.0f;		// startからend
	Vector3 transOffset = Vector3();	// 移動補正
	Vector3 scale = Vector3(1, 1, 1);	// 拡縮補正
	Vector3 angle = Vector3();			// 角度補正
	Vector3 knockbackVel = Vector3();	// ノックバック
	float attackPower = 0.0f;			// 攻撃力
	float breakPower = 0.0f;			// 削り値
};

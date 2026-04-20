#pragma once
#include <string>

namespace WeakAnimData {
	// アニメーション

	const std::wstring kBaseAnimName = L"Armature|";
	const std::wstring kAnimNameSpawn = kBaseAnimName + L"SpecialAttack2";
	const std::wstring kAnimNameIdle = kBaseAnimName + L"Idle";
	const std::wstring kAnimNameDash = kBaseAnimName + L"Dash";
	const std::wstring kAnimNameHeavyAttack1 = kBaseAnimName + L"HeavyAttack_1";
	const std::wstring kAnimNameReact = kBaseAnimName + L"React";
	const std::wstring kAnimNameDeath = kBaseAnimName + L"Dying";

	// アニメーションの速度
	constexpr float kBaseAnimSpeed = 1.0f;
	constexpr float kSpawnAnimSpeed = kBaseAnimSpeed * 1.5f;
	constexpr float kAttackAnimSpeed = kBaseAnimSpeed * 0.5f;
	constexpr float kDeathAnimSpeed = kBaseAnimSpeed * 2.0f;
}

namespace BossAnimData {
	// アニメーション

	const std::wstring kBaseAnimName = L"Armature|";
	const std::wstring kAnimNameSpawn = kBaseAnimName + L"SpecialAttack2";
	const std::wstring kAnimNameIdle = kBaseAnimName + L"Idle";
	const std::wstring kAnimNameDash = kBaseAnimName + L"Dash";
	//const std::wstring kAnimNameLightAttack = kBaseAnimName + L"LightAttack";
	//const std::wstring kAnimNameHeavyAttack = kBaseAnimName + L"HeavyAttack";
	//const std::wstring kAnimNameBressAttack = kBaseAnimName + L"BressAttack";
	const std::wstring kAnimNameLightAttack = kBaseAnimName + L"LightAttack_1";
	const std::wstring kAnimNameHeavyAttack = kBaseAnimName + L"LightAttack_3";
	const std::wstring kAnimNameBressAttack = kBaseAnimName + L"LightAttack_2";
	const std::wstring kAnimNameReact = kBaseAnimName + L"React";
	const std::wstring kAnimNameDeath = kBaseAnimName + L"Dying";

	// アニメーションの速度
	constexpr float kBaseAnimSpeed = 0.6f;
	constexpr float kSpawnAnimSpeed = kBaseAnimSpeed * 2.0f;
	constexpr float kAttackAnimSpeed = kBaseAnimSpeed * 1.0f;
	constexpr float kAttackLightAnimSpeed = kAttackAnimSpeed * 1.2f;
	constexpr float kAttackHeavyAnimSpeed = kAttackAnimSpeed * 0.7f;
	constexpr float kAttackBressAnimSpeed = kAttackAnimSpeed * 1.0f;
	constexpr float kReactAnimSpeed = kBaseAnimSpeed * 1.0f;
	constexpr float kDeathAnimSpeed = kBaseAnimSpeed * 1.0f;
}


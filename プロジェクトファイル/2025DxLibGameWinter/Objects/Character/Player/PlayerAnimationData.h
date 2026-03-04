#pragma once
#include <string>

namespace {
	// アニメーション

	//const std::wstring kBaseAnimName = L"Armature|Animation_";
	//const std::wstring kAnimNameIdle = kBaseAnimName + L"Idle";
	//const std::wstring kAnimNameWalk = kBaseAnimName + L"Walk";
	//const std::wstring kAnimNameDash = kBaseAnimName + L"Run";
	//const std::wstring kAnimNameJump = kBaseAnimName + L"Jump";
	//const std::wstring kAnimNameFall = kBaseAnimName + L"Fall";
	//const std::wstring kAnimNameAttack1 = kBaseAnimName + L"AttackCombo1";
	//const std::wstring kAnimNameAttack2 = kBaseAnimName + L"AttackCombo2";
	//const std::wstring kAnimNameAttack3 = kBaseAnimName + L"AttackCombo3";
	//const std::wstring kAnimNameAttack4 = kBaseAnimName + L"AttackCombo4";
	//const std::wstring kAnimNameAttack5 = kBaseAnimName + L"AttackCombo5";
	//const std::wstring kAnimNameAirAttack = kBaseAnimName + L"AirAttack";
	//const std::wstring kAnimNameReact = kBaseAnimName + L"BlockReact";
	//const std::wstring kAnimNameDeath = kBaseAnimName + L"Dying";

	const std::wstring kBaseAnimName = L"Armature|";
	const std::wstring kAnimNameIdle = kBaseAnimName + L"Idle";
	const std::wstring kAnimNameDash = kBaseAnimName + L"Dash";
	const std::wstring kAnimNameJump = kBaseAnimName + L"JumpingUp";
	const std::wstring kAnimNameFalling = kBaseAnimName + L"Falling";
	const std::wstring kAnimNameLanding = kBaseAnimName + L"FallingToLanding";
//	const std::wstring kAnimNameGettingUp = kBaseAnimName + L"GettingUp";
	const std::wstring kAnimNameLightAttack1 = kBaseAnimName + L"LightAttack_1";
	const std::wstring kAnimNameLightAttack2 = kBaseAnimName + L"LightAttack_2";
	const std::wstring kAnimNameLightAttack3 = kBaseAnimName + L"LightAttack_3";
	const std::wstring kAnimNameLightAttack4 = kBaseAnimName + L"LightAttack_4";
	const std::wstring kAnimNameHeavyAttack1 = kBaseAnimName + L"HeavyAttack_1";
	const std::wstring kAnimNameHeavyAttack2 = kBaseAnimName + L"HeavyAttack_2";
	const std::wstring kAnimNameHeavyAttack3 = kBaseAnimName + L"HeavyAttack_3";
	const std::wstring kAnimNameHeavyAttack4 = kBaseAnimName + L"HeavyAttack_4";
	const std::wstring kAnimNameSpecialAttack1 = kBaseAnimName + L"SpecialAttack1";
	const std::wstring kAnimNameSpecialAttack2 = kBaseAnimName + L"SpecialAttack2";
	const std::wstring kAnimNameDodge = kBaseAnimName + L"Dodge";
	const std::wstring kAnimNameDodgeFail = kBaseAnimName + L"DodgeFailure";
	const std::wstring kAnimNameReact = kBaseAnimName + L"React";
	const std::wstring kAnimNameDeath = kBaseAnimName + L"Dying";

	// アニメーションの速度
	constexpr float kBaseAnimSpeed = 1.0f;
	constexpr float kJumpingAnimSpeed = kBaseAnimSpeed * 1.0f;
	constexpr float kLandingAnimSpeed = kBaseAnimSpeed * 1.7f;
	constexpr float kDodgeAnimSpeed = kBaseAnimSpeed * 2.5f;
	constexpr float kDodgeFailAnimSpeed = kDodgeAnimSpeed * 0.5f;
	constexpr float kReactAnimSpeed = kBaseAnimSpeed * 1.5f;
}
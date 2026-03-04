#pragma once
#include <string>

struct PlayerAttackAnimData {
	int id = -1;			// このアニメーションのID
	int defaultId = -1;		// デフォルトの遷移先のID
	int punchId = -1;		// パンチ入力時のID
	int kickId = -1;		// キック入力時のID
	float startAttackColActiveFrameRate = 0.0f;	// 攻撃判定を出し始めるアニメーション進行度
	float endAttackColActiveFrameRate = 0.0f;	// 攻撃判定を無くすアニメーション進行度
	float startTransformFrameRate = 0.0f;		// 攻撃移動をし始めるアニメーション進行度
	float endTransformFrameRate = 0.0f;			// 攻撃移動を終了するアニメーション進行度
	float animSpeed = 1.0f;						// アニメーション速度
	std::wstring animName = L"";		//	このアニメーションの名前

	/// <summary>
	/// 攻撃判定を出して良いタイミングならtrue
	/// </summary>
	/// <param name="currentFrameRate">アニメーション進行度</param>
	/// <returns></returns>
	bool CanAttackColActive(float currentFrameRate) {
		return (currentFrameRate > startAttackColActiveFrameRate &&
			currentFrameRate < endAttackColActiveFrameRate);
	}
	/// <summary>
	/// 攻撃時の移動をして良いタイミングならtrue
	/// </summary>
	/// <param name="currentFrameRate">アニメーション進行度</param>
	/// <returns></returns>
	bool CanAttackTransform(float currentFrameRate) {
		return (currentFrameRate > startTransformFrameRate &&
			currentFrameRate < endTransformFrameRate);
	}
};
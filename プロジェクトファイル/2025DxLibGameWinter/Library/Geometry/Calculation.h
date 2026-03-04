#pragma once
#include <cmath>

namespace Calculation {
	inline constexpr float kPi = 3.1415926535897932384626433832795028f;

	/// <summary>
	/// 弧度法(ラジアン)へ変換
	/// </summary>
	/// <param name="degree">度数法</param>
	/// <returns>弧度法</returns>
	inline float ToRadian(float degree) {
		return (kPi / 180.0f) * degree;
	}
	/// <summary>
	/// 度数法(デグリー)へ変換
	/// </summary>
	/// <param name="radian">弧度法</param>
	/// <returns>度数法</returns>
	inline float ToDegree(float radian) {
		return (180.0f / kPi) * radian;
	}

	/// <summary>
	/// 角度を -Pi から +Pi の範囲に正規化
	/// </summary>
	/// <param name="radian"></param>
	/// <returns></returns>
	inline float RadianNormalize(float radian) {
		// 角度を +Pi することで、剰余演算の結果を
		// 0 から 2Pi の範囲に調整しやすくする
		float remainder = fmodf(radian + kPi, 2.0f * kPi);
		// 剰余が負の場合は 2Pi を足して正の範囲に収める
		if (remainder < 0.0f) {
			remainder += 2.0f * kPi;
		}
		// 最後に Pi を引くことで、-Pi から Pi の範囲に正規化する
		return remainder - kPi;
	}

	/// <summary>
	/// 線形補間を行う
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	inline float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}
}

// ほかのクラスからCalcという名前で使用できる
// (Calculationと呼び出しごとに打つのは冗長な為)
namespace Calc = Calculation;
#pragma once
#include <cmath>

namespace {
	constexpr float kPi = 3.141592653589793238;
}

enum class EasingType {
	EaseInSine,
	EaseOutSine,
	EaseInOutSine,

	EaseInQuad,
	EaseOutQuad,
	EaseInOutQuad,

	EaseInCubic,
	EaseOutCubic,
	EaseInOutCubic,

	easeInQuart,
	easeOutQuart,
	easeInOutQuart,

	EaseInQuint,
	EaseOutQuint,
	EaseInOutQuint,

	EaseInExpo,
	EaseOutExpo,
	EaseInOutExpo,

	EaseInCirc,
	EaseOutCirc,
	EaseInOutCirc,

	EaseInBack,
	EaseOutBack,
	EaseInOutBack,

	EaseInElastic,
	EaseOutElastic,
	EaseInOutElastic,

	EaseInBounce,
	EaseOutBounce,
	EaseInOutBounce,
};

/// <summary>
/// 参考 : https://easings.net/ja
/// </summary>
class Easing {
public:
	/// <summary>
	/// <para> 進行度を対応したイージングを掛けて返す </para>
	/// </summary>
	/// <param name="t">進行度(0~1)</param>
	/// <param name="type">イージングタイプ</param>
	/// <returns></returns>
	static float Get(float t, EasingType type) {

		switch (type) {
		case EasingType::EaseInSine:
			return 1 - cos((t * kPi) / 2);
		case EasingType::EaseOutSine:
			return sin((t * kPi) / 2);
		case EasingType::EaseInOutSine:
			return -(cos(kPi * t) - 1) / 2;

		case EasingType::EaseInQuad:
			return t * t;
		case EasingType::EaseOutQuad:
			return 1 - (1 - t) * (1 - t);
		case EasingType::EaseInOutQuad:
			return (t < 0.5f) ? (2 * t * t) : (1 - pow(-2 * t + 2, 2) / 2);

		case EasingType::EaseInCubic:
			return t * t * t;
		case EasingType::EaseOutCubic:
			return 1 - pow(1 - t, 3);
		case EasingType::EaseInOutCubic:
			return (t < 0.5f) ? (4 * t * t * t) : (1 - pow(-2 * t + 2, 3) / 2);

		case EasingType::easeInQuart:
			return t * t * t * t;
		case EasingType::easeOutQuart:
			return 1 - pow(1 - t, 4);
		case EasingType::easeInOutQuart:
			return (t < 0.5f) ? (8 * t * t * t * t) : (1 - pow(-2 * t + 2, 4) / 2);

		case EasingType::EaseInQuint:
			return t * t * t * t * t;
		case EasingType::EaseOutQuint:
			return 1 - pow(1 - t, 5);
		case EasingType::EaseInOutQuint:
			return (t < 0.5f) ? (16 * t * t * t * t * t) : (1 - pow(-2 * t + 2, 5) / 2);

		case EasingType::EaseInExpo:
			return (t == 0.0f) ? 0 : (pow(2, 10 * t - 10));
		case EasingType::EaseOutExpo:
			return (t == 1.0f) ? 1 : (1 - pow(2, -10 * t));
		case EasingType::EaseInOutExpo:
			return t == 0
				? 0
				: t == 1
				? 1
				: t < 0.5f 
				? pow(2, 20 * t - 10) / 2
				: (2 - pow(2, -20 * t + 10)) / 2;

		case EasingType::EaseInCirc:
			return 1 - sqrt(1 - pow(t, 2));
		case EasingType::EaseOutCirc:
			return sqrt(1 - pow(t - 1, 2));
		case EasingType::EaseInOutCirc:
			return t < 0.5f
				? (1 - sqrt(1 - pow(2 * t, 2))) / 2
				: (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;

		case EasingType::EaseInBack: {
			const float c1 = 1.70158f;
			const float c3 = c1 + 1;
			return c3 * t * t * t - c1 * t * t;
		}
		case EasingType::EaseOutBack: {
			const float c1 = 1.70158f;
			const float c3 = c1 + 1;
			return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);
		}
		case EasingType::EaseInOutBack: {
			const float c1 = 1.70158f;
			const float c2 = c1 * 1.525f;
			return t < 0.5
				? (pow(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
				: (pow(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
		}

		case EasingType::EaseInElastic: {
			const float c4 = (2 * kPi) / 3;
			return t == 0
				? 0
				: t == 1
				? 1
				: -pow(2, 10 * t - 10) * sin((t * 10 - 10.75f) * c4);
		}
		case EasingType::EaseOutElastic: {
			const float c4 = (2 * kPi) / 3;
			return t == 0
				? 0
				: t == 1
				? 1
				: pow(2, -10 * t) * sin((t * 10 - 0.75f) * c4) + 1;
		}
		case EasingType::EaseInOutElastic: {
			const float c5 = (2 * kPi) / 4.5f;
			return t == 0
				? 0
				: t == 1
				? 1
				: t < 0.5f
				? -(pow(2, 20 * t - 10) * sin((20 * t - 11.125f) * c5)) / 2
				: (pow(2, -20 * t + 10) * sin((20 * t - 11.125f) * c5)) / 2 + 1;
		}

		case EasingType::EaseInBounce:
			return 1 - Get(1 - t, EasingType::EaseOutBounce);
		case EasingType::EaseOutBounce: {
			const float n1 = 7.5625f;
			const float d1 = 2.75f;
			if (t < 1 / d1) {
				return n1 * t * t;
			}
			else if (t < 2 / d1) {
				return n1 * (t -= 1.5 / d1) * t + 0.75;
			}
			else if (t < 2.5 / d1) {
				return n1 * (t -= 2.25 / d1) * t + 0.9375;
			}
			else {
				return n1 * (t -= 2.625 / d1) * t + 0.984375;
			}
		}
		case EasingType::EaseInOutBounce:
			return t < 0.5f
				? (1 - Get(1 - 2 * t, EasingType::EaseOutBounce)) / 2
				: (1 + Get(2 * t - 1, EasingType::EaseOutBounce)) / 2;
		default:
			return 0.0f;
		}
		return 0.0f;
	}

};
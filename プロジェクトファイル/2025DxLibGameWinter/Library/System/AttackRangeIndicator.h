#pragma once
#include "Library/Geometry/Vector3.h"

/// <summary>
/// 攻撃範囲インジケーターの形状種別
/// </summary>
enum class AttackRangeShape {
	None,
	Circle,
	Rect,
};

/// <summary>
/// <para> 敵の攻撃範囲を床に赤く表示するためのインジケーター </para>
/// <para> 円形と長方形 </para>
/// </summary>
class AttackRangeIndicator
{
public:
	AttackRangeIndicator();

	/// <summary>
	/// 円形として初期化する
	/// </summary>
	/// <param name="center">中心座標</param>
	/// <param name="radius">半径</param>
	void InitAsCircle(const Position3& center, float radius);

	/// <summary>
	/// 長方形として初期化する
	/// </summary>
	/// <param name="origin">手前側中央の座標</param>
	/// <param name="width">横幅</param>
	/// <param name="length">奥行き</param>
	/// <param name="rotY">回転角</param>
	void InitAsRect(const Position3& origin, float width, float length, float rotY);

	/// <summary>
	/// 長方形の原点と回転を更新する </para>
	/// </summary>
	void UpdateRectTransform(const Position3& origin, float rotY);

	/// <summary>
	/// アニメーション進行度に応じて不透明度を更新する
	/// </summary>
	void UpdateAlphaByRate(float currentRate, float startRate, float endRate);

	/// <summary>
	/// 経過時間に応じて不透明度を更新する
	/// </summary>
	void UpdateAlphaByTime(float elapsedTime, float fadeDuration);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// 表示を無効化する
	/// </summary>
	void Reset();

private:
	void DrawCircle() const;
	void DrawRect() const;

	AttackRangeShape _shape;

	Position3 _center;   // 円であれば中心、矩形であれば原点
	float _radius;
	float _width;
	float _length;
	float _rotY;

	int _currentAlpha;
};
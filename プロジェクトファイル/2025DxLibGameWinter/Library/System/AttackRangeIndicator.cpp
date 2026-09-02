#include "AttackRangeIndicator.h"
#include "Library/Geometry/Calculation.h"
#include <DxLib.h>
#include <algorithm>
#include <cmath>

namespace {
	// 地面から浮かせる量
	// Zファイティングを防ぐため
	constexpr float kUpperY = -100.0f;
	// 円の分割数
	constexpr int kCircleDivNum = 24;

	// 不透明度の最大値
	constexpr int kAttackRangeIndicatorMaxAlpha = 150;
}

AttackRangeIndicator::AttackRangeIndicator() :
	_shape(AttackRangeShape::None),
	_center(),
	_radius(0.0f),
	_width(0.0f),
	_length(0.0f),
	_rotY(0.0f),
	_currentAlpha(0)
{
}

void AttackRangeIndicator::InitAsCircle(const Position3& center, float radius)
{
	_shape = AttackRangeShape::Circle;
	_center = center;
	_radius = radius;
	_currentAlpha = 0;
}

void AttackRangeIndicator::InitAsRect(const Position3& origin, float width, float length, float rotY)
{
	_shape = AttackRangeShape::Rect;
	_center = origin;
	_width = width;
	_length = length;
	_rotY = rotY;
	_currentAlpha = 0;
}

void AttackRangeIndicator::UpdateRectTransform(const Position3& origin, float rotY)
{
	if (_shape != AttackRangeShape::Rect) return;

	_center = origin;
	_rotY = rotY;
}

void AttackRangeIndicator::UpdateAlphaByRate(float currentRate, float startRate, float endRate)
{
	if (_shape == AttackRangeShape::None) return;

	float rate = 0.0f;
	if (endRate > startRate) {
		rate = (currentRate - startRate) / (endRate - startRate);
	}
	rate = std::clamp(rate, 0.0f, 1.0f);

	_currentAlpha = static_cast<int>(kAttackRangeIndicatorMaxAlpha * rate);
}

void AttackRangeIndicator::UpdateAlphaByTime(float elapsedTime, float fadeDuration)
{
	if (_shape == AttackRangeShape::None) return;

	if (fadeDuration <= 0.0f) {
		_currentAlpha = kAttackRangeIndicatorMaxAlpha;
		return;
	}

	float rate = std::clamp(elapsedTime / fadeDuration, 0.0f, 1.0f);
	_currentAlpha = static_cast<int>(kAttackRangeIndicatorMaxAlpha * rate);
}

void AttackRangeIndicator::Reset()
{
	_shape = AttackRangeShape::None;
	_currentAlpha = 0;
}

void AttackRangeIndicator::Draw() const
{
	if (_shape == AttackRangeShape::None) return;
	if (_currentAlpha <= 0) return;

	// 裏面カリングとライティングを無効化
	SetUseLighting(FALSE);
	SetUseBackCulling(FALSE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, _currentAlpha);

	switch (_shape) {
	case AttackRangeShape::Circle:
		DrawCircle();
		break;
	case AttackRangeShape::Rect:
		DrawRect();
		break;
	default:
		break;
	}

	// 元の状態に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetUseBackCulling(TRUE);
	SetUseLighting(TRUE);
}

void AttackRangeIndicator::DrawCircle() const
{
	Position3 base = Position3(_center.x, _center.y + kUpperY, _center.z);

	VERTEX3D vertex[kCircleDivNum * 3];

	for (int i = 0; i < kCircleDivNum; ++i) {
		float angle0 = 2.0f * Calculation::kPi * i / kCircleDivNum;
		float angle1 = 2.0f * Calculation::kPi * (i + 1) / kCircleDivNum;

		Position3 p1 = base + Vector3(cosf(angle0) * _radius, 0.0f, sinf(angle0) * _radius);
		Position3 p2 = base + Vector3(cosf(angle1) * _radius, 0.0f, sinf(angle1) * _radius);

		vertex[i * 3 + 0].pos = base; // 中心
		vertex[i * 3 + 1].pos = p1;
		vertex[i * 3 + 2].pos = p2;
	}



	for (auto& v : vertex) {
		v.norm = Vector3(0, 1, 0);
		v.dif = GetColorU8(255, 0, 0, 255);
		v.spc = GetColorU8(0, 0, 0, 0);
		v.u = 0.0f; v.v = 0.0f; v.su = 0.0f; v.sv = 0.0f;
	}

	DrawPolygon3D(vertex, kCircleDivNum, DX_NONE_GRAPH, TRUE);
}

void AttackRangeIndicator::DrawRect() const
{
	float halfW = _width * 0.5f;

	// originを基準としたローカル座標
	Vector3 local[4] = {
		Vector3(-halfW, 0.0f, 0.0f),
		Vector3(halfW, 0.0f, 0.0f),
		Vector3(-halfW, 0.0f, _length),
		Vector3(halfW, 0.0f, _length),
	};

	// Y軸回転
	float s = sinf(_rotY);
	float c = cosf(_rotY);

	Position3 world[4];
	for (int i = 0; i < 4; ++i) {
		float rx = local[i].x * c + local[i].z * s;
		float rz = -local[i].x * s + local[i].z * c;
		world[i] = Position3(
			_center.x + rx,
			_center.y + kUpperY,
			_center.z + rz);
	}

	VERTEX3D vertex[6];
	vertex[0].pos = world[0];
	vertex[1].pos = world[1];
	vertex[2].pos = world[2];
	vertex[3].pos = world[1];
	vertex[4].pos = world[3];
	vertex[5].pos = world[2];

	for (auto& v : vertex) {
		v.norm = Vector3(0, 1, 0);
		v.dif = GetColorU8(255, 0, 0, 255);
		v.spc = GetColorU8(0, 0, 0, 0);
		v.u = 0.0f; v.v = 0.0f; v.su = 0.0f; v.sv = 0.0f;
	}

	DrawPolygon3D(vertex, 2, DX_NONE_GRAPH, TRUE);
}
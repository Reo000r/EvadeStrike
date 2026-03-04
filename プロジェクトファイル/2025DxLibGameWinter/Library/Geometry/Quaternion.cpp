#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix4x4.h"

#include <cmath>
#include <cassert>

namespace {
	constexpr float kPI = 3.141592653589793238f;
}

Quaternion::Quaternion() :
	w(0),
	x(0),
	y(0),
	z(0)
{
}

Quaternion::Quaternion(float _w, float _x, float _y, float _z) :
	w(_w),
	x(_x),
	y(_y),
	z(_z)
{
}

Quaternion Quaternion::operator*(const float& s) const
{
    return Quaternion(w*s, x*s, y*s, z*s);
}

void Quaternion::operator*=(const float& s)
{
    *this = (*this) * s;
}

bool Quaternion::operator==(const Quaternion& q) const
{
    return (
        w == q.w && 
        x == q.x && 
        y == q.y && 
        z == q.z);
}

void Quaternion::Normalized()
{
    *this = Normalize();
}

Quaternion Quaternion::Normalize() const
{
    float mag = std::sqrt(w * w + x * x + y * y + z * z);
    if (mag == 0.0f) return *this;  //0除算回避
    return Quaternion(w / mag, x / mag, y / mag, z / mag);
}

Quaternion Quaternion::Inverse() const
{
    return Quaternion(w, -x, -y, -z);
}

Quaternion LookRotation(const Vector3& dir, const Vector3& up)
{
    // 方向ベクトルがゼロに近い場合はデフォルトを返す
    if (dir.SqrMagnitude() < 0.0001f) return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

    // 各軸のベクトルを算出
    // ワールド回転行列を作る
    Vector3 forward = dir.Normalize();
    Vector3 right = Cross(up, forward).Normalize();

    // forwardとrightが平行な場合(真上/真下を向く時)のケア
    if (right.SqrMagnitude() < 0.0001f) {
        right = Cross(Vector3(0, 0, 1), forward).Normalize();
    }
    Vector3 newUp = Cross(forward, right);

    // 回転行列を作成
    Matrix4x4 mat = MatIdentity();
    mat.m[0][0] = right.x;   mat.m[0][1] = right.y;   mat.m[0][2] = right.z;
    mat.m[1][0] = newUp.x;   mat.m[1][1] = newUp.y;   mat.m[1][2] = newUp.z;
    mat.m[2][0] = forward.x; mat.m[2][1] = forward.y; mat.m[2][2] = forward.z;

    // 行列からクォータニオンに変換し返す
    return ConvMatrix4x4ToQuaternion(mat);
}

Quaternion operator*(const Quaternion lQ, const Quaternion rQ)
{
	Quaternion ret;

	// クォータニオン同士の計算
	ret.w = lQ.w * rQ.w - lQ.x * rQ.x - lQ.y * rQ.y - lQ.z * rQ.z;
	ret.x = lQ.w * rQ.x + lQ.x * rQ.w + lQ.y * rQ.z - lQ.z * rQ.y;
	ret.y = lQ.w * rQ.y + lQ.y * rQ.w + lQ.z * rQ.x - lQ.x * rQ.z;
	ret.z = lQ.w * rQ.z + lQ.z * rQ.w + lQ.x * rQ.y - lQ.y * rQ.x;

	return ret;
}

Vector3 operator*(const Quaternion qRot, const Vector3 right)
{
    return RotateVector3(qRot, right);
}

Vector3 RotateVector3(const Quaternion qRot, const Vector3 right)
{
    Quaternion qPos, qInv;

    Vector3 vPos;

    // 三次元座標をクォータニオンに変換
    qPos.w = 0.0f;
    qPos.x = right.x;
    qPos.y = right.y;
    qPos.z = right.z;

    // インバース
    qInv = qRot.Inverse();

    // 回転後のクォータニオンを作成
    qPos = qRot * qPos * qInv;

    // 三次元座標に戻す
    vPos.x = qPos.x;
    vPos.y = qPos.y;
    vPos.z = qPos.z;

    return vPos;
}

Quaternion AngleAxis(const Vector3& axis, const float& angle)
{
	// 正規化
	Vector3 ax = axis.Normalize();
	Quaternion q;
	// 軸ベクトルをクォータニオンに当てる
	q.w = cos(angle * 0.5f);
	q.x = ax.x * sin(angle * 0.5f);
	q.y = ax.y * sin(angle * 0.5f);
	q.z = ax.z * sin(angle * 0.5f);

    // 正規化
    q.Normalized();

	return q;
}

Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t)
{
    // tが0-1ではないなら相応しい値を返す
    if (t < 0.0f) return a; // 小さい場合
    if (t > 1.0f) return b; // 大きい場合

    // クォータニオンの内積を計算
    float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
    Quaternion end = b;
    // 内積が負の場合、bを反転して最短経路で補間する
    if (dot < 0.0f) {
        dot = -dot;
        end = Quaternion(-b.w, -b.x, -b.y, -b.z);
    }
    // クォータニオンがほぼ同じ場合は線形補間(Lerp)で近似
    if (dot > 0.9995f) {
        return Quaternion(
            a.w + t * (end.w - a.w),
            a.x + t * (end.x - a.x),
            a.y + t * (end.y - a.y),
            a.z + t * (end.z - a.z)
        ).Normalize();
    }
    // 球面線形補間（Slerp）の計算
    float theta0 = std::acos(dot);  // 2つのクォータニオン間の角度
    float theta = theta0 * t;       // tに応じた角度
    float sinTheta = std::sin(theta);
    float sinTheta0 = std::sin(theta0);

    float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;    // aの係数
    float s1 = sinTheta / sinTheta0;                            // bの係数

    // 補間結果を正規化して返す
    return Quaternion(
        a.w * s0 + end.w * s1,
        a.x * s0 + end.x * s1,
        a.y * s0 + end.y * s1,
        a.z * s0 + end.z * s1
    ).Normalize();
}

Quaternion ConvMatrix4x4ToQuaternion(const Matrix4x4 mat)
{
    // 各成分ごとにクォータニオンのx, y, z, wを求めるための値を計算
    float elem[4];  // 0:x, 1:y, 2:z, 3:w
    elem[0] =  mat.m[0][0] - mat.m[1][1] - mat.m[2][2] + 1.0f;
    elem[1] = -mat.m[0][0] + mat.m[1][1] - mat.m[2][2] + 1.0f;
    elem[2] = -mat.m[0][0] - mat.m[1][1] + mat.m[2][2] + 1.0f;
    elem[3] =  mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f;

    // 数値誤差対策のため、最大値のインデックスを検索
    unsigned int biggestIndex = 0;
    for (int i = 1; i < 4; ++i) {
        if (elem[i] > elem[biggestIndex]) {
            biggestIndex = i;
        }
    }

    // 最大値が0未満の場合は異常な行列なのでassert
    if (elem[biggestIndex] < 0.0f) {
        assert(false && "引数の行列に間違いがある");
        return Quaternion();
    }

    // 最大要素からクォータニオンの値を計算
    Quaternion ret;
    float v = sqrtf(elem[biggestIndex]) * 0.5f; // 最大成分の値
    float mult = 0.25f / v;                     // 他成分計算用の係数

    // 最大成分に応じて各値を計算
    switch (biggestIndex) {
    case 0: // x
        ret.x = v;
        ret.y = (mat.m[0][1] + mat.m[1][0]) * mult;
        ret.z = (mat.m[2][0] + mat.m[0][2]) * mult;
        ret.w = (mat.m[1][2] - mat.m[2][1]) * mult;
        break;

    case 1: // y
        ret.x = (mat.m[0][1] + mat.m[1][0]) * mult;
        ret.y = v;
        ret.z = (mat.m[1][2] + mat.m[2][1]) * mult;
        ret.w = (mat.m[2][0] - mat.m[0][2]) * mult;
        break;

    case 2: // z
        ret.x = (mat.m[2][0] + mat.m[0][2]) * mult;
        ret.y = (mat.m[1][2] + mat.m[2][1]) * mult;
        ret.z = v;
        ret.w = (mat.m[0][1] - mat.m[1][0]) * mult;
        break;

    case 3: // w
        ret.x = (mat.m[1][2] - mat.m[2][1]) * mult;
        ret.y = (mat.m[2][0] - mat.m[0][2]) * mult;
        ret.z = (mat.m[0][1] - mat.m[1][0]) * mult;
        ret.w = v;
        break;
    }

    // 正規化
    ret.Normalized();

    return ret;
}

Matrix4x4 ConvQuaternionToMatrix4x4(const Quaternion q)
{
    Matrix4x4 ret = MatIdentity();

    // 回転行列成分を設定
    ret.m[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
    ret.m[0][1] =        2.0f * q.x * q.y + 2.0f * q.w * q.z;
    ret.m[0][2] =        2.0f * q.x * q.z - 2.0f * q.w * q.y;

    ret.m[1][0] =        2.0f * q.x * q.y - 2.0f * q.w * q.z;
    ret.m[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
    ret.m[1][2] =        2.0f * q.y * q.z + 2.0f * q.w * q.x;

    ret.m[2][0] =        2.0f * q.x * q.z + 2.0f * q.w * q.y;
    ret.m[2][1] =        2.0f * q.y * q.z - 2.0f * q.w * q.x;
    ret.m[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

    return ret;
}

Vector3 ConvQuaternionToEuler(const Quaternion& q)
{
    float qw = q.w, qx = q.x, qy = q.y, qz = q.z;
    float sqw = qw * qw;
    float sqx = qx * qx;
    float sqy = qy * qy;
    float sqz = qz * qz;

    // ピッチ（X軸回転）
    float sinp = 2.0f * (qw * qx + qy * qz);
    float cosp = 1.0f - 2.0f * (sqx + sqy);
    float pitch = std::atan2(sinp, cosp);

    // ヨー（Y軸回転）
    float siny = 2.0f * (qw * qy - qz * qx);
    // 範囲外対策
    if      (siny >  1.0f)  siny =  1.0f;
    else if (siny < -1.0f)  siny = -1.0f;
    float yaw = std::asin(siny);

    // ロール（Z軸回転）
    float sinr = 2.0f * (qw * qz + qx * qy);
    float cosr = 1.0f - 2.0f * (sqy + sqz);
    float roll = std::atan2(sinr, cosr);

    return Vector3(pitch, yaw, roll);
}

Quaternion ConvEulerToQuaternion(const Vector3& euler)
{
    // オイラー角（ラジアン）
    float pitch = euler.x * 0.5f;   // X軸
    float yaw = euler.y * 0.5f;     // Y軸
    float roll = euler.z * 0.5f;    // Z軸

    float sinPitch = std::sin(pitch);
    float cosPitch = std::cos(pitch);
    float sinYaw = std::sin(yaw);
    float cosYaw = std::cos(yaw);
    float sinRoll = std::sin(roll);
    float cosRoll = std::cos(roll);

    Quaternion q;

    // Yaw-Pitch-Roll(Y→X→Z)の順で合成
    q.w = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;
    q.x = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
    q.y = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;
    q.z = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;

    q.Normalized();

    return q;
}

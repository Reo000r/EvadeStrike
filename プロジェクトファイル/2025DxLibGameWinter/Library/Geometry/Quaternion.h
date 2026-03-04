#pragma once

class Vector3;
class Matrix4x4;

/// <summary>
/// クォータニオン(四元数)
/// </summary>
class Quaternion final {
public:
	float w, x, y, z;

	Quaternion();
	/// <summary>
	/// 順番注意
	/// </summary>
	Quaternion(float _w, float _x, float _y, float _z);

	Quaternion operator*(const float& s) const;	// 回転合成
	void operator*=(const float& s);			// 回転合成
	bool operator==(const Quaternion& q) const;	// 等価比較

	/// <summary>
	/// 正規化する
	/// </summary>
	void Normalized();
	/// <summary>
	/// 正規化した値を返す
	/// </summary>
	Quaternion Normalize() const;

	/// <summary>
	/// 逆四元数(インバース)を返す
	/// </summary>
	Quaternion Inverse() const;
};

/// <summary>
/// 指定した方向を向くクォータニオンを返す
/// </summary>
/// <param name="dir"></param>
/// <param name="up"></param>
/// <returns></returns>
Quaternion LookRotation(const Vector3& dir, const Vector3& up);

/// <summary>
/// クォータニオン同士の回転合成
/// </summary>
/// <param name="lQ"></param>
/// <param name="rQ"></param>
/// <returns></returns>
Quaternion operator*(const Quaternion lQ, const Quaternion rQ);

/// <summary>
/// ベクトルに対しクォータニオンの回転を適用する
/// 演算子オーバーロード
/// </summary>
/// <param name="qRot"></param>
/// <param name="right"></param>
/// <returns>回転後のベクトル</returns>
Vector3 operator*(const Quaternion qRot, const Vector3 right);

/// <summary>
/// ベクトルに対しクォータニオンの回転を適用する
/// </summary>
/// <param name="qRot"></param>
/// <param name="right"></param>
/// <returns>回転後のベクトル</returns>
Vector3 RotateVector3(const Quaternion qRot, const Vector3 right);

/// <summary>
/// 回転軸と回転角から回転クォータニオンを返す
/// </summary>
/// <param name="axis">回転軸ベクトル</param>
/// <param name="angle">回転角(ラジアン)</param>
/// <returns>回転クォータニオン</returns>
Quaternion AngleAxis(const Vector3& axis, const float& angle);

/// <summary>
/// 二つの四元数間の補間
/// </summary>
/// <param name="a">start</param>
/// <param name="b">end</param>
/// <param name="t">範囲(0.0-1.0)</param>
/// <returns>保管されたクォータニオン</returns>
Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

/// <summary>
/// 行列からクォータニオンの変換
/// 純粋な回転行列を渡さないと不具合が出る
/// </summary>
/// <returns></returns>
Quaternion ConvMatrix4x4ToQuaternion(const Matrix4x4 mat);

/// <summary>
/// クォータニオンから行列の変換
/// 参考:http://marupeke296.com/DXG_No58_RotQuaternionTrans.html
/// </summary>
/// <returns></returns>
Matrix4x4 ConvQuaternionToMatrix4x4(const Quaternion q);

/// <summary>
/// クォータニオンからオイラー角(Vector3)への変換を行う
/// ジンバルロックが発生しそうな付近は注意
/// </summary>
/// <param name="q"></param>
/// <returns></returns>
Vector3 ConvQuaternionToEuler(const Quaternion& q);

/// <summary>
/// オイラー角(Vector3)からクォータニオンへの変換を行う
/// </summary>
/// <param name="q"></param>
/// <returns></returns>
Quaternion ConvEulerToQuaternion(const Vector3& euler);

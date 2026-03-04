#pragma once
#include <array>		// Matrix用
#include "Library/Geometry/Vector3.h"

// 相互変換のためのプロトタイプ宣言
namespace DxLib {
	struct tagMATRIX;
}

/// <summary>
/// 行列クラス
/// 行列成分はDirectX形式(行方向が軸の向き)
/// (m[行(横)][列(縦)])
/// </summary>
class Matrix4x4 final {
public:
	Matrix4x4() : m({}) {};
	Matrix4x4(std::array<std::array<float, 4>, 4> m_) : m(m_) {};
	std::array<std::array<float, 4>, 4> m;

	// tagMATRIXとの相互変換
	operator DxLib::tagMATRIX();
	operator DxLib::tagMATRIX() const;
	Matrix4x4(const DxLib::tagMATRIX& mat);

	Matrix4x4 operator*(const Matrix4x4& mat) const;	// 乗算
	void operator*=(const Matrix4x4& mat);	// 乗算
	bool operator==(const Matrix4x4& mat);	// 等価比較

	/// <summary>
	/// 自身を使いベクトルを行列乗算し結果を返す
	/// </summary>
	/// <param name="vec">ベクトル</param>
	/// <returns></returns>
	Vector3 VecMultiple(const Vector3& vec) const;

	/// <summary>
	/// 行列を引数倍する
	/// </summary>
	/// <param name="scale">倍率</param>
	/// <returns></returns>
	void MatScale(const float& scale);

	/// <summary>
	/// DxLibの行列を変換する関数
	/// </summary>
	/// <param name="mat"></param>
	/// <returns></returns>
	DxLib::tagMATRIX GetMATRIX() const;
};

/// <summary>
/// 単位行列を返す
/// </summary>
/// <returns></returns>
Matrix4x4 MatIdentity();

/// <summary>
/// 平行移動行列を返す
/// </summary>
/// <param name="vec">平行移動量</param>
/// <returns></returns>
Matrix4x4 MatTranslate(const Vector3& vec);
/// <summary>
/// 平行移動行列を返す
/// </summary>
/// <param name="x">x方向平行移動量</param>
/// <param name="y">y方向平行移動量</param>
/// <param name="z">z方向平行移動量</param>
/// <returns></returns>
Matrix4x4 MatTranslate(const float& x, const float& y, const float& z);

/// <summary>
/// x軸回転行列を返す
/// </summary>
/// <param name="angle">回転値</param>
/// <returns></returns>
Matrix4x4 MatRotateX(const float& angle);

/// <summary>
/// y軸回転行列を返す
/// </summary>
/// <param name="angle">回転値</param>
/// <returns></returns>
Matrix4x4 MatRotateY(const float& angle);

/// <summary>
/// z軸回転行列を返す
/// </summary>
/// <param name="angle">回転値</param>
/// <returns></returns>
Matrix4x4 MatRotateZ(const float& angle);

/// <summary>
/// 二つの行列の乗算を返す
/// </summary>
/// <param name="lmat">左辺値</param>
/// <param name="rmat">右辺値</param>
/// <returns></returns>
Matrix4x4 MatMultiple(const Matrix4x4& lmat, const Matrix4x4& rmat);

/// <summary>
/// ベクトルに対して行列乗算し結果を返す
/// </summary>
/// <param name="mat">行列</param>
/// <param name="vec">ベクトル</param>
/// <returns></returns>
Vector3 VecMultiple(const Matrix4x4& mat, const Vector3& vec);

/// <summary>
/// 拡大行列を返す
/// </summary>
/// <param name="scale">拡大率</param>
/// <returns></returns>
Matrix4x4 MatGetScale(const Vector3& scale);

/// <summary>
/// 逆行列の導出
/// </summary>
/// <param name="mat">行列</param>
/// <returns></returns>
Matrix4x4 MatInverse(const Matrix4x4& mat);

/// <summary>
/// 転置行列を返す
/// </summary>
/// <param name="mat">行列</param>
/// <returns></returns>
Matrix4x4 MatTranspose(const Matrix4x4& mat);

/// <summary>
/// <para> eyeからZ+方向をtargetへ向けた行列を返す </para>
/// <para> Y軸回転成分のみを適用 </para>
/// </summary>
/// <param name="eye">始点</param>
/// <param name="target">終点</param>
/// <param name="targetDist">targetからの距離</param>
/// <param name="up">上ベクトル</param>
/// <returns></returns>
Matrix4x4 GetLookAtMatrixY(Position3 eye, Position3 target, float targetDist, Vector3 up = Vector3Up());

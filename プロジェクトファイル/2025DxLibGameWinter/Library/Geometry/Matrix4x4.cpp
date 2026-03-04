#include "Matrix4x4.h"
#include "Vector3.h"

#include <DxLib.h>
#include <cassert>

Matrix4x4 MatTranslate(const Vector3& vec) {
	return MatTranslate(vec.x, vec.y, vec.z);
}

Matrix4x4 MatTranslate(const float& x, const float& y, const float& z) {
	Matrix4x4 ret = MatIdentity();
	ret.m[3][0] = x;
	ret.m[3][1] = y;
	ret.m[3][2] = z;
	return ret;
}

Matrix4x4::operator DxLib::tagMATRIX()
{
	return GetMATRIX();
}

Matrix4x4::operator DxLib::tagMATRIX() const
{
	return GetMATRIX();
}

Matrix4x4::Matrix4x4(const DxLib::tagMATRIX& mat)
{
	const int range = 4;
	for (int i = 0; i < range; ++i) {
		for (int j = 0; j < range; ++j) {
			m[i][j] = mat.m[i][j];
		}
	}
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const {
	return MatMultiple(*this, mat);
}

void Matrix4x4::operator*=(const Matrix4x4& mat) {
	*this = MatMultiple(*this, mat);
}

bool Matrix4x4::operator==(const Matrix4x4& mat)
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if ((*this).m[i][j] != mat.m[i][j]) return false;
		}
	}
	return true;
}

Vector3 Matrix4x4::VecMultiple(const Vector3& vec) const {
	Vector3 ret = {};
	ret.x = this->m[0][0] * vec.x + this->m[0][1] * vec.y + this->m[0][2] * vec.z + this->m[0][3];
	ret.y = this->m[1][0] * vec.x + this->m[1][1] * vec.y + this->m[1][2] * vec.z + this->m[1][3];
	ret.z = this->m[2][0] * vec.x + this->m[2][1] * vec.y + this->m[2][2] * vec.z + this->m[2][3];
	return ret;
}

void Matrix4x4::MatScale(const float& scale) {
	if (scale == 0.0f) {
		*this = MatIdentity();	// スケールがゼロの場合は単位行列に戻す
		return;
	}
	const int range = 4;//sizeof(ret.m[0]) / sizeof(ret.m[0][0]);
	for (int i = 0; i < range; ++i) {
		this->m[i][i] *= scale;
	}
	// 修正前
	//for (int k = 0; k < range; ++k) {
	//	for (int j = 0; j < range; ++j) {
	//		for (int i = 0; i < range; ++i) {
	//			this->m[k][j] *= scale;
	//		}
	//	}
	//}
}

tagMATRIX Matrix4x4::GetMATRIX() const
{
	tagMATRIX ret;
	const int range = 4;//sizeof(ret.m[0]) / sizeof(ret.m[0][0]);
	for (int i = 0; i < range; ++i) {
		for (int j = 0; j < range; ++j) {
			ret.m[i][j] = m[i][j];
		}
	}
	return ret;
}

Matrix4x4 MatIdentity() {
	Matrix4x4 ret = {};
	ret.m[0][0] = ret.m[1][1] = ret.m[2][2] = ret.m[3][3] = 1;
	return ret;
}

Matrix4x4 MatRotateX(const float& angle)
{
	Matrix4x4 ret = MatIdentity();
	ret.m[1][1] = static_cast<float>(cos(angle));
	ret.m[1][2] = static_cast<float>(sin(angle));
	ret.m[2][1] = static_cast<float>(-sin(angle));
	ret.m[2][2] = static_cast<float>(cos(angle));
	return ret;
}

Matrix4x4 MatRotateY(const float& angle) {
	Matrix4x4 ret = MatIdentity();
	ret.m[0][0] = static_cast<float>(cos(angle));
	ret.m[0][2] = static_cast<float>(-sin(angle));
	ret.m[2][0] = static_cast<float>(sin(angle));
	ret.m[2][2] = static_cast<float>(cos(angle));
	return ret;
}

Matrix4x4 MatRotateZ(const float& angle)
{
	Matrix4x4 ret = MatIdentity();
	ret.m[0][0] = static_cast<float>(cos(angle));
	ret.m[0][1] = static_cast<float>(sin(angle));
	ret.m[1][0] = static_cast<float>(-sin(angle));
	ret.m[1][1] = static_cast<float>(cos(angle));
	return ret;
}

Matrix4x4 MatMultiple(const Matrix4x4& lmat, const Matrix4x4& rmat) {
	Matrix4x4 ret = {};
	const int range = 4;//sizeof(ret.m[0]) / sizeof(ret.m[0][0]);
	for (int i = 0; i < range; ++i) {
		for (int j = 0; j < range; ++j) {
			for (int k = 0; k < range; ++k) {
				ret.m[i][j] += lmat.m[i][k] * rmat.m[k][j];
			}
		}
	}
	return ret;
	// 分解↓
	// kは行、jは列、iは結果の一要素にかける計算回数
	// 4x4行列の掛け算
	// ret = lmat * rmat
	//
	// 各要素の計算方法：
	// ret.m[i][j] = lmat.m[i][0] * rmat.m[0][j]
	//             + lmat.m[i][1] * rmat.m[1][j]
	//             + lmat.m[i][2] * rmat.m[2][j]
	//             + lmat.m[i][3] * rmat.m[3][j];
	// │a b c d│   │e f g h│   │ae+bf+cg+dh ...│
	// │e f g h│ x │i j k l│ = │...            │
	// │i j k l│   │m n o p│   │...            │
	// │m n o p│   │q r s t│   │...            │
	//
	// ※ ret.m[行][列] = 左の行 × 右の列 の積の総和
}

Vector3 VecMultiple(const Matrix4x4& mat, const Vector3& vec) {
	return mat.VecMultiple(vec);
}

Matrix4x4 MatGetScale(const Vector3& scale) {
	Matrix4x4 ret = MatIdentity();
	ret.m[0][0] = scale.x;
	ret.m[1][1] = scale.y;
	ret.m[2][2] = scale.z;
	return ret;
}

Matrix4x4 MatInverse(const Matrix4x4& mat) {
	// DxLibの逆行列導出を利用する
	return DxLib::MInverse(mat);
	
	// 旧コード(掃き出し法)
	/*
	Matrix4x4 ret = mat;
	float temp = 0;
	for (int i = 0; i < 4; ++i) {
		ret.m[1][i] -= ret.m[0][i];		// 1. 2行から1行を引く
		ret.m[2][i] -= ret.m[0][i];		// 2. 3行から1行を引く
		ret.m[3][i] += ret.m[0][i];		// 3. 4行に  1行を足す
		temp = ret.m[1][i];		// 4. 2行と  4行を入れ替える
		ret.m[1][i] = ret.m[3][i];
		ret.m[3][i] = temp;
		ret.m[1][i] *= 0.5f;			// 5. 2行に  1/2(0.5)を掛ける
		ret.m[0][i] -= ret.m[1][i];		// 6. 1行から2行を引く
		ret.m[2][i] += ret.m[0][i] * 2;	// 7. 3行に  2行*2を足す
		ret.m[2][i] *= 0.5f;			// 8. 3行に  1/2(0.5)を掛ける
		ret.m[1][i] -= ret.m[2][i];		// 9. 2行から3行を引く
		ret.m[3][i] += ret.m[2][i] * 2;	// 10.4行に  3行*2を足す
		ret.m[3][i] *= 0.25f;			// 11.4行に  1/4(0.25)を掛ける
		ret.m[0][i] += ret.m[3][i];		// 12.1行に  4行を足す
		ret.m[1][i] += ret.m[3][i];		// 13.2行に  4行を足す
		ret.m[2][i] -= ret.m[3][i];		// 14.3行から4行を引く
	}
	return ret;
	*/
}

Matrix4x4 MatTranspose(const Matrix4x4& mat)
{
	// 転置
	Matrix4x4 ret;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ret.m[i][j] = mat.m[j][i];
		}
	}
	return ret;
}

Matrix4x4 GetLookAtMatrixY(Position3 eye, Position3 target, float targetDist, Vector3 up)
{
	// eyeからtargetの方向
	VECTOR dir = VSub(target, eye);
	dir.y = 0.0f;       // 水平
	dir = VNorm(dir);

	// target から dir の逆方向に targetDist だけ離す
	VECTOR pos = VSub(target, VScale(dir, targetDist));

	// X軸
	// デフォルトでは Upは常にY+
	VECTOR xaxis = VCross(dir, up);
	xaxis = VNorm(xaxis);
	// Y軸
	VECTOR yaxis = up;
	// ローカル前方向 Z 軸
	VECTOR zaxis = dir;  // Z+ を対象方向へ向ける
	// 行列作成
	MATRIX mat;

	mat.m[0][0] = xaxis.x;  mat.m[0][1] = yaxis.x;  mat.m[0][2] = zaxis.x;  mat.m[0][3] = 0.0f;
	mat.m[1][0] = xaxis.y;  mat.m[1][1] = yaxis.y;  mat.m[1][2] = zaxis.y;  mat.m[1][3] = 0.0f;
	mat.m[2][0] = xaxis.z;  mat.m[2][1] = yaxis.z;  mat.m[2][2] = zaxis.z;  mat.m[2][3] = 0.0f;
	mat.m[3][0] = pos.x;    mat.m[3][1] = pos.y;    mat.m[3][2] = pos.z;    mat.m[3][3] = 1.0f;

	return mat;
}

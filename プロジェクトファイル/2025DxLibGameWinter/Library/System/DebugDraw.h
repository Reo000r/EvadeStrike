#pragma once
#include "Library/Geometry/Vector3.h"
#include <vector>
#include <string>

/// <summary>
/// デバッグ用の描画情報をまとめ、後で表示するクラス
/// </summary>
class DebugDraw final {
private:
	DebugDraw();
	DebugDraw(const DebugDraw&) = delete;
	void operator=(const DebugDraw&) = delete;

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static DebugDraw& GetInstance();

	void Clear();
	void Draw() const;

	/// <summary>
	/// 線分描画情報登録
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="color"></param>
	void DrawLine(const Vector3& start, const Vector3& end, int color);

	/// <summary>
	/// 球描画情報登録
	/// </summary>
	/// <param name="center"></param>
	/// <param name="rad"></param>
	/// <param name="color"></param>
	void DrawSphere(const Vector3& center, float rad, int color);
	
	/// <summary>
	/// カプセル描画情報登録
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="rad"></param>
	/// <param name="color"></param>
	void DrawCapsule(const Vector3& start, const Vector3& end, float rad, int color);

private:

	// 線分情報
	struct LineInfo {
		Vector3	start;
		Vector3	end;
		int		color;
	};
	// 球情報
	struct SphereInfo {
		Vector3	center;
		float	rad;
		int		color;
	};
	// カプセル情報
	struct CapsuleInfo {
		Vector3	start;
		Vector3	end;
		float	rad;
		int		color;
	};

	std::vector<LineInfo>	_lineInfo;
	std::vector<SphereInfo>	_sphereInfo;
	std::vector<CapsuleInfo>	_capsuleInfo;
};
#pragma once
#include "Library/Geometry/Matrix4x4.h"
#include "Library/Geometry/Vector3.h"
#include <string>
#include <memory>

/// <summary>
/// モデルを管理する
/// </summary>
class Model : public std::enable_shared_from_this<Model>
{
public:
	Model();
	virtual ~Model();

	/// <summary>
	/// モデル描画
	/// </summary>
	void Draw();

	/// <summary>
	/// モデルハンドルを設定する
	/// </summary>
	/// <returns></returns>
	void SetHandle(int handle) { _handle = handle; }
	/// <summary>
	/// モデルハンドルを返す
	/// </summary>
	/// <returns></returns>
	int GetHandle() const { return _handle; }

	/// <summary>
	/// 描画状態を変更する
	/// </summary>
	/// <returns></returns>
	void SetVisible(bool isVisible) const;
	/// <summary>
	/// 描画状態を返す
	/// </summary>
	/// <returns></returns>
	bool GetVisible() const;

	/// <summary>
	/// モデルに位置を適用する
	/// </summary>
	/// <param name="pos"></param>
	void AttachPos(const Position3& pos);

	/// <summary>
	/// <para> 指定されたフレーム名の行列を返す </para>
	/// <para> 見つからなかった場合は単位行列を返す </para>
	/// </summary>
	/// <param name="frameName"></param>
	/// <returns></returns>
	Matrix4x4 GetFrameMatrix(const std::wstring& frameName) const;

	/// <summary>
	/// モデルの位置を返す(当たり判定の位置ではない)
	/// </summary>
	/// <returns></returns>
	Position3 GetPos();
	/// <summary>
	/// モデルの行列を返す
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetMatrix();

	Position3 GetDrawOffset() const { return _drawOffset; }
	/// <summary>
	/// 描画位置補正を設定
	/// </summary>
	/// <param name="offset"></param>
	void SetDrawOffset(Position3 offset);

protected:

	int _handle;
	Position3 _drawOffset;
};


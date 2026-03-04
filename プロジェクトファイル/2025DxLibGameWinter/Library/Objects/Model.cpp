#include "Model.h"
#include <DxLib.h>
#include <cassert>

Model::Model() :
	_handle(-1),
	_drawOffset()
{
}

Model::~Model()
{
	if (_handle >= 0) MV1DeleteModel(_handle);
}

void Model::Draw()
{
	if (_handle >= 0) {
		MV1DrawModel(_handle);
	}
}

void Model::SetVisible(bool isVisible) const
{
	MV1SetVisible(_handle, isVisible);
}

bool Model::GetVisible() const
{
	return MV1GetVisible(_handle);
}

void Model::AttachPos(const Position3& pos)
{
	if (_handle >= 0) MV1SetPosition(_handle, pos + _drawOffset);
}

Matrix4x4 Model::GetFrameMatrix(const std::wstring& frameName) const
{
	// モデルが有効かチェック
	if (_handle < 0) {
		assert(false && "不正なモデルハンドル");
		return MatIdentity();	// 単位行列を返す
	}
	// フレームのインデックスを検索
	int frameIndex = MV1SearchFrame(_handle, frameName.c_str());
	// インデックスが有効かチェック
	if (frameIndex < 0) {
		assert(false && "指定されたフレームが見つからなかった");
		return MatIdentity();	// 単位行列を返す
	}
	// フレームのワールド行列を取得し返す
	return MV1GetFrameLocalWorldMatrix(_handle, frameIndex);
}

Position3 Model::GetPos()
{
	return MV1GetPosition(_handle);
}

Matrix4x4 Model::GetMatrix()
{
	return MV1GetMatrix(_handle);
}

void Model::SetDrawOffset(Position3 offset)
{
	// 前回との差分だけ足す
	Position3 pos = MV1GetPosition(_handle);
	MV1SetPosition(_handle, pos + (offset - _drawOffset));
	_drawOffset = offset;
}

#include "SkyDome.h"
#include "Objects/Camera/Camera.h"

#include <DxLib.h>
#include <cassert>

Skydome::Skydome() :
	_handle(-1),
	_camera()
{
}

Skydome::~Skydome()
{
	MV1DeleteModel(_handle);
}

void Skydome::Init(std::weak_ptr<Camera> camera)
{
	_camera = camera;

	// スカイドーム読み込み
	_handle = MV1LoadModel(L"Data/Model/Skydome/Sky_Daylight01.mv1");
	assert(_handle >= 0);
	// 調整
	MV1SetScale(_handle, Vector3(1, 1, 1) * 32.0f);
	MV1SetPosition(_handle, _camera.lock()->GetPos());

	// 自身がZバッファに干渉しないようにする
	MV1SetWriteZBuffer(_handle, FALSE);
}

void Skydome::Update()
{
	MV1SetPosition(_handle, _camera.lock()->GetPos());
}

void Skydome::Draw()
{
	MV1DrawModel(_handle);
}

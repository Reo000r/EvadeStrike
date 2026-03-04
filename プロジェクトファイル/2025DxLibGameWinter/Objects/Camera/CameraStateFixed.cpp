#include "CameraStateFixed.h"
#include "Camera.h"

CameraStateFixed::CameraStateFixed(
	std::weak_ptr<Camera> parent, const Position3& fixedPos, const Position3& fixedTarget) :
	CameraStateBase(parent),
	_fixedPos(fixedPos),
	_fixedTarget(fixedTarget)
{
}

void CameraStateFixed::OnEnter()
{
	// アニメーション開始時の位置と注視点を設定
	_camera.lock()->SetPos(_fixedPos);
	_camera.lock()->SetTargetPos(_fixedTarget);
}

void CameraStateFixed::Update()
{
	// 処理なし
}

void CameraStateFixed::OnExit()
{
	// 処理なし
}

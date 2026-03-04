#include "CameraStateBase.h"

CameraStateBase::CameraStateBase(std::weak_ptr<Camera> parent)
{
	_camera = parent;
}

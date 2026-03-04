#include "Camera.h"
#include "Library/Geometry/Calculation.h"
#include "CameraShaker.h"
#include "CameraStateFollow.h"
#include "Library/Physics/Collider.h"
#include "Library/System/Event/EventCollider.h"
#include "Library/System/UI/UIManager.h"
#include "Library/System/UI/UILockonTarget.h"
#include <DxLib.h>

namespace {
	constexpr float kTargetHormingRad = 1000.0f;

	constexpr float kRotSpeedX = -0.03f;
	constexpr float kRotSpeedY = -0.05f;

	// 初期回転量
	const Vector3 kDefaultRotAngle = Vector3(0, Calc::ToRadian(180.0f), 0);


	// ライトの有効距離
	constexpr float kPointLightRange = 50000.0f;
	// 減衰係数
	// DxLibの仕様： (1 / (a0 + a1*d + a2*d*d))
	constexpr float kPointLightAtten0 = 1.0f;
	constexpr float kPointLightAtten1 = 0.0f;
	constexpr float kPointLightAtten2 = 0.0f;
}

Camera::Camera() :
	_pos(),
	_targetPos(),
	_up(Vector3Up()),
	_rotAngle(kDefaultRotAngle),
	_near(10.0f),
	_far(100000.0f),
	_viewAngle(Calc::ToRadian(60.0f)),
	_lightHandle(-1),
	_currentState(),
	_shaker(std::make_shared<CameraShaker>()),
	_lockOnGraph()
{
}

Camera::~Camera()
{
	if (_lightHandle != -1) {
		DeleteLightHandle(_lightHandle);
		_lightHandle = -1;
	}
}

void Camera::Init(std::shared_ptr<CameraStateBase>& state)
{
	// UI生成
	auto lockonGraph = std::make_shared<UILockonTarget>(
		LoadGraph(L"Data/Graph/LockonTarget.png"),
		Position2(0, 0), 0.8f
	);
	UIManager::GetInstance().RegisterUI(lockonGraph);
	_lockOnGraph = lockonGraph;
	
	// 初期ステートを設定する
	ChangeState(state);
	// DxLibのカメラ設定
	SetCameraNearFar(_near, _far);
	SetupCamera_Perspective(_viewAngle);

	_lightHandle = CreatePointLightHandle(
		_pos,
		kPointLightRange,
		kPointLightAtten0,
		kPointLightAtten1,
		kPointLightAtten2
	);
}

void Camera::Update()
{
	// ステートの更新
	if (_currentState) {
		_currentState->Update();
	}

	// 揺れの更新
	_shaker->Update();

	// 最終的なカメラ情報をDxLibに適用
	ApplyToDxLibCamera();

	if (_lightHandle > 0) {
		//// カメラ視線方向
		//Vector3 dir = _targetPos - _pos;
		//dir.Normalize();

		//SetLightDirectionHandle(_lightHandle, dir);

		// カメラ位置 + 揺れを反映
		Vector3 finalPos = _pos + _shaker->GetCurrentOffset();

		SetLightPositionHandle(_lightHandle, finalPos);
	}
}

void Camera::ChangeState(std::shared_ptr<CameraStateBase>& newState)
{
	// 有効であればステート終了処理
	if (_currentState) {
		_currentState->OnExit();
	}
	// ステート変更
	_currentState = newState;
	// 有効であればステート開始処理
	if (_currentState) {
		_currentState->OnEnter();
	}
}

void Camera::Shake(float magnitude, int durationFrame, float frequency)
{
	_shaker->Shake(magnitude, durationFrame, frequency);
}

void Camera::SetStageObjectColliders(
	std::vector<std::shared_ptr<Collider>> stageColliders, 
	std::vector<std::shared_ptr<EventCollider>> eventColliders)
{
	_stageColliders.clear();
	_eventColliders.clear();

	_stageColliders = stageColliders;
	_eventColliders = eventColliders;
}

std::vector<std::weak_ptr<Collider>> Camera::GetStageColliders()
{
	// _stageColliders と _eventColliders を合わせて返す
	std::vector<std::weak_ptr<Collider>> ret;

	// 事前に必要な容量を確保
	ret.reserve(_stageColliders.size() + _eventColliders.size());

	// 各リストの要素を末尾に追加
	ret.insert(ret.end(), _stageColliders.begin(), _stageColliders.end());
	std::vector<std::weak_ptr<Collider>> eventCols;
	for (const auto& col : _eventColliders) {
		// 削除済みであれば飛ばす
		if (col->IsExpired()) continue;
		eventCols.emplace_back(col->GetCollider());
	}
	ret.insert(ret.end(), eventCols.begin(), eventCols.end());

	return ret;
}

void Camera::ApplyToDxLibCamera() const
{
	// ステートによる位置に、揺れのオフセットを加える
	Vector3 finalPosition = _pos + _shaker->GetCurrentOffset();
	Vector3 finalTarget = _targetPos;

	SetCameraPositionAndTargetAndUpVec(finalPosition, finalTarget, _up);
}

void Camera::AddOtherTarget(std::weak_ptr<CharacterBase> target)
{
	// nullptr以外が返ってきたならそのステート
	if (std::dynamic_pointer_cast<CameraStateFollow>(_currentState)) {
		std::static_pointer_cast<CameraStateFollow>(_currentState)->AddOtherTarget(target);
	}
}

void Camera::ReleaseOtherTarget(std::weak_ptr<CharacterBase> target)
{
	// nullptr以外が返ってきたならそのステート
	if (std::dynamic_pointer_cast<CameraStateFollow>(_currentState)) {
		std::static_pointer_cast<CameraStateFollow>(_currentState)->ReleaseOtherTarget(target);
	}
}

void Camera::ResetOtherTarget()
{
	// nullptr以外が返ってきたならそのステート
	if (std::dynamic_pointer_cast<CameraStateFollow>(_currentState)) {
		std::static_pointer_cast<CameraStateFollow>(_currentState)->ResetOtherTarget();
	}
}



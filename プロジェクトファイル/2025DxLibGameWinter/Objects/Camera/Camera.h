#pragma once
#include "Library/Geometry/Vector3.h"
#include <memory>
#include <vector>

class CameraStateBase;
class CameraShaker;
class CharacterBase;
class Collider;
class EventCollider;
class UILockonTarget;

/// <summary>
/// カメラを管理するクラス
/// </summary>
class Camera final : public std::enable_shared_from_this<Camera> {
public:
	Camera();
	~Camera();

	void Init(std::shared_ptr<CameraStateBase>& state);
	void Update();

	/// <summary>
	/// カメラのステートを変更する
	/// </summary>
	void ChangeState(std::shared_ptr<CameraStateBase>& newState);

	/// <summary>
	/// カメラを揺らす
	/// </summary>
	/// <param name="magnitude">揺れの強さ</param>
	/// <param name="durationFrame">持続フレーム</param>
	/// <param name="frequency">振動数</param>
	void Shake(float magnitude, int durationFrame, float frequency);

	const Position3& GetPos() const { return _pos; }
	void SetPos(Position3 pos) { _pos = pos; }
	const Position3& GetTargetPos() const { return _targetPos; }
	void SetTargetPos(const Position3& target) { _targetPos = target; }
	const Vector3 GetRotAngle() const { return _rotAngle; }
	void AddRotAngle(Vector3 angle) { _rotAngle += angle; }
	void SetRotAngle(Vector3 angle) { _rotAngle = angle; }
	const float GetRotAngleX() const { return _rotAngle.x; }
	const float GetRotAngleY() const { return _rotAngle.y; }
	void SetRotAngleX(float angle) { _rotAngle.x = angle; }
	void SetRotAngleY(float angle) { _rotAngle.y = angle; }

	std::weak_ptr<UILockonTarget> GetLockonGraph() { return _lockOnGraph; }

	/// <summary>
	/// 当たり判定を行うステージを登録
	/// </summary>
	/// <param name=""></param>
	void SetStageObjectColliders(
		std::vector<std::shared_ptr<Collider>> stageColliders, 
		std::vector<std::shared_ptr<EventCollider>> eventColliders);

	std::vector<std::weak_ptr<Collider>> GetStageColliders();

	std::shared_ptr<CameraShaker> GetShaker() { return _shaker; }

	/// <summary>
	/// DxLibのカメラ設定を更新する
	/// </summary>
	void ApplyToDxLibCamera() const;

	void AddOtherTarget(std::weak_ptr<CharacterBase> target);
	void ReleaseOtherTarget(std::weak_ptr<CharacterBase> target);
	void ResetOtherTarget();

private:
	Position3 _pos;
	Position3 _targetPos;
	Vector3 _up;
	Vector3 _rotAngle;	// 軸回転情報

	// カメラ設定
	float _near;
	float _far;
	float _viewAngle;

	// カメラが管理するライト
	int _lightHandle;

	std::weak_ptr<UILockonTarget> _lockOnGraph;

	// 現在のステート
	std::shared_ptr<CameraStateBase> _currentState;
	// 揺れコンポーネント
	std::shared_ptr<CameraShaker> _shaker;

	std::vector<std::shared_ptr<Collider>> _stageColliders;
	std::vector<std::shared_ptr<EventCollider>> _eventColliders;
};


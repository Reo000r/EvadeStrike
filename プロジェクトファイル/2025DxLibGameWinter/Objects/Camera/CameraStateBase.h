#pragma once
#include <memory>

class Camera;
class Player;

/// <summary>
/// カメラステートの基底クラス
/// </summary>
class CameraStateBase abstract
{
public:
	CameraStateBase(std::weak_ptr<Camera> parent);
	virtual ~CameraStateBase() = default;

	/// <summary>
	/// ステート開始直後の処理
	/// </summary>
	virtual void OnEnter() abstract;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() abstract;
	/// <summary>
	/// ステート終了直前の処理
	/// </summary>
	virtual void OnExit() abstract;

	/// <summary>
	/// 処理が終わったか
	/// </summary>
	/// <returns></returns>
	virtual bool IsComplete() const abstract;
	/// <summary>
	/// ステートが変更可能か
	/// </summary>
	/// <returns></returns>
	virtual bool CanChangeState() const abstract;

protected:

	std::weak_ptr<Camera> _camera;
};


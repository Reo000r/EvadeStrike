#pragma once
#include "CameraStateBase.h"
#include "Library/Geometry/Vector3.h"

/// <summary>
/// 指定された位置と注視点でカメラを固定する状態
/// </summary>
class CameraStateFixed : public CameraStateBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="parent">親クラス</param>
    /// <param name="fixedPos">固定するカメラの位置</param>
    /// <param name="fixedTarget">固定するカメラの注視点</param>
    CameraStateFixed(std::weak_ptr<Camera> parent, 
        const Position3& fixedPos, const Position3& fixedTarget);
    ~CameraStateFixed() = default;

    void OnEnter() override;
    void Update() override;
    void OnExit() override;

    bool IsComplete() const override { return true; }
    bool CanChangeState() const override { return true; }

private:
    Position3 _fixedPos;		// 固定カメラの位置
    Position3 _fixedTarget;		// 固定カメラの注視点
};


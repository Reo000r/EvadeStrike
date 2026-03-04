#pragma once
#include "CameraStateBase.h"
#include "Library/Geometry/Vector3.h"

class Camera;

/// <summary>
/// 指定位置・注視点間をアニメーションするカメラの状態
/// </summary>
class CameraStateAnimation : public CameraStateBase
{
public:
    CameraStateAnimation(
        std::weak_ptr<Camera> parent,
        const Position3& startPos, const Position3& endPos,
        const Position3& startTargetPos, const Position3& endTargetPos,
        int totalFrame, int stopFrame = 0);
    ~CameraStateAnimation() = default;

    void OnEnter() override;
    void Update() override;
    void OnExit() override;

    bool IsComplete() const override { return _isComplete; }
    bool CanChangeState() const override { return IsComplete(); }

private:
    Position3 _startPos;
    Position3 _endPos;
    Position3 _startTargetPos;
    Position3 _endTargetPos;

    
    int _totalFrame;    // アニメーションの全体フレーム数(停止時間は含まれない)
    int _stopFrame;     // アニメーション開始前に停止するフレーム数(全体フレームに含まれない)
    int _animFrame;     // 現在の進行フレーム
    bool _isComplete;   // アニメーションが終わったかどうか
};

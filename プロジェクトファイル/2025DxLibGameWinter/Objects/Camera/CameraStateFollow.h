#pragma once
#include "CameraStateBase.h"
#include "Library/Geometry/Vector3.h"
#include <memory>
#include <vector>

class CharacterBase;
class Player;

/// <summary>
/// 指定したオブジェクトを追従するステート
/// </summary>
class CameraStateFollow : public CameraStateBase
{
public:
    CameraStateFollow(std::weak_ptr<Camera> parent,
        std::weak_ptr<Player> target);
    ~CameraStateFollow() = default;

    void OnEnter() override;
    void Update() override;
    void OnExit() override;

    bool IsComplete() const override;
    bool CanChangeState() const override;

    bool IsLockOn() const { return _isLockOn; }

    void AddOtherTarget(std::weak_ptr<CharacterBase> target);
    void ReleaseOtherTarget(std::weak_ptr<CharacterBase> target);
    void ResetOtherTarget();

private:
    /// <summary>
    /// カメラを動かす
    /// </summary>
    void CameraUpdate(Vector3 rotAngle, float followLerpFactor);

    void UpdateLockOnFrame();

    /// <summary>
    /// 徐々にtargetの進行方向へ向ける
    /// </summary>
    void ApplyTargetDir();

    Vector3 GetPlayerMoveDir();

    void SetLockonGraphState(bool isDraw, Position3 targetPos, float targetHitPointRatio);

    // ターゲットとなるキャラクター
    std::weak_ptr<Player> _target;
    std::vector<std::weak_ptr<CharacterBase>> _otherTargets;
    // 注視点のオフセット
    Vector3 _targetOffset;
    // カメラの回転量
    //Vector3 _rotAngle;

    bool _isLockOn;
    int _lockOnFrame;
};

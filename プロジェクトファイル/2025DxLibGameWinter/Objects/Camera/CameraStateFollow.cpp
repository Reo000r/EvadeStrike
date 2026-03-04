#include "CameraStateFollow.h"
#include "Camera.h"
#include "Library/Geometry/Vector3.h"
#include "Library/Geometry/Matrix4x4.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Statistics.h"
#include "Library/System/Input.h"
#include "Library/Objects/Character/CharacterBase.h"
#include "Objects/Character/Player/Player.h"
#include "Library/Physics/Collider.h"
#include "Library/Physics/ColliderDataPolygon.h"
#include "Library/System/UI/UILockonTarget.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>

#define IS_CAMERA_REVERSE

namespace {
    // 通常時のカメラ関連の定数
#ifdef IS_CAMERA_REVERSE
    constexpr float kRotReverse = -1.0f;
#else
    constexpr float kRotReverse = 1.0f;
#endif // IS_CAMERA_REVERSE

    constexpr float kRotSpeedX = -0.02f * kRotReverse;
    constexpr float kRotSpeedY = 0.06f * kRotReverse;
    constexpr float kRotAngleXMin = -0.45f;
    constexpr float kRotAngleXMax = 0.85f;
    constexpr float kTargetTurnLerp = 0.02f;
    constexpr float kDefaultCameraFollowLerpFactor = 0.1f;
    const Vector3 kPlayerToTarget = Vector3(0.0f, 150.0f, 0.0f);
    const Vector3 kTargetToCamera = Vector3(0.0f, 250.0f, 900.0f);
    const Vector3 kPlayerToCamera = kTargetToCamera + kPlayerToTarget;
    constexpr float kBaseTargetRate = 0.6f; // ベースに寄らせる量
    constexpr int kLockOnFrame = 2 * Statistics::kFPS; // ロックオン持続時間


    //// 線形補間速度
    //constexpr float kDefaultCameraFollowLerpFactor = 0.1f;		// 通常時の線形補間速度
    //constexpr float kStartAnimCameraFollowLerpFactor = 0.000000001f;	// 開始演出時の線形補間速度
    //constexpr float kEndAnimCameraFollowLerpFactor = 0.001f;	// 開始演出時の線形補間速度

    //const Vector3 kPlayerToTarget = Vector3(0.0f, 250.0f, 0.0f);
    //const Vector3 kTargetToCamera = Vector3(0.0f, 350.0f, 700.0f);
    //const Vector3 kPlayerToCamera = kTargetToCamera + kPlayerToTarget;

    //// 初期位置
    //const Vector3 kDefaultPosition = Vector3(
    //	kTargetToCamera.x, kTargetToCamera.y, kTargetToCamera.z * -1
    //	) + kPlayerToTarget;
    //// 初期回転量
    //const float kDefaultRotation = Calc::ToRadian(180.0f);

    //constexpr int kStartAnimationStopFrame = 60 * 3.0f;		// 開始アニメーション前の停止フレーム数
    //constexpr int kStartAnimationTotalFrame = 60 * 1.0f + kStartAnimationStopFrame;	// 開始アニメーションの総フレーム
    //constexpr int kEndAnimationTotalFrame = 60 * 4.0f;		// 終了アニメーションの総フレーム
    //const Position3 kStartAnimationPos		= Position3(0.0f, 1000.0f, -0000.0f);
    //const Vector3 kStartAnimationTargetPos	= Vector3(0.0f, 1700.0f, 1000.0f);
    ////const Position3 kStartAnimationPos		= Position3(0.0f, 100.0f, -800.0f);
    ////const Vector3 kStartAnimationTargetPos	= Vector3(0.0f, 1300.0f, 3000.0f);
    //const Position3 kEndAnimationPos		= kStartAnimationPos;
    //const Vector3 kEndAnimationTargetPos	= kStartAnimationTargetPos;

    //constexpr int kCanFadeoutFrameOffset = 20;
}

CameraStateFollow::CameraStateFollow(
    std::weak_ptr<Camera> parent, 
    std::weak_ptr<Player> target) :
    CameraStateBase(parent),
    _target(target),
    _otherTargets(),
    _isLockOn(false),
    _lockOnFrame(0)
{
    // 処理なし
}

void CameraStateFollow::OnEnter()
{
    // 初期位置設定
    Matrix4x4 rotMtx = MatIdentity();
    Matrix4x4 playerRotMtx = MatIdentity();
    float rotAngleX = _camera.lock()->GetRotAngle().x;
    rotAngleX =
        std::min<float>(kRotAngleXMax,
            std::max<float>(kRotAngleXMin, rotAngleX));
    _camera.lock()->SetRotAngleX(rotAngleX);

    Matrix4x4 cameraRotMtxX = MatRotateX(rotAngleX);
    Matrix4x4 cameraRotMtxY = MatRotateY(_camera.lock()->GetRotAngle().y);
    rotMtx = playerRotMtx * cameraRotMtxY * cameraRotMtxX;

    // ベースの追跡対象
    Vector3 baseTargetPos = Vector3Zero();
    if (_target.lock()) {
        baseTargetPos = _target.lock()->GetCenterPos();
    }
    // 他の追跡対象
    Vector3 otherTargetPos = Vector3Zero();
    float otherTargetHPRatio = 0.0f;
    for (auto& target : _otherTargets) {
        if (target.lock()) {
            otherTargetPos = target.lock()->GetCenterPos();
            if (target.lock()->GetMaxHitPoint() != 0.0f) {
                otherTargetHPRatio = target.lock()->GetHitPoint() / target.lock()->GetMaxHitPoint();
            }
            _isLockOn = true;
        }
    }
    // 他の対象の中点を求める
    otherTargetPos /= static_cast<float>(_otherTargets.size());
    // 最終的な追跡点を求める
    Vector3 targetPos = 
        baseTargetPos * kBaseTargetRate + 
        otherTargetPos * (1.0f - kBaseTargetRate);

    Vector3 toCamera = rotMtx.VecMultiple(kPlayerToCamera);
    Vector3 toTarget = rotMtx.VecMultiple(kPlayerToTarget);

    Vector3 finalPos = targetPos + toCamera;
    Vector3 finalTargetPos = targetPos + toTarget;

    // 現在位置と目標位置を線形補間して新しい位置を決定
    Position3 newPos = finalPos;
    Position3 newTargetPos = finalTargetPos;

    // 更新
    _camera.lock()->SetPos(newPos);
    _camera.lock()->ApplyToDxLibCamera();

    // ロックオンUI更新
    SetLockonGraphState(_isLockOn, otherTargetPos, otherTargetHPRatio);
}

void CameraStateFollow::Update()
{
    Input& input = Input::GetInstance();

    // スティックによる平面移動
    Vector2 stick = input.GetPadRightSitck();
    stick *= 0.001f;

    // スティックの縦成分 + X軸回転時の回転量
    // スティックの横成分 + Y軸回転時の回転量
    Vector3 rotAngle = Vector3(
        stick.y * kRotSpeedX, 
        stick.x * kRotSpeedY, 
        0.0f);

    // targetの進行方向に向く
    ApplyTargetDir();

    UpdateLockOnFrame();

    CameraUpdate(rotAngle, kDefaultCameraFollowLerpFactor);
}

void CameraStateFollow::OnExit()
{
}

bool CameraStateFollow::IsComplete() const
{
    // 完了条件なし
    return true;
}

bool CameraStateFollow::CanChangeState() const
{
    // 常に他のステートに遷移可能
    return true;
}

void CameraStateFollow::AddOtherTarget(std::weak_ptr<CharacterBase> target)
{
    // 見つからなければ登録
    auto it = std::find_if(
        _otherTargets.begin(), _otherTargets.end(),
        [&target](const std::weak_ptr<CharacterBase>& wptr) {
            return !wptr.expired() && !target.expired() &&
                wptr.lock() == target.lock();
        });

    // endであれば(見つかっていなければ)
    if (it == _otherTargets.end()) {
        _otherTargets.emplace_back(target);
        _isLockOn = true;
    }
}

void CameraStateFollow::ReleaseOtherTarget(std::weak_ptr<CharacterBase> target)
{
    std::shared_ptr<CharacterBase> sPtr = target.lock();
    // 登録解除(eraseif 要C++20)
    size_t count = std::erase_if(
        _otherTargets,
        [sPtr](std::weak_ptr<CharacterBase> target) { return target.lock() == sPtr; });
    // 登録されてなかったらassert
    if (count <= 0) {
        printf("Camera:指定のCharacterは未登録\n");
        //assert(false && "指定のCharacterは未登録");
    }

    // 追加のターゲットがいなければ
    if (_otherTargets.size() <= 0) {
        _isLockOn = false;
        SetLockonGraphState(false, Position3(0, 0, 0), 0.0f);
    }
}

void CameraStateFollow::ResetOtherTarget()
{
    _otherTargets.clear();
    _isLockOn = false;
    _lockOnFrame = 0;
    SetLockonGraphState(false, Position3(0,0,0), 0.0f);
}

void CameraStateFollow::CameraUpdate(Vector3 rotAngle, float followLerpFactor)
{
    // 軸回転
    _camera.lock()->AddRotAngle(rotAngle);

    Matrix4x4 rotMtx = MatIdentity();
    Matrix4x4 playerRotMtx = MatIdentity();
    float rotAngleX = _camera.lock()->GetRotAngle().x;
    rotAngleX = 
        std::min<float>(kRotAngleXMax,
            std::max<float>(kRotAngleXMin, rotAngleX));
    _camera.lock()->SetRotAngleX(rotAngleX);

    Matrix4x4 cameraRotMtxX = MatRotateX(rotAngleX);
    Matrix4x4 cameraRotMtxY = MatRotateY(_camera.lock()->GetRotAngle().y);
    rotMtx = playerRotMtx * cameraRotMtxY * cameraRotMtxX;


    // ベースの追跡対象
    Vector3 baseTargetPos = Vector3Zero();
    if (_target.lock()) {
        baseTargetPos = _target.lock()->GetCenterPos();
    }
    // 他の追跡対象
    Vector3 otherTargetPos = Vector3Zero();
    float otherTargetHPRatio = 0.0f;
    for (auto& target : _otherTargets) {
        if (target.lock()) {
            otherTargetPos += target.lock()->GetCenterPos();
            if (target.lock()->GetMaxHitPoint() != 0.0f) {
                otherTargetHPRatio = target.lock()->GetHitPoint() / target.lock()->GetMaxHitPoint();
            }
            _isLockOn = true;
        }
    }
    // 他の対象の中点を求める
    otherTargetPos /= static_cast<float>(_otherTargets.size());
    // 最終的な追跡点を求める
    Vector3 targetPos =
        baseTargetPos * kBaseTargetRate +
        otherTargetPos * (1.0f - kBaseTargetRate);
    if (!_isLockOn) {
        targetPos = baseTargetPos;
    }

    Vector3 toCamera = rotMtx.VecMultiple(kPlayerToCamera);
    Vector3 toTarget = rotMtx.VecMultiple(kPlayerToTarget);

    Vector3 finalPos = targetPos + toCamera;
    Vector3 finalTargetPos = targetPos + toTarget;

    // 現在位置と目標位置を線形補間して新しい位置を決定
    Position3 newPos = _camera.lock()->GetPos() * (1.0f - followLerpFactor) + finalPos * followLerpFactor;
    Position3 newTargetPos = _camera.lock()->GetTargetPos() * (1.0f - followLerpFactor) + finalTargetPos * followLerpFactor;


    // 当たり判定データからポリゴンデータを抽出
    auto colliders = _camera.lock()->GetStageColliders();
    std::vector<std::shared_ptr<ColliderDataPolygon>> colData;
    for (const auto col : colliders) {
        auto data = std::static_pointer_cast<ColliderDataPolygon>(col.lock()->GetColData());
        colData.emplace_back(data);
    }

    constexpr float kCameraWallOffset = 3.0f;
    // 遮蔽物検知による位置補正
    for (const auto& data : colData) {
        int stageModelHandle = data->GetModelHandle();

        Position3 startV = newTargetPos;
        Position3 endV = newPos;

        // 注視点からカメラ位置に向かってレイを飛ばす
        MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(stageModelHandle, -1, startV, endV);

        if (hitPoly.HitFlag == 1) {
            // 衝突点と法線の取得
            Vector3 hitPoint = hitPoly.HitPosition;
            Vector3 wallNormal = hitPoly.Normal;
            // カメラ位置の補正
            newPos = hitPoint + (wallNormal * kCameraWallOffset);
            // 注視点の補正
            newTargetPos = newTargetPos + (wallNormal * kCameraWallOffset);
        }
    }
    _camera.lock()->SetPos(newPos);
    _camera.lock()->SetTargetPos(newTargetPos);

    SetLockonGraphState(_isLockOn, otherTargetPos, otherTargetHPRatio);
}

void CameraStateFollow::UpdateLockOnFrame()
{
    if (!_isLockOn) return;
    _lockOnFrame++;
    if (_lockOnFrame >= kLockOnFrame) {
        ResetOtherTarget();
    }
}

void CameraStateFollow::ApplyTargetDir()
{
    Vector3 moveDir = GetPlayerMoveDir();
    // 動いていないとみなされるなら
    if (moveDir.Magnitude() < PhysicsData::kSleepThreshold) {
        return;
    }

    // 現在のカメラのY軸回転量
    float camYaw = _camera.lock()->GetRotAngle().y;
    float targetYaw = atan2f(moveDir.z, moveDir.x) + DX_PI_F * 0.5f;

    // 差分を最短方向に正規化
    float diff = targetYaw - camYaw;
    diff = Calc::RadianNormalize(diff);


    // プレイヤーがカメラ方向に動いている場合は補正を弱める
    // カメラの前方向（水平のみ）
    Vector3 camForward = Vector3(sinf(camYaw), 0.0f, cosf(camYaw));
    camForward.Normalized();
    moveDir.Normalized();

    // 内積し、1なら同方向、-1なら真逆
    float dot = Dot(camForward, moveDir);

    // 真逆(-1)なら補正無し、同方向(1)なら補正100%
    float dirFactor = abs(dot) * 0.1f + 0.2f;

    // 下限
    constexpr float kMinFactor = 0.5f;
    dirFactor = std::clamp(dirFactor, kMinFactor, 1.0f);

    // 補正量に方向係数を掛ける
    float addYaw = diff * kTargetTurnLerp * dirFactor;

    // 反映
    _camera.lock()->AddRotAngle(Vector3(0, addYaw, 0));
}

Vector3 CameraStateFollow::GetPlayerMoveDir()
{
    if (_target.expired()) return Vector3Zero();

    // 移動量を取得
    Vector3 vel = _target.lock()->GetVel();
    vel.y = 0.0f;
    // 移動していないとみなされる量なら
    if (vel.Magnitude() < PhysicsData::kSleepThreshold) {
        return Vector3Zero();
    };
    return vel.Normalize();
}

void CameraStateFollow::SetLockonGraphState(bool isDraw, Position3 targetPos, float targetHitPointRatio)
{
    // ロックオンUI更新
    auto lockOnGraph = _camera.lock()->GetLockonGraph();
    if (isDraw) {
        // 描画位置計算
        VECTOR rawDrawPos = ConvWorldPosToScreenPos(targetPos);
        Position2 drawPos = Position2(rawDrawPos.x, rawDrawPos.y);
        lockOnGraph.lock()->SetCenterPos(drawPos);

        // scale計算
        // 高さを少しずらしスクリーンに直した際の差を用いる
        Position3 offsetPos = targetPos;
        offsetPos.y += 300.0f;
        VECTOR offsetRawDrawPos = ConvWorldPosToScreenPos(offsetPos);
        Position2 offsetDrawPos = Position2(offsetRawDrawPos.x, offsetRawDrawPos.y);
        float dist = (drawPos - offsetDrawPos).Magnitude();
        int lockonGraphHandle = lockOnGraph.lock()->GetGraphHandle();
        int w, h;
        GetGraphSize(lockonGraphHandle, &w, &h);
        float scale = dist / h;
        lockOnGraph.lock()->SetScale(scale);
        lockOnGraph.lock()->SetPercent(targetHitPointRatio * 100.0f);

        lockOnGraph.lock()->SetDrawState(true);
    }
    else {
        lockOnGraph.lock()->SetDrawState(false);
    }
}

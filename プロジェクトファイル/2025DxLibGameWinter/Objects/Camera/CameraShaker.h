#pragma once
#include "Library/Geometry/Vector3.h"

/// <summary>
/// カメラの揺れを管理するクラス
/// </summary>
class CameraShaker
{
public:
    CameraShaker();
    ~CameraShaker() = default;

    void Update();

    /// <summary>
    /// カメラを揺らす
    /// </summary>
    /// <param name="magnitude">揺れの強さ</param>
    /// <param name="durationFrame">持続フレーム</param>
    /// <param name="frequency">振動数</param>
    void Shake(float magnitude, int durationFrame, float frequency);

    /// <summary>
    /// 現在の揺れによる位置のオフセットを取得
    /// </summary>
    Vector3 GetCurrentOffset() const;

private:
    // 揺れの強さ
    float _magnitude;
    // 持続フレーム数
    int _durationFrame;
    // 振動数
    float _frequency;
    // 経過フレーム
    int _elapsedFrame;
};

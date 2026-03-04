#include "CameraShaker.h"
#include "Library/Geometry/Calculation.h"
#include <DxLib.h>
#include <functional>

namespace {
    // 揺れの細かさ 増やすと細分化される
    constexpr int kShakeDimAmount = 500;
}

CameraShaker::CameraShaker() :
    _magnitude(0.0f),
    _durationFrame(0),
    _frequency(0.0f),
    _elapsedFrame(0)
{
}

void CameraShaker::Update()
{
    // 経過時間が持続時間を超えていなければ
    if (_elapsedFrame < _durationFrame) {
        // 経過時間を進める
        _elapsedFrame++;
    }
    else {
        // 揺れの強さをなくす
        _magnitude = 0.0f;
    }
}

void CameraShaker::Shake(float magnitude, int durationFrame, float frequency)
{
    _magnitude = magnitude;
    _durationFrame = durationFrame;
    _frequency = frequency;
    _elapsedFrame = 0;
}

Vector3 CameraShaker::GetCurrentOffset() const
{
    // 揺れの強さがないなら
    if (_magnitude == 0.0f) {
        // 揺れの量は0
        return Vector3Zero();
    }

    // 経過時間に基づいて揺れの量を減衰させる
    float currentMagnitude = _magnitude * (1.0f - static_cast<float>(_elapsedFrame) / _durationFrame);

    // 揺れを生成
    const int randAmount = kShakeDimAmount * 2;
    // ランダムな揺れの量を返す
    std::function<float()> GetShakeAmount = 
        [currentMagnitude] () {return (static_cast<float>(GetRand(randAmount)) / kShakeDimAmount - 1.0f) * currentMagnitude;};
    // 揺れの量を設定
    float shakeX = GetShakeAmount();
    float shakeY = GetShakeAmount();
    float shakeZ = GetShakeAmount();

    return Vector3(shakeX, shakeY, shakeZ);
}

#include "CameraStateAnimation.h"
#include "Camera.h"

CameraStateAnimation::CameraStateAnimation(
	std::weak_ptr<Camera> parent,
	const Position3& startPos, const Position3& endPos, 
	const Position3& startTargetPos, const Position3& endTargetPos, 
	int totalFrame, int stopFrame) :
	CameraStateBase(parent),
    _startPos(startPos),
    _endPos(endPos),
    _startTargetPos(startTargetPos),
    _endTargetPos(endTargetPos),
    _totalFrame(totalFrame),
    _stopFrame(stopFrame),
    _animFrame(0),
    _isComplete(false)
{
    // 処理なし
}

void CameraStateAnimation::OnEnter()
{
    // アニメーション開始時の位置と注視点を設定
    _camera.lock()->SetPos(_startPos);
    _camera.lock()->SetTargetPos(_startTargetPos);
}

void CameraStateAnimation::Update()
{
    if (_isComplete) return;

    _animFrame++;

    // アニメーションの終了時間を超えていたら
    if (_animFrame >= _totalFrame + _stopFrame) {
        _isComplete = true;
        _camera.lock()->SetPos(_endPos);
        _camera.lock()->SetTargetPos(_endTargetPos);
        return;
    }

    // 停止フレーム中は処理をしない
    if (_animFrame < _stopFrame) {
        return;
    }

    // イージングを用いた進行度の計算(easeOutCubic)
    // MEMO:イージングタイプを選択可能にしたい
    // https://easings.net/ja
    float rate = static_cast<float>(_animFrame - _stopFrame) / (_totalFrame - _stopFrame);
    rate = 1.0f - powf(1.0f - rate, 3.0f);    // easeOutCubic

    Position3 newPos = _startPos * (1.0f - rate) + _endPos * rate;
    Position3 newTarget = _startTargetPos * (1.0f - rate) + _endTargetPos * rate;

    _camera.lock()->SetPos(newPos);
    _camera.lock()->SetTargetPos(newTarget);
}

void CameraStateAnimation::OnExit()
{
    // 最終的な位置を再設定
    _camera.lock()->SetPos(_endPos);
    _camera.lock()->SetTargetPos(_endTargetPos);
}

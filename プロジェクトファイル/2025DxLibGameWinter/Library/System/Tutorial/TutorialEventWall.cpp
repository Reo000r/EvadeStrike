#include "TutorialEventWall.h"
#include "TutorialManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Statistics.h"
#include "Library/Geometry/Easing.h"
#include "Library/Physics/Collider.h"
#include "Library/Physics/ProjectSettings.h"

namespace {
	constexpr float kStageScale = 1.75f;
	const Vector3 kEffectPosOffset =
		Vector3(0, 500 * kStageScale, 0);
	constexpr float kEffectScale = 5.0f * kStageScale;
	constexpr float kDefaultEffectSpeed = 1.0f;
	constexpr float kTotalAnimTime = 1.0f;
	constexpr float kAnimSpeed = 0.4f;
}

TutorialEventWall::TutorialEventWall(
	std::weak_ptr<Physics> physics,
	const ObjectData& data,
	std::weak_ptr<ObjectHandleHolder> holder) :
	EventCollider(physics, data, holder, data.colData.isTrigger),
	_tutorialManager(),
	_isPlayAnim(false),
	_animTime(0.0f)
{
	// トリガー状態は不可視のまま
	if (IsTriggerMode()) {
		_isDraw = false;
	}
}

void TutorialEventWall::Init()
{
	EventCollider::Init();

	// 壁状態のみエフェクトを生成
	if (!IsTriggerMode()) {
		_effect = EffectManager::GetInstance().GenerateEffect(
			"Wall_magic_circle.efkefc", GetPos() + kEffectPosOffset);
		if (!_effect.expired()) {
			_effect.lock()->SetScale(Vector3(kEffectScale, kEffectScale, kEffectScale));
			_effect.lock()->SetPlaySpeed(kDefaultEffectSpeed);
			_effect.lock()->SetRot(_data.transData.rot);

			// 仮壁表示を切る
			_isDraw = false;
		}
	}
}

void TutorialEventWall::Update()
{
	// トリガー状態は何もしない
	if (IsTriggerMode()) return;
	// 開放演出中でなければ何もしない
	if (!_isPlayAnim) return;

	// アニメーションが終わっていれば
	if (_animTime <= 0.0f) {
		if (!_effect.expired()) {
			_effect.lock()->DeleteEffect();
		}
		EventCollider::Expire();
		return;
	}
	// アニメーション時間の更新
	_animTime -= kAnimSpeed / Statistics::kFPS;

	// エフェクトの更新
	if (!_effect.expired()) {
		float prog = _animTime / kTotalAnimTime;
		prog = Easing::Get(prog, EasingType::EaseOutBack);
		float scale = kEffectScale * prog;
		_effect.lock()->SetScale(Vector3(scale, scale, scale));
	}
}

void TutorialEventWall::OnCollide(const std::weak_ptr<Collider> collider)
{
	// 壁状態は反応しない
	if (!IsTriggerMode()) return;
	// 既に役目を終えていれば何もしない
	if (_isExpired) return;

	// プレイヤーと当たったらチュートリアルの進行を通知
	if (collider.lock()->GetTag() == PhysicsData::GameObjectTag::Player) {
		if (!_tutorialManager.expired()) {
			// 自身が保持するCallEventIdを通知
			_tutorialManager.lock()->OnTriggerWallTouched(_data.eventData.callEventId);
		}
		// 一度発火したら役割終了
		_isExpired = true;
	}
}

void TutorialEventWall::OnCall(int id, const std::string& type)
{
	// トリガー状態は通知を無視
	if (IsTriggerMode()) return;
	// チュートリアル完了通知以外は無視
	if (type != "TutorialComplete") return;
	// 自身のThisEventId宛てでなければ無視
	if (id != _data.eventData.thisEventId) return;

	// 開放演出開始
	Expire();
}

void TutorialEventWall::Expire()
{
	// トリガー状態は即座に削除対象にする
	if (IsTriggerMode()) {
		EventCollider::Expire();
		return;
	}

	// 壁状態は開放演出を開始する
	if (_isPlayAnim) return;
	_animTime = kTotalAnimTime;
	_isPlayAnim = true;
}
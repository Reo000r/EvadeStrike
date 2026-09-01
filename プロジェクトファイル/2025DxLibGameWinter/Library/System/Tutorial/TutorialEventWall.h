#pragma once
#include "Library/System/Event/EventCollider.h"

class EffekseerEffect;
class TutorialManager;

/// <summary>
/// <para> チュートリアル専用のイベント壁 </para>
/// <para> トリガーと壁になる </para>
/// </summary>
class TutorialEventWall : public EventCollider {
public:
	TutorialEventWall(
		std::weak_ptr<Physics> physics,
		const ObjectData& data,
		std::weak_ptr<ObjectHandleHolder> holder);

	void Init() override;
	void Update() override;

	void OnCollide(const std::weak_ptr<Collider> collider) override;

	/// <summary>
	/// <para> TutorialManagerからの通知 </para>
	/// <para> 壁状態のみ反応する </para>
	/// </summary>
	void OnCall(int id, const std::string& type) override;

	/// <summary>
	/// <para> 削除対象にする </para>
	/// <para> 壁状態では開放演出を開始する </para>
	/// </summary>
	void Expire() override;

	/// <summary>
	/// 通知先のTutorialManagerを設定する
	/// </summary>
	void SetTutorialManager(std::weak_ptr<TutorialManager> manager) { _tutorialManager = manager; }

	/// <summary>
	/// トリガー状態かどうか
	/// </summary>
	bool IsTriggerMode() const { return _data.colData.isTrigger; }

private:

	std::weak_ptr<TutorialManager> _tutorialManager;

	// 壁状態用
	std::weak_ptr<EffekseerEffect> _effect;
	bool _isPlayAnim;
	float _animTime;
};
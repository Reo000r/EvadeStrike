#pragma once
#include "Library/Objects/AnimationModel.h"
#include <memory>

class TimeScaleManager;

/// <summary>
/// ジャスト回避時の管理を行う
/// </summary>
class JustDodgeManager
{
public:
	enum class JustDodgeState {
		AllSlow,	// すべてスロー
		OtherSlow,	// プレイヤー以外スロー
		None,		// スローなし
	};

	JustDodgeManager();
	~JustDodgeManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// ジャスト回避演出を開始する
	/// </summary>
	void Start();

	/// <summary>
	/// ジャスト回避を行っているか
	/// </summary>
	/// <returns></returns>
	bool IsJustDodge() const;
	/// <summary>
	/// <para> ジャスト回避が行えるか </para>
	/// <para> 誰もスローでないなら可能 </para>
	/// </summary>
	/// <returns></returns>
	bool CanJustDodge() const;

private:
	float _timeCount;
	JustDodgeState _state;

	struct PlayerDummyDrawData {
		int handle = -1;
		AnimationModel::AnimData currentData;
		AnimationModel::AnimData prevData;
	};
};


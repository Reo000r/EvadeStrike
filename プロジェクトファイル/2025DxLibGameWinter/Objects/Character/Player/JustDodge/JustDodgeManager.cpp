#include "JustDodgeManager.h"
#include "Library/System/TimeScaleManager.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>

namespace {
	// それぞれのスロー時間
	constexpr float kPlayerSlowTime = 0.5f;
	constexpr float kPlayerDummyDrawDist = kPlayerSlowTime / 5.0f;
	constexpr float kOtherSlowTime = kPlayerSlowTime + 5.3f;

	constexpr float kDefaultScale = 1.0f;					// 通常倍率
	constexpr float kPlayerSlowScale = kDefaultScale * 0.2f;// スロー倍率(プレイヤー)
	constexpr float kOtherSlowScale = kDefaultScale * 0.1f;	// スロー倍率(その他)
	constexpr float kToSlowTransTime = 60.0f * 0.166f;		// スローに移行しきる時間
	constexpr float kToDefaultTransTime = 60.0f * 0.166f;	// 通常速度に移行しきる時間
}

JustDodgeManager::JustDodgeManager() :
	_timeCount(0.0f),
	_state(JustDodgeState::None)
{
}

JustDodgeManager::~JustDodgeManager()
{
}

void JustDodgeManager::Init()
{
}

void JustDodgeManager::Update()
{
	// 何も行っていなければreturn
	if (_state == JustDodgeState::None) {
		return;
	}

	const float decValue = (1.0f / 60.0f);
	float tempCount = _timeCount - decValue;	// 更新後の時間
	
	TimeScaleManager& manager = TimeScaleManager::GetInstance();

	// このタイミングで0を下回るなら
	if (_timeCount > 0.0f &&
		tempCount <= 0.0f) {
		// すべてスローなら
		if (_state == JustDodgeState::AllSlow) {
			// プレイヤーのタイムスケールを戻す
			manager.ChangePlayerScale(kDefaultScale, kToDefaultTransTime);
			// 時間とステートを更新
			_timeCount += kOtherSlowTime - kPlayerSlowTime;
			_state = JustDodgeState::OtherSlow;
		}
		// プレイヤー以外がスローなら
		else if (_state == JustDodgeState::OtherSlow) {
			// すべてのオブジェクトのタイムスケールを戻す
			manager.ChangeScale(kDefaultScale, kToDefaultTransTime);
			_state = JustDodgeState::None;
		}
	}

	// 時間更新
	_timeCount -= decValue;
	if (_timeCount < 0.0f) {
		_timeCount = 0.0f;
	}
}

void JustDodgeManager::Draw() const
{
	// プレイヤーのスケールによって画面効果を加える
	float currentPlayerScale = TimeScaleManager::GetInstance().GetOtherCurrentScale();
	float reverse = kDefaultScale - currentPlayerScale;
	float rate = reverse * 0.6f;
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x4b0082, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void JustDodgeManager::Start()
{
	_state = JustDodgeState::AllSlow;
	// オブジェクトのタイムスケールを下げる
	// プレイヤーは指定割合軽減する
	TimeScaleManager& manager = TimeScaleManager::GetInstance();
	manager.ChangePlayerScale(kPlayerSlowScale, kToSlowTransTime);
	manager.ChangeOtherScale(kOtherSlowScale, kToSlowTransTime);

	// プレイヤーのスロー時間をカウントする
	_timeCount = kPlayerSlowTime;
}

bool JustDodgeManager::IsJustDodge() const
{
	return (_state != JustDodgeState::None);
}

bool JustDodgeManager::CanJustDodge() const
{
	return !(IsJustDodge());
}


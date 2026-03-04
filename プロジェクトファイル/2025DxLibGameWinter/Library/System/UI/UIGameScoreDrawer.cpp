#include "UIGameScoreDrawer.h"
#include "Scene/ResultDataHolder.h"
#include "Library/System/Statistics.h"
#include "UIComboAction.h"
#include "UIComboScore.h"
#include "UIManager.h"
#include <DxLib.h>
#include <cassert>

namespace {
	constexpr int kMaxDigits = 5;   // 表示する桁数
	constexpr float kBaseScale = 0.5f * Statistics::kScreenWidthFullHDRatio;	// 大きさ
	// 描画開始基準X座標(1文字目の左端)
	constexpr int kStartScoreValueX = Statistics::kScreenWidth * 0.8f;
	// 描画開始基準Y座標(文字の中心)
	constexpr int kStartScoreValueY = Statistics::kScreenHeight * 0.1f;

	constexpr int kStartActionX = Statistics::kScreenWidth * 0.8f;
	constexpr int kStartActionY = Statistics::kScreenHeight * 0.2f;
	constexpr float kScaleAction = kBaseScale * 1.0f;
	constexpr float kDistAction = kScaleAction * 100.0f;

	constexpr int kStartScoreX = Statistics::kScreenWidth * 0.9f;
	constexpr int kStartScoreY = Statistics::kScreenHeight * 0.3f;
	//constexpr float kScaleScore = kBaseScale * 1.0f;
	constexpr float kDistScore = kScaleAction * (128.0f + 16.0f);
}

UIGameScoreDrawer::UIGameScoreDrawer() :
	UIScoreDrawer(Position2(0,0), true, LoadGraph(L"Data/Graph/EvadeStrike_Numbers.png")),
	_drawComboScoreBasePos(Position2(kStartScoreX, kStartScoreY)),
	_lastScoreListSize(),
	_lastComboList(),
	_drawComboActionUIList(),
	_drawComboScoreUIList(),
	_comboHolder()
{
}

void UIGameScoreDrawer::Init()
{
	ResetActionList();
}

void UIGameScoreDrawer::Update()
{
	// スコア取得
	_rawScore = ResultDataHolder::GetInstance().GetScore();

	UpdateComboScoreBasePos();
	UpdateCurrentAction();
	UpdateScoreValue();
}

void UIGameScoreDrawer::Draw(Vector2 shakeOffset)
{
	DrawScore(Position2(kStartScoreValueX, kStartScoreValueY) + shakeOffset, kMaxDigits, kBaseScale);
}

void UIGameScoreDrawer::SetPlayerComboHolder(std::weak_ptr<PlayerComboHolder> comboHolder)
{
	_comboHolder = comboHolder;
}

void UIGameScoreDrawer::AddDrawComboAction()
{
	int listSize = _lastComboList.size();
	if (listSize <= 0) return;
	PlayerActionKind action = _lastComboList[listSize - 1];

	int handle = GetActionGraph(action);
	if (handle <= 0) {
		assert(false && "不正なハンドルまたは不正なアクション種別");
		return;
	}

	Position2 drawPos = Position2(kStartActionX, kStartActionY);
	drawPos.x += kDistAction * _drawComboActionUIList.size();

	auto ui = std::make_shared<UIComboAction>(
		handle, drawPos, true,
		kScaleAction, 1.0f);
	UIManager::GetInstance().RegisterUI(ui);
	_drawComboActionUIList.emplace_back(ui);
}

void UIGameScoreDrawer::AddDrawComboScore()
{
	// 現在の攻撃を取得
	PlayerActionKind current = GetCurrentAction();

	int handle = GetActionGraph(current);
	if (handle <= 0) {
		assert(false && "不正なハンドルまたは不正なアクション種別");
		return;
	}

	Position2 drawPos = _drawComboScoreBasePos;
	drawPos.y += kDistScore * _drawComboScoreUIList.size();

	constexpr int kDrawScorePunch = 10;
	auto ui = std::make_shared<UIComboScore>(
		handle, drawPos, kDrawScorePunch);
	UIManager::GetInstance().RegisterUI(ui);
	_drawComboScoreUIList.emplace_back(ui);
}

void UIGameScoreDrawer::ResetActionList()
{
	// 待機状態を解除してから手放す
	for (const auto& ui : _drawComboActionUIList) {
		ui.lock()->SetStayState(false);
	}
	_drawComboActionUIList.clear();
}

void UIGameScoreDrawer::UpdateComboScoreBasePos()
{
	// 削除可能なUIを削除
	size_t count = std::erase_if(
		_drawComboScoreUIList,
		[](std::weak_ptr<UIBase> target) { return target.expired(); });

	Position2 defaultPos = Position2(kStartScoreX, kStartScoreY);
	// 要素が前回よりも減っていれば
	if (_drawComboScoreUIList.size() < _lastScoreListSize) {
		_drawComboScoreBasePos.y += kDistScore;
	}
	_lastScoreListSize = _drawComboScoreUIList.size();

	float offsetY = _drawComboScoreBasePos.y - defaultPos.y;
	float distOffset = 0.0f;
	// 初期位置に行き切っていなければ
	if (defaultPos.y + 1.0f <
		_drawComboScoreBasePos.y) distOffset = kDistScore;
	_drawComboScoreBasePos.y = defaultPos.y + offsetY * 0.9f;

	Position2 basePos = _drawComboScoreBasePos;
	for (const auto& ui : _drawComboScoreUIList) {
		ui.lock()->SetBasePos(basePos);
		basePos.y += kDistScore;
	}
}

void UIGameScoreDrawer::UpdateCurrentAction()
{
	// PlayerComboHolderから現在の攻撃を取得
	auto actionList = _comboHolder.lock()->GetActionList();
	auto currntAction = GetCurrentAction();
	// サイズが異なるかつ不正なアクション種別でないなら
	if (actionList.size() != _lastComboList.size() &&
		currntAction != PlayerActionKind::None) {
		_lastComboList = actionList;
		AddDrawComboAction();
	}
}

void UIGameScoreDrawer::UpdateScoreValue()
{
	// 追加スコア計算
	int addValue = _rawScore - _drawScore;
	// 更新がなければreturn
	if (addValue == 0) return;
	addValue *= 0.08f;
	// 1より小さければ1にする
	if (addValue <= 1) addValue = 1;
	// 描画スコア更新
	_drawScore += addValue;
}

PlayerActionKind UIGameScoreDrawer::GetCurrentAction()
{
	// PlayerComboHolderから現在の攻撃を取得
	auto actionList = _comboHolder.lock()->GetActionList();
	int size = actionList.size();
	// 攻撃がないなら登録しない
	if (size <= 0) return PlayerActionKind::None;
	return actionList[size-1];
}

int UIGameScoreDrawer::GetActionGraph(PlayerActionKind kind)
{
	int handle = -1;
	switch (kind) {
	case PlayerActionKind::Punch:
		handle = LoadGraph(L"Data/Graph/ComboPunch.png");
		break;
	case PlayerActionKind::Kick:
		handle = LoadGraph(L"Data/Graph/ComboKick.png");
		break;
	case PlayerActionKind::Special:
		handle = LoadGraph(L"Data/Graph/ComboSpecial.png");
		break;
	default:
		break;
	}

	if (handle <= 0) {
		assert(false && "不正なハンドルまたは不正なアクション種別");
	}
	return handle;
}

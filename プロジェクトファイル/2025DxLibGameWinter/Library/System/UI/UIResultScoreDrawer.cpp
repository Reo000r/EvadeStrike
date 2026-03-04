#include "UIResultScoreDrawer.h"
#include "Scene/ResultDataHolder.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>

namespace {
	constexpr int kMaxDigits = 5;   // 表示する桁数

	// 大きさ
	constexpr float kScale = 0.7f * Statistics::kScreenWidthFullHDRatio;
	constexpr int kStartScoreX = Statistics::kScreenWidth * 0.39f;
	constexpr int kStartScoreY = Statistics::kScreenHeight * 0.4f;
}

UIResultScoreDrawer::UIResultScoreDrawer() :
	UIScoreDrawer(Position2(0, 0), true, LoadGraph(L"Data/Graph/EvadeStrike_Numbers.png"))
{
}

void UIResultScoreDrawer::Init()
{
	_drawScore = ResultDataHolder::GetInstance().GetScore();
	_rawScore = _drawScore;
}

void UIResultScoreDrawer::Update()
{
	// 処理なし
}

void UIResultScoreDrawer::Draw(Vector2 shakeOffset)
{
	DrawScore(Position2(kStartScoreX, kStartScoreY) + shakeOffset, kMaxDigits, kScale);
}

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
	
	constexpr float kScoreAnimSpeed = 0.04f;
}

UIResultScoreDrawer::UIResultScoreDrawer() :
	UIScoreDrawer(Position2(0, 0), true, LoadGraph(L"Data/Graph/EvadeStrike_Numbers.png"))
{
}

void UIResultScoreDrawer::Init()
{
	_rawScore = ResultDataHolder::GetInstance().GetScore();
}

void UIResultScoreDrawer::Update()
{
	UpdateScoreValue();
}

void UIResultScoreDrawer::Draw(Vector2 shakeOffset)
{
	DrawScore(Position2(kStartScoreX, kStartScoreY) + shakeOffset, kMaxDigits, kScale);
}

void UIResultScoreDrawer::UpdateScoreValue()
{
	// 追加スコア計算
	float addValue = _rawScore - _drawScore;
	// 更新がなければreturn
	if (addValue == 0) return;
	addValue *= kScoreAnimSpeed;
	// 1より小さければ1にする
	//if (addValue <= 1) addValue = 1;
	// 描画スコア更新
	_drawScore += addValue;
}

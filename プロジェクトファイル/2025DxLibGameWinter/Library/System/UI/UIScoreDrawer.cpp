#include "UIScoreDrawer.h"
#include <DxLib.h>


UIScoreDrawer::UIScoreDrawer(Position2 centerPos, 
	bool isDraw, int scoreGraphHandle) :
	UIBase(centerPos, isDraw),
	_scoreGraphHandle(scoreGraphHandle),
	_rawScore(0),
	_drawScore(0)
{
}

UIScoreDrawer::~UIScoreDrawer()
{
	if (_scoreGraphHandle >= 0) DeleteGraph(_scoreGraphHandle);
}

void UIScoreDrawer::DrawScore(Position2 centerPos, int maxDigits, float scale)
{
	if (_scoreGraphHandle == -1) return;

	int allWidth, allHeight;
	GetGraphSize(_scoreGraphHandle, &allWidth, &allHeight);

	int charWidth = allWidth / 10;
	int charHeight = allHeight;

	// 実際に画面上で占める1文字の幅
	float displayWidth = charWidth * scale;

	for (int i = 0; i < maxDigits; ++i) {
		int powerOfTen = 1;
		for (int j = 0; j < (maxDigits - 1 - i); ++j) {
			powerOfTen *= 10;
		}
		int digit = (_drawScore / powerOfTen) % 10;

		int srcX = digit * charWidth;

		// startXを左端にするため、中心座標を「displayWidth / 2」分だけ右にずらして開始する
		float drawX = centerPos.x + (displayWidth / 2.0f) + (i * displayWidth);
		float drawY = centerPos.y;

		DrawRectRotaGraph(
			(int)drawX, (int)drawY,
			srcX, 0,
			charWidth, charHeight,
			(double)scale, 0.0,
			_scoreGraphHandle, TRUE
		);
	}
}

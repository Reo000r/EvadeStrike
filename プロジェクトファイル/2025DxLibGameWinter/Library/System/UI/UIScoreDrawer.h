#pragma once
#include "UIBase.h"

class UIScoreDrawer : public UIBase
{
public:
	UIScoreDrawer(Position2 centerPos, bool isDraw, int scoreGraphHandle);
	virtual ~UIScoreDrawer();

	void DrawScore(Position2 centerPos, int maxDigits, float scale);

protected:

	int _scoreGraphHandle;
	float _rawScore;
	float _drawScore;
};


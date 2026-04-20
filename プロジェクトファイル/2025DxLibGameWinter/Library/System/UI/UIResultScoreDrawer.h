#pragma once
#include "UIScoreDrawer.h"

class UIResultScoreDrawer : public UIScoreDrawer
{
public:
	UIResultScoreDrawer();
	~UIResultScoreDrawer();

	void Init();
	void Update();
	void Draw(Vector2 shakeOffset);

private:
	void UpdateScoreValue();
};


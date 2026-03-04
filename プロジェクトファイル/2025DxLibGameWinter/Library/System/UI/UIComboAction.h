#pragma once
#include "UIGraph.h"

class UIComboAction : public UIGraph
{
public:
	UIComboAction(int graphHandle, Position2 centerPos,
		bool isDraw, float scale, float alpha);
	~UIComboAction();

	void Update() override;

	void SetStayState(bool isStay) { _isStay = isStay; }

private:

	using UpdateFunc_t = void (UIComboAction::*)();
	UpdateFunc_t _nowUpdateState = nullptr;

	void UpdateIn();
	void UpdateStay();
	void UpdateOut();

	/// <summary>
	/// アニメーション更新
	/// </summary>
	void UpdateAnimScale(float progScale);

	bool _isStay;

	// アニメーション進行度
	float _scaleAnimProgress;
	float _progScale;
};


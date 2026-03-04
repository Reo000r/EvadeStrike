#pragma once
#include "UIBase.h"

class UIComboScore : public UIBase
{
public:
	UIComboScore(int graphHandle, Position2 centerPos, int drawScore);
	~UIComboScore();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Vector2 shakeOffset) override;

	void SetBasePos(Vector2 basePos) { _basePos = basePos; }

private:

	using UpdateFunc_t = void (UIComboScore::*)();
	UpdateFunc_t _nowUpdateState = nullptr;

	void UpdateIn();
	void UpdateStay();
	void UpdateOut();

	/// <summary>
	/// アニメーション更新
	/// </summary>
	void UpdateAnimMove(float progScale);
	/// <summary>
	/// アニメーション更新
	/// </summary>
	void UpdateAnimScale(float progScale);

	int _graphHandle;
	int _drawScore;

	float _scale;
	Vector2 _basePos;
	Vector2 _posOffset;

	// アニメーション進行度
	int _drawFrame;
	float _moveAnimProgress;
	float _scaleAnimProgress;
	float _progScale;
};


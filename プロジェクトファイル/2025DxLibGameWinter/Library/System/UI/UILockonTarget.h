#pragma once
#include "UIBase.h"

class UILockonTarget : public UIBase
{
public:
	UILockonTarget(int graphHandle, Position2 centerPos, float scale);
	~UILockonTarget();

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

	void SetDrawState(bool drawState) override;

	int GetGraphHandle() const { return _graphHandle; }
	float GetAnimPercent() const;
	void SetPercent(float percent);
	void SetScale(float scale) { _baseScale = scale; }

private:

	// アニメーション更新
	void UpdateAnim();
	void UpdateAnimDec();
	void UpdateAnimDraw();

	float CalcDecAnimProg() const;
	float CalcCurrentDrawAnimProg() const;
	float CalcCurrentScale() const;
	float CalcCurrentAlpha() const;

	int _graphHandle;
	float _percent;
	float _baseScale;

	// アニメーション
	float _animStartPercent;  // アニメーション開始時の値
	int _decAnimFrame;        // 残りフレーム数

	int _drawAnimFrame;
};


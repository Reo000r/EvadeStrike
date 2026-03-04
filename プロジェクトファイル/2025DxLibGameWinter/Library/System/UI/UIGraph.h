#pragma once
#include "UIBase.h"
#include <memory>

class UIGraph : public UIBase
{
public:

	UIGraph(int graphHandle, Position2 centerPos, 
		bool isDraw, float scale, float alpha, bool isShake = true);
	~UIGraph();

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

	int GetGraphHandle() const { return _graphHandle; }
	void SetScale(float scale) { _scale = scale; }

private:

	int _graphHandle;
	float _scale;
	float _alpha;
};
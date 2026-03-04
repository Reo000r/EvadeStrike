#include "UIGraph.h"
#include <DxLib.h>

UIGraph::UIGraph(int graphHandle, Position2 centerPos, 
	bool isDraw, float scale, float alpha, bool isShake) :
	UIBase(centerPos, isDraw, isShake),
	_graphHandle(graphHandle),
	_scale(scale),
	_alpha(alpha)
{
}

UIGraph::~UIGraph()
{
	if (_graphHandle >= 0) {
		DeleteGraph(_graphHandle);
		_graphHandle = -1;
	}
}

void UIGraph::Init()
{
}

void UIGraph::Update()
{
}

void UIGraph::Draw(Vector2 shakeOffset)
{
	if (!_isDraw) return;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * _alpha));
	int w, h;
	GetGraphSize(_graphHandle, &w, &h);
	int drawPosX = _centerPos.x + shakeOffset.x;
	int drawPosY = _centerPos.y + shakeOffset.y;
	DrawRectRotaGraph(
		drawPosX, drawPosY, // 画面上の描画中心
		0, 0,			// 元画像の左上座標
		w, h,			// 元画像の幅と高さ
		_scale,			// 拡大率
		0.0,			// 回転（ラジアン）
		_graphHandle,	// グラフィックハンドル
		TRUE,			// 透過フラグ
		FALSE			// 反転フラグ
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

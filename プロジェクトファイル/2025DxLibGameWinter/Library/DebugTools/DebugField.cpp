#include "DebugField.h"
#include "Library/Geometry/Vector3.h"
#include <DxLib.h>

namespace {
	constexpr int kDivNum = 20;
	constexpr int kMaxPos = 8000;
}

DebugField::DebugField()
{
}

DebugField::~DebugField()
{
}

void DebugField::Draw()
{
	// 浮かせる量
	constexpr int kUpperY = 3;
	Position3 start = Position3(-kMaxPos, kUpperY, -kMaxPos);
	Position3 end   = Position3( kMaxPos, kUpperY, -kMaxPos);
	const int add = kMaxPos * 2 / kDivNum;
	unsigned int col = 0xff2222;
	// x
	for (int i = 0; i < kDivNum + 1; ++i) {
		DrawLine3D(start, end, col);
		start.z += add;
		end.z = start.z;
	}
	start = Position3(-kMaxPos, kUpperY, -kMaxPos);
	end = Position3(-kMaxPos, kUpperY, kMaxPos);
	col = 0x2222ff;
	for (int i = 0; i < kDivNum + 1; ++i) {
		DrawLine3D(start, end, col);
		start.x += add;
		end.x = start.x;
	}

    DrawPolygon();
}

void DebugField::DrawPolygon()
{
    VERTEX3D Vertex[6];
	int polygonNum = 2;
	int grHandle = DX_NONE_GRAPH;
	int transFlag = false;

    for (int i = 0; i < 6; ++i) {
        Vertex[i].norm = Vector3(0, 1, 0);
        Vertex[i].dif = GetColorU8(255, 255, 255, 255);
        Vertex[i].spc = GetColorU8(0, 0, 0, 0);
        Vertex[i].u = 0.0f;
        Vertex[i].v = 0.0f;
        Vertex[i].su = 0.0f;
        Vertex[i].sv = 0.0f;
    }

	constexpr int kDrawMax = kMaxPos * 1;
    Position3 upRight = Position3(-kDrawMax, 0, kDrawMax);
    Position3 upLeft = Position3(kDrawMax, 0, kDrawMax);
    Position3 downRight = Position3(-kDrawMax, 0, -kDrawMax);
    Position3 downLeft = Position3(kDrawMax, 0, -kDrawMax);

    // ２ポリゴン分の頂点のデータをセット
    Vertex[0].pos = upRight;
    Vertex[1].pos = upLeft;
    Vertex[2].pos = downRight;

    Vertex[3].pos = upLeft;
    Vertex[4].pos = downLeft;
    Vertex[5].pos = downRight;

    DrawPolygon3D(Vertex, polygonNum, grHandle, transFlag);
}

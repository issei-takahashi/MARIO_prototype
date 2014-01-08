#pragma once

#include "includes.hpp"
#include <DxLib.h>

namespace MyDxLib
{
	void DrawRectLine(int x1, int y1, int x2, int y2, int color, int thick)
	{
		DxLib::DrawLine(x1,y1,x1,y2,color,thick);
		DxLib::DrawLine(x1,y2,x2,y2,color,thick);
		DxLib::DrawLine(x2,y2,x2,y1,color,thick);
		DxLib::DrawLine(x2,y1,x1,y1,color,thick);
	}
	void DrawCross(int x, int y, int color, int thick, int len)
	{
		DxLib::DrawLine(x-len,y,x+len,y,color,thick);
		DxLib::DrawLine(x,y-len,x,y+len,color,thick);
	}
};
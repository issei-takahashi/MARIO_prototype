#include "Block.hpp"
#include "Camera.hpp"
#include <DxLib.h>

Block::Block()
{

}

int Block::libDraw()
{
	static Point<int> startP, endP;
	startP = Camera::getDrawPos(this->p1);
	endP   = Camera::getDrawPos(this->p2);
	int col = DxLib::GetColor(255,0,0);
	return DxLib::DrawBox( startP.x, startP.y, endP.x, endP.y, col, true );
}
#pragma once
#include "includes.hpp"

namespace GameLib
{
	// ���C�u����������
	int initLib();
	// ���C�u�����I��
	int endLib();

	namespace draw{
		int DrawCircle( int _x, int _y, int _r, int _col, int _fillFlag );
		int	ScreenFlip() ;
	}
};

#pragma once

#include "Point.hpp"

class macro{
public:
	static int const X_SCREEN_SIZE_DEFAULT;
	static int const Y_SCREEN_SIZE_DEFAULT;
	static Point<int> const SCREEN_CENTER;
	static double const PI;
	static double const EPS;
	static bool const DBG_FLAG;
	static int const KEY_INPUT_FRAME_MAX;
	static int const WRONG_INT;
};


#define foreach(it,container) for(auto (it)=(container).begin();(it)!=(container).end();(it)++)

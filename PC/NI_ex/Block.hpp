#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Observer.hpp"

class Block : public Rect<int> //, public Observer
{
interface__:
	Block();
	int libDraw();
open__:
};
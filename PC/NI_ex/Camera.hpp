#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include "IO.hpp"
#include "GameLib.hpp"

class Camera
{
interface__:
	// フィールド座標を描画座標に変換
	template<class T>
	static Point<T> getDrawPos( Point<T> const & p )
	{
		IO::mutex.lock();
		static int const SCR_W = IO::getConst("SCR_W");
		static int const SCR_H = IO::getConst("SCR_H");
		IO::mutex.unlock();

		double zoom                = IO::get_zoom();
		Point<int> stP             = IO::get_cameraPos();
		static Point<T> retP;
		retP.x = (T)((T)SCR_W/2 + (p.x-(T)stP.x)*zoom);
		retP.y = (T)((T)SCR_H/2 + (p.y-(T)stP.y)*zoom);
		return retP;
	}
};
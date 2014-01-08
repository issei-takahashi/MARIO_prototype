#include "Timer.hpp"
#include "utils.hpp"
#include <DxLib.h>

//static変数の初期化
double Timer::lastTime;
double Timer::frameTime;
int Timer::frameNum;

string Timer::LocalTime::getLocalTimeString() const
{
	return utils::i2s(this->year) + "_" + utils::i2s(this->month) + "_" + utils::i2s(this->day) + "_" + utils::i2s(this->hour) + "_" + utils::i2s(this->min) + "_" + utils::i2s(this->sec);
}


Timer::LocalTime Timer::getLocalTime()
{
     time_t now = time(NULL);
     struct tm *pnow = localtime(&now);
	 Timer::LocalTime ret;

	 ret.year = pnow->tm_year + 1900;
	 ret.month = pnow->tm_mon + 1;
	 ret.day = pnow->tm_mday;

	 ret.hour = pnow->tm_hour;
	 ret.min = pnow->tm_min;
	 ret.sec = pnow->tm_sec;
	 
	 return ret;
}

 //最終呼び出し時間の記録
void Timer::setLastTime()
{
	lastTime = (double)(DxLib::GetNowCount()/1000.0);
	frameNum++;
}

//1フレームにかかった時間を計測
void Timer::setFrameTime()
{
	// 最新のフレーム時間を記録
	frameTime = (double)(DxLib::GetNowCount()/1000.0) - lastTime;
}

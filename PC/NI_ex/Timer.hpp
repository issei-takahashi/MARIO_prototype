#pragma once

#include "includes.hpp"

/*------------------- Class Timer ---------------------------

目的：現在時刻（ミリ秒）の計測
属性：モノステート

--------------------------------------------------------*/

class Timer
{
	friend class GameMain;
open__:
	class LocalTime
	{
		friend class boost::serialization::access;
	public:
		string getLocalTimeString() const;
		int year;  // 年
		int month; // 月
		int day;   // 日
		int hour;  // 時
		int min;   // 分
		int sec;   // 秒
	private:
		//シリアライズ用
		template<class Archive>
		void serialize( Archive& ar, unsigned int ver )
		{
			ar & boost::serialization::make_nvp("year", this->year);
			ar & boost::serialization::make_nvp("month",this->month );
			ar & boost::serialization::make_nvp("day", this->day );
			ar & boost::serialization::make_nvp("hour",this->hour );
			ar & boost::serialization::make_nvp("min",this->min );
			ar & boost::serialization::make_nvp("sec",this->sec );
		}
	} localTime ;
interface__:
	// 絶対時間系get
	static Timer::LocalTime getLocalTime();

	// フレーム系get
	static double GetLastTime(){return lastTime;}
	static double GetFrameTime(){return frameTime;}
	static int    GetFrameNum(){ return frameNum; }

inner__:
	static double lastTime; //直前の計測時間(s)
	static double frameTime; //1フレームにかかった時間(s)
	static int frameNum; //　ゲーム起動時からのフレーム数

inner__:  //GameMainのみから呼び出し
	static void setLastTime(); //最終計測時間の測定
	static void setFrameTime(); //1フレームにかかった時間の測定
	static void setStartTime(); //計測開始時間の測定
};
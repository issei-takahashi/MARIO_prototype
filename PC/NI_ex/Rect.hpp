#pragma once
#include "includes.hpp"
#include "Point.hpp"


#define LOC_LEFT  (1)
#define LOC_RIGHT (2)
#define LOC_UP    (4)
#define LOC_DOWN  (8)
#define LOC_LRHIT (16)
#define LOC_UDHIT (32) 
#define LOC_UP_FIT   (64)
#define LOC_DOWN_FIT (128)
#define LOC_LEFT_FIT (256)
#define LOC_RIGHT_FIT (512)


template<class T>
class Rect
{
	friend class boost::serialization::access;
interface__:
	Rect()
	{

	}
	Rect( T x1, T y1, T x2, T y2 )
		:p1(Point<T>(x1,y1)),p2(Point<T>(x2,y2))
	{

	}

	T getWidth() const 
	{
		return this->p2.x-this->p1.x;
	}

	// 別のRectに対して「自分が」どういう状態にあるか
	int Rect::getLocationType( Rect<T> const & that ) const
	{
		int ret = 0;
		
		/* 上下方向 */
		// 自分のほうが上
		if( this->p2.y < that.p1.y ){
			ret |= LOC_UP;
		}
		// 自分のほうが上で接してる
		ef( this->p2.y == that.p1.y ){
			ret |= LOC_UP_FIT;
		}
		// 自分のほうが下
		ef( that.p2.y < this->p1.y ){
			ret |= LOC_DOWN;
		}
		// 自分のほうがで接してる
		ef( that.p2.y == this->p1.y ){
			ret |= LOC_DOWN_FIT;
		}
		// あたっている
		el{
			ret |= LOC_UDHIT;
		}

		/* 左右方向 */
		// 自分のほうが左
		if( this->p2.x < that.p1.x ){
			ret |= LOC_LEFT;
		}
		// 自分のほうが左で接してる
		ef( this->p2.x == that.p1.x ){
			ret |= LOC_LEFT_FIT;
		}
		// 自分のほうが右
		ef( that.p2.x < this->p1.x ){
			ret |= LOC_RIGHT;
		}
		// 自分のほうが右で接してる
		ef( that.p2.x == this->p1.x ){
			ret |= LOC_RIGHT_FIT;
		}
		el{
			ret |= LOC_LRHIT;
		}
		//cout << endl;
		return ret;
	}
open__:
	Point<T> p1,p2; // 左上の点と右下の点

inner__:
	//シリアライズ用
	template<class Archive>
	void serialize( Archive& ar, unsigned int ver )
	{
		ar & boost::serialization::make_nvp("p1", this->p1);
		ar & boost::serialization::make_nvp("p2", this->p2);
	}
};
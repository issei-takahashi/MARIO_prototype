#pragma once
#include <boost/serialization/serialization.hpp>

/*---------------------- Class Point----------------------------

目的：2次元座標の汎用クラス

属性：テンプレート

--------------------------------------------------------------------------*/

template <class T>
class Point
{
	friend class boost::serialization::access;

	public:
		//座標値
		T x,y;

		//シリアライズ用
		template<class Archive>
		void serialize( Archive& ar, unsigned int ver )
		{
			ar & boost::serialization::make_nvp("x", this->x);
			ar & boost::serialization::make_nvp("y", this->y);
		}

		double distance( Point<T> const & p ) const 
		{
			return sqrt( powf( (float)( this->x - p.x ), 2 ) + powf( (float)( this->y - p.y ), 2 ) );
		}

		//コンストラクタ
		Point()
		{
			x = 0;
			y = 0;
		}
      
		//コンストラクタ
		Point(const T& t1, const T& t2)
		{
			x = t1;
			y = t2;
		}

		//コンストラクタ
		Point(double const t1, double const t2)
		{
			x = (const T)t1;
			y = (const T)t2;
		}

		// +演算子オーバーロード
		Point<T> operator+(const Point<T>& obj) const
		{
			Point tmp;
			tmp.x = x + obj.x;
			tmp.y = y + obj.y;
			return tmp;
		}


		// -演算子オーバーロード
		Point<T> operator-(const Point<T>& obj) const
		{
			Point tmp;
			tmp.x = x - obj.x;
			tmp.y = y - obj.y;
			return tmp;
		}

		// *演算子オーバーロード
		Point<T> operator*(const double& d) const
		{
			Point tmp;
			tmp.x = (T)(this->x * d);
			tmp.y = (T)(this->y * d);
			return tmp;
		}

		// /演算子オーバーロード
		Point<T> operator/(const double& d) const
		{
			Point tmp;
			tmp.x = (T)(this->x / d);
			tmp.y = (T)(this->y / d);
			return tmp;
		}

		// +=演算子オーバーロード
		Point<T>& operator+=(const Point<T>& obj)
		{
			x += obj.x;
			y += obj.y;
			return *this;
		}

		// -=演算子オーバーロード
		Point<T>& operator-=(const Point<T>& obj)
		{
			x -= obj.x;
			y -= obj.y;
			return *this;
		}

		// *=演算子オーバーロード
		Point<T>& operator*=(const double d)
		{
			(T)(this->x *= d);
			(T)(this->y *= d);
			return *this;
		}
		
		// /=演算子オーバーロード
		Point<T>& operator/=(const double d)
		{
			(T)(this->x /= d);
			(T)(this->y /= d);
			return *this;
		}

		// =演算子オーバーロード
		Point<T>& operator=(const Point<T>& obj)
		{
			x = obj.x;
			y = obj.y;
			return *this;
		}

		// ==演算子オーバーロード
		bool operator==(const Point<T>& obj) const
		{
			if( x == obj.x && y == obj.y)
				return true;
			else
				return false;
		}
		
		// !=演算子オーバーロード
		bool operator!=(const Point<T>& obj) const
		{
			if( x != obj.x || y != obj.y)
				return true;
			else
				return false;
		}

		// <演算子オーバーロード
		bool operator<(const Point<T>& obj) const
		{
			if( x*100 + y < obj.x*100 + obj.y )
				return true;
			else
				return false;
		}



};
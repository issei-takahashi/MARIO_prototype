#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include <opencv2\opencv.hpp>

class utils{
public:
	//stringをdoubleに変換
	static double s2d(const string& str)
	{
		double rt;
		stringstream ss;
		ss << str;
		ss >> rt;
		return rt;
	}

	//stringをintに変換
	static int s2i(const string& str)
	{
		int rt;
		stringstream ss;
		ss << str;
		ss >> rt;
		return rt;
	}

	//doubleをstringに変換
	static string d2s(double d)
	{
		string rt;
		stringstream ss;
		ss << d;
		ss >> rt;
		return rt;
	}

	//intをstringに変換
	static string i2s(int d)
	{
		string rt;
		stringstream ss;
		ss << d;
		ss >> rt;
		return rt;
	}

	// 2^n < var となる最大のn
	static int lowerPow( int var, int fund )
	{
		assert(fund>1);
		int tmp = 1;
		int n = 0;
		while( tmp < var ){
			tmp *= fund;
			n++;
		}
		return n-1;
	}

	static int upperPow( int var, int fund )
	{
		return 1 + lowerPow( var, fund );
	}

	//ワイド文字列からマルチバイト文字列
	//ロケール依存
	static void wstring2string(const std::wstring &src, std::string &dest) {
		char *mbs = new char[src.length() * MB_CUR_MAX + 1];
		wcstombs(mbs, src.c_str(), src.length() * MB_CUR_MAX + 1);
		dest = mbs;
		delete [] mbs;
	}

	//マルチバイト文字列からワイド文字列
	//ロケール依存
	static void string2wstring(const std::string &src, std::wstring &dest) {
		wchar_t *wcs = new wchar_t[src.length() + 1];
		mbstowcs(wcs, src.c_str(), src.length() + 1);
		dest = wcs;
		delete [] wcs;
	}

	// ワイド文字からマルチバイト文字
	static TCHAR wchar_t2TCHAR( wchar_t wchar )
	{
		wstring wstr;
		wstr = wchar;
		string str;
		wstring2string(wstr,str);
		return str.at(0);
	}

	// マルチバイト文字からワイド文字
	static wchar_t TCHAR2w_char( TCHAR tchar )
	{
		string str;
		str = tchar;
		wstring wstr;
		string2wstring(str,wstr);
		return wstr.at(0);
	}

	// Point<int> から cv::Point に
	static cv::Point cvPoint( Point<int> const & p )
	{
		return cv::Point( p.x, p.y );
	}

	// csvの1行をセルごとにカット
	static void cutLine( string _line, vector<string>& _dst )
	{
		_dst.clear();
		if( _line == "" || _line.find("//")==0 || _line.find(",")==0 ){
			return;
		}
		// タブとスペースとクォーテーションマークをカット
		boost::algorithm::replace_all( _line, " ", "" );
		boost::algorithm::replace_all( _line, "\t", "" );
		boost::algorithm::replace_all( _line, "\"", "" );
		// 文字列_lineを","で区切って_dstに格納
		boost::algorithm::split( _dst, _line,  boost::is_any_of(",") );
	}
};
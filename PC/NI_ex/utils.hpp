#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include <opencv2\opencv.hpp>

class utils{
public:
	//string��double�ɕϊ�
	static double s2d(const string& str)
	{
		double rt;
		stringstream ss;
		ss << str;
		ss >> rt;
		return rt;
	}

	//string��int�ɕϊ�
	static int s2i(const string& str)
	{
		int rt;
		stringstream ss;
		ss << str;
		ss >> rt;
		return rt;
	}

	//double��string�ɕϊ�
	static string d2s(double d)
	{
		string rt;
		stringstream ss;
		ss << d;
		ss >> rt;
		return rt;
	}

	//int��string�ɕϊ�
	static string i2s(int d)
	{
		string rt;
		stringstream ss;
		ss << d;
		ss >> rt;
		return rt;
	}

	// 2^n < var �ƂȂ�ő��n
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

	//���C�h�����񂩂�}���`�o�C�g������
	//���P�[���ˑ�
	static void wstring2string(const std::wstring &src, std::string &dest) {
		char *mbs = new char[src.length() * MB_CUR_MAX + 1];
		wcstombs(mbs, src.c_str(), src.length() * MB_CUR_MAX + 1);
		dest = mbs;
		delete [] mbs;
	}

	//�}���`�o�C�g�����񂩂烏�C�h������
	//���P�[���ˑ�
	static void string2wstring(const std::string &src, std::wstring &dest) {
		wchar_t *wcs = new wchar_t[src.length() + 1];
		mbstowcs(wcs, src.c_str(), src.length() + 1);
		dest = wcs;
		delete [] wcs;
	}

	// ���C�h��������}���`�o�C�g����
	static TCHAR wchar_t2TCHAR( wchar_t wchar )
	{
		wstring wstr;
		wstr = wchar;
		string str;
		wstring2string(wstr,str);
		return str.at(0);
	}

	// �}���`�o�C�g�������烏�C�h����
	static wchar_t TCHAR2w_char( TCHAR tchar )
	{
		string str;
		str = tchar;
		wstring wstr;
		string2wstring(str,wstr);
		return wstr.at(0);
	}

	// Point<int> ���� cv::Point ��
	static cv::Point cvPoint( Point<int> const & p )
	{
		return cv::Point( p.x, p.y );
	}

	// csv��1�s���Z�����ƂɃJ�b�g
	static void cutLine( string _line, vector<string>& _dst )
	{
		_dst.clear();
		if( _line == "" || _line.find("//")==0 || _line.find(",")==0 ){
			return;
		}
		// �^�u�ƃX�y�[�X�ƃN�H�[�e�[�V�����}�[�N���J�b�g
		boost::algorithm::replace_all( _line, " ", "" );
		boost::algorithm::replace_all( _line, "\t", "" );
		boost::algorithm::replace_all( _line, "\"", "" );
		// ������_line��","�ŋ�؂���_dst�Ɋi�[
		boost::algorithm::split( _dst, _line,  boost::is_any_of(",") );
	}
};
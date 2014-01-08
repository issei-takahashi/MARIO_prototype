#pragma once
#include "includes.hpp"
#include <opencv2\opencv.hpp>

namespace mycv
{
	void depthFrame2Mat( int rows, int cols, int type, void* data, cv::Mat* pDst );
	
	template<class T>
	int at_ifOutOgRange_zero(cv::Mat const & mat,int y, int x)
	{
		if( mat.rows <= y ){
			return 0;
		}
		ef( mat.cols <= x ){
			return 0;
		}
		ef( y < 0 ){
			return 0;
		}
		ef( x < 0 ){
			return 0;
		}
		el{
			return (int)mat.at<T>(y,x);
		}
	}

};
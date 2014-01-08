#pragma once
#include "includes.hpp"
#include "DrawBuffer.hpp"
#include "Point.hpp"

class Image : public DrawBuffer::DrawInterface
{
interface__:
	Image( string const& _imgFileName, int _totalNum, int _imgNumX, int _imgNumY, int _imgSizeX, int _imgSizeY, int _drawInterval, int _loopStart, int _zanzouCount=0, int _zanzouFrame=0 );
	void libDraw() ; // カメラ位置を考慮して描画
	void simpleDraw(); // centerを中心として描画
	void loadImage();
	void setCenter(Point<int> _center);
	void initIterator();

inner__:
	void updateIterator();

inner__:
	string const imgFileName;
	int const totalNum;
	int const imgNumX;
	int const imgNumY;
	int const imgSizeX;
	int const imgSizeY;
	int const drawInterval;
	int const loopStart;
	vector<int> v_handle;
	
	int index_v_handle;
	bool isLoaded;
	Point<int> center;
	int drawCount;
	int totalFrameCount;

	int zanzouCount;
	int zanzouFrame;
	list< pair< pair< Point<int>, Point<int> >, int > > l_pa_pa_zanzouPoint;

outofuse__:
	Image() : imgFileName(), totalNum(), imgNumX(), imgNumY(), imgSizeX(), imgSizeY(), drawInterval(), loopStart() {}
};
#include "DrawBuffer.hpp"
#include "Camera.hpp"
#include <DxLib.h>

#define HERE DrawBuffer

map<int,HERE::DrawInterface*> HERE::m_inter;

// バッファを空に（全命令をクリア）
void HERE::clearBuffer()
{
	HERE::m_inter.clear();
}

// 描画開始命令
void HERE::drawStartCall( int _layer, DrawInterface* _inter )
{
	MINSERT(HERE::m_inter,_layer,_inter);
}

// 描画停止命令
void HERE::drawStopCall( int _layer, DrawInterface* _inter )
{
	MERASE(HERE::m_inter,_layer);
}

// バッファにたまっているものを全部描画
void HERE::drawAll()
{
	foreach(it,HERE::m_inter){
		it->second->libDraw();
	}
}

MovieImage::MovieImage( string const & _dir, string const & _fileName, int _intervalFrame, double _xEx, double _yEx )
	:fileDir(_dir),fileName(_fileName),intervalFrame(_intervalFrame),
	frameCount(0),
	handle( DxLib::LoadGraph( (_dir+_fileName).c_str() ) ),
	xExRate(_xEx), yExRate(_yEx)
{
	assert(this->handle!=-1);
	DxLib::GetGraphSize(this->handle,&this->xSize,&this->ySize);
	DxLib::PlayMovieToGraph( this->handle ) ;
}

void MovieImage::libDraw()
{
	this->frameCount++;
	if( this->frameCount >= this->intervalFrame ){
		Point<int> p1,p2,dp1,dp2;
		p1 = this->position;
		p2 = this->position;
		p1.x -= this->xSize*this->xExRate;
		p1.y -= this->ySize*this->yExRate;
		p2.x += this->xSize*this->xExRate;
		p2.y += this->ySize*this->yExRate;
		dp1 = Camera::getDrawPos(p1);
		dp2 = Camera::getDrawPos(p2);
		DxLib::DrawExtendGraph( dp1.x, dp1.y, dp2.x, dp2.y, this->handle , FALSE );
		if( DxLib::GetMovieStateToGraph( this->handle )  == 0 ){
			DxLib::SeekMovieToGraph( this->handle , 0.00 ) ;
			DxLib::PlayMovieToGraph( this->handle ) ;
		}
		this->frameCount = 0;
	}
}
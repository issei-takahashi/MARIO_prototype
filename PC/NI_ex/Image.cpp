#include "Image.hpp"
#include "IO.hpp"
#include "Camera.hpp"
#include <DxLib.h>

Image::Image( string const& _imgFileName, int _totalNum, int _imgNumX, int _imgNumY, int _imgSizeX, int _imgSizeY, int _drawInterval, int _loopStart, int _zanzouCount, int _zanzouFrame )
	:imgFileName(_imgFileName), totalNum(_totalNum), imgNumX(_imgNumX), imgNumY(_imgNumY), imgSizeX(_imgSizeX), imgSizeY(_imgSizeY), drawInterval(_drawInterval), loopStart(_loopStart) 
	,isLoaded(false), drawCount(0), zanzouCount(_zanzouCount), zanzouFrame(_zanzouFrame), totalFrameCount(0)
{
	assert(this->loopStart<this->totalNum);
}

void Image::libDraw() 
{
	Point<int> p1, p2;
	p1.x = (this->center.x-this->imgSizeX/2);
	p1.y = (this->center.y-this->imgSizeY/2);
	p2.x = (this->center.x+this->imgSizeX/2);
	p2.y = (this->center.y+this->imgSizeY/2);
	Point<int> dp1, dp2;
	dp1 = Camera::getDrawPos(p1);
	dp2 = Camera::getDrawPos(p2);
	int handle = this->v_handle.at(this->index_v_handle);
	DxLib::DrawExtendGraph(dp1.x,dp1.y,dp2.x,dp2.y,handle,true);
	
	if( this->zanzouCount ){
		if( (++this->totalFrameCount)%this->zanzouCount == 0 ){
			this->l_pa_pa_zanzouPoint.push_back( make_pair( make_pair(dp1,dp2), handle ) );
			if( this->l_pa_pa_zanzouPoint.size() > this->zanzouCount ){
				this->l_pa_pa_zanzouPoint.pop_front();
			}
		}
		int i=1;
		foreach(it,this->l_pa_pa_zanzouPoint){
			DxLib::SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 * ( (double)i/this->l_pa_pa_zanzouPoint.size() ) );
			DxLib::DrawExtendGraph( it->first.first.x,  it->first.first.y,  it->first.second.x,  it->first.second.y,  it->second,  true );
			DxLib::SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 255 );
			i++;
		}
	}
	this->updateIterator();
}

void Image::simpleDraw()
{
	int handle = this->v_handle.at(this->index_v_handle);
	DxLib::DrawRotaGraph(this->center.x,this->center.y,1.0,0,handle,1);
	
	if( this->zanzouCount ){
		if( (++this->totalFrameCount)%this->zanzouCount == 0 ){
			this->l_pa_pa_zanzouPoint.push_back( make_pair( make_pair(this->center,this->center), handle ) );
			if( this->l_pa_pa_zanzouPoint.size() > this->zanzouCount ){
				this->l_pa_pa_zanzouPoint.pop_front();
			}
		}
		int i=1;
		foreach(it,this->l_pa_pa_zanzouPoint){
			DxLib::SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 * ( (double)i/this->l_pa_pa_zanzouPoint.size() ) );
			DxLib::DrawRotaGraph(it->first.first.x,it->first.first.y,1.0,0,it->second,1);
			DxLib::SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 255 );
			i++;
		}
	}
	this->updateIterator();
}

void Image::loadImage()
{
	if( this->isLoaded == false ){
		int* handleBuf = new int[this->totalNum];
		DxLib::LoadDivGraph(this->imgFileName.c_str(),this->totalNum,this->imgNumX,this->imgNumY,this->imgSizeX,this->imgSizeY,handleBuf);
		for(int i=0;i<this->totalNum;i++){
			this->v_handle.push_back(handleBuf[i]);
		}
		this->index_v_handle=0;
		delete[] handleBuf;
		this->isLoaded = true;
	}
}

void Image::setCenter(Point<int> _center)
{
	this->center = _center;
}

void Image::initIterator()
{
	this->index_v_handle=0;
}

void Image::updateIterator()
{
	if(this->drawCount>this->drawInterval){
		this->index_v_handle++;
		if(this->index_v_handle==this->v_handle.size()){
			this->index_v_handle=this->loopStart;
		}
		this->drawCount = 0;
	}
	else{
		this->drawCount++;
	}
}


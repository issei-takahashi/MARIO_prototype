#include "RealField.hpp"
#include "IO.hpp"
#include "FT.hpp"
#include "utils.hpp"
#include "GameMain.hpp"
#include "mycv.hpp"
#include "macro.hpp"
#include "Main.hpp"

#define HERE RealField
#define DEPTH_WINDOW_NAME "Depth Image"

/* Kinectからとったもの */
cv::Mat                       HERE::depthImage;
cv::Mat                       HERE::colorImage;
/* カメラ関連 */
openni::VideoStream           HERE::depthStream;
vector<openni::VideoStream*>  HERE::v_pDepthStream;
openni::VideoStream           HERE::colorStream;
vector<openni::VideoStream*>  HERE::v_pColorStream;
openni::Device                HERE::device;
/* 実空間のパラメータ */
int                           HERE::deskDepth; // 机までの深さ(mm)
int                           HERE::blockSide; // ブロックの一辺(mm)
int                           HERE::errLen;    // 認識の許容誤差(mm)
int                           HERE::blockNumX;          // x方向のブロックの最大数
int                           HERE::blockNumY;          // y方向のブロックの最大数
int                           HERE::blockNumZ;          // z方向のブロックの最大数
vector<int>                   HERE::v_blockSideScr;      // 投影変換後の画面上でのブロックの1辺(key:下から数えたブロックの数)
vector<int>                   HERE::v_blockDan[3];           // 今積まれてるブロックの段数
map<int,int>                  HERE::m_highest;        // 最も高い場所一覧
vector< pair<int,int> >       HERE::v_pa_drawOrbit; // 描画可能な場所の軌跡
HERE::orbitPoint3D            HERE::highestOrbitPoint;
Wave                          HERE::heightWave;     // 連続的な高さ分布
Wave                          HERE::smoothedWave;     // 連続的な高さ分布を平滑化したもの
/* スレッド関連 */
Mutex                         HERE::mutex;
/* その他 */
bool                          HERE::kinectUseFlag = true;
bool                          HERE::calibModeFlag = false;
bool                          HERE::dispCvImageFlag = true;

void HERE::mainLoop()
{
	if(HERE::kinectUseFlag){
		while( Main::get_endFlag() == false ){
			switch( GameMain::getGameState() ){
			case GS_FIELD:
				{
					HERE::updateDepthImageAndDigitalFieldInfo();
					break;
				}
			case GS_ANALOG:
				{
					HERE::updateDepthImageAndDigitalFieldInfo();
					break;
				}
			case GS_FIELD3D:
				{
					HERE::updateDepthImageAndAnalogFieldInfo();
					break;
				}
			}
		}
	}
	el{
		while( Main::get_endFlag() == false ){
			HERE::updateFieldArrWithoutKinect();
		}
	}
}

// カメラの初期化
void HERE::initCamera()
{
	try {
		/* OpenNIの初期化 */
		openni::OpenNI::initialize();
		auto ret = HERE::device.open( openni::ANY_DEVICE );
		if ( ret != openni::STATUS_OK ) {
			throw std::runtime_error( "" );
		}

		/* 深さカメラの初期化 */
		HERE::depthStream.create( device, openni::SensorType::SENSOR_DEPTH );
		HERE::depthStream.start();
		HERE::v_pDepthStream.push_back( &HERE::depthStream );

		/* カラーカメラの初期化 */
		HERE::colorStream.create( device, openni::SensorType::SENSOR_COLOR );
		HERE::colorStream.start();
		HERE::v_pColorStream.push_back( &HERE::colorStream );

		/* ウィンドウの初期化 */
		static const int DBG_MODE = IO::getConst("DBG_MODE");
		if( DBG_MODE ){
			cv::namedWindow( DEPTH_WINDOW_NAME, CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO );
		}
		// マウスイベントに対するコールバック関数を登録
		cv::setMouseCallback( DEPTH_WINDOW_NAME, HERE::onMouse, 0 );
	}
	catch ( std::exception& ) {
		//std::cout << openni::OpenNI::getExtendedError() << std::endl;
		cout << "Kinectが見つかりませんでした。Kinect無しモードで起動します。" << endl;
		HERE::kinectUseFlag = false;
	}
}

// 実空間に関する各種パラメータの初期化
void HERE::initParams()
{
	HERE::mutex.lock();

	HERE::deskDepth    = (int)IO::getConst("DESK_DEPTH");
	HERE::blockSide    = (int)IO::getConst("BLOCK_SIDE");
	HERE::errLen       = (int)IO::getConst("ERR_LEN");
	HERE::blockNumX     = (int)IO::getConst("BLOCK_NUM_X");
	HERE::blockNumY     = (int)IO::getConst("BLOCK_NUM_Y");
	HERE::blockNumZ     = (int)IO::getConst("BLOCK_NUM_Z");

	for(int i=0;i<3;i++){
		VZERO(HERE::v_blockDan[i],HERE::blockNumX,0); // 要素確保
	}
	/*
	長さW(mm)の物体が、Kinectの面からL(mm)の距離にあるとき、画面上での幅Wm(px)は、
	Wm = W * 526.1146341 / (L+30)
	となる。（この式は13/05/10の実験に基づく）
	*/
	int W = HERE::blockSide;
	int L = 0;
	for( int i = 0; i <= HERE::blockNumY; i++ ){
		L = HERE::deskDepth - i * HERE::blockSide;
		HERE::v_blockSideScr.push_back( (int)( W * 526.1146341 / (L+30) ) );
	}

	HERE::mutex.unlock();
}

// 深さ画像を更新してDigitalフィールド情報を更新
int HERE::updateDepthImageAndDigitalFieldInfo()
{
	/// lockするとデッドロックになる
	//HERE::mutex.lock();

	int changedIndex;
	openni::OpenNI::waitForAnyStream( &HERE::v_pDepthStream[0], HERE::v_pDepthStream.size(), &changedIndex );
	if ( changedIndex == 0 ) {
		openni::VideoFrameRef depthFrame;
		depthStream.readFrame( &depthFrame );
		if ( depthFrame.isValid() ) {
			HERE::depthImage = cv::Mat( depthStream.getVideoMode().getResolutionY(),
				depthStream.getVideoMode().getResolutionX(),
				CV_16U, (char*)depthFrame.getData() );
			// 段数を表示
			for( int i = 0; i < 3; i++ ){
				HERE::updateFieldInfo( i );
			}

			if( HERE::dispCvImageFlag ){
				// 0-10000mmまでのデータを0-255にする
				HERE::depthImage.convertTo( depthImage, CV_8U, 255.0 / 10000 );

				// ブロック情報を読み取る場所に線を描く
				for( int i = 0; i < 3; i++ ){
					for( int x = 1; x <= HERE::blockNumX; x++ ){
						cv::Point p1( IO::configData.startPoint.x , IO::configData.startPoint.y + i*IO::configData.zPixel );
						cv::Point p2( IO::configData.startPoint.x + x * HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + i*IO::configData.zPixel );
						cv::rectangle( HERE::depthImage, p1, p2, cv::Scalar(255), 1 );
					}
				}

				// 段数の数字を描く
				for( int i = 0; i < 3; i++ ){
					HERE::drawDanToDepthImage(i);
				}
				static int const DBG_MODE = (int)IO::getConst("DBG_MODE");
				if( DBG_MODE ){
					// 画像表示
					cv::imshow( DEPTH_WINDOW_NAME, depthImage );
				}
					// キー操作
				int key = cv::waitKey( 1 );
			}
		}
	}

	//HERE::mutex.unlock();
	return 0;
}

// 深さ画像を更新してAnalogフィールド情報を更新
int HERE::updateDepthImageAndAnalogFieldInfo()
{

	int changedIndex;
	openni::OpenNI::waitForAnyStream( &HERE::v_pDepthStream[0], HERE::v_pDepthStream.size(), &changedIndex );
	if ( changedIndex == 0 ) {
		openni::VideoFrameRef depthFrame;
		depthStream.readFrame( &depthFrame );
		if ( depthFrame.isValid() ) {
			HERE::depthImage = cv::Mat( depthStream.getVideoMode().getResolutionY(),
				depthStream.getVideoMode().getResolutionX(),
				CV_16U, (char*)depthFrame.getData() );
			cv::flip(HERE::depthImage,HERE::depthImage,0);
			
			/* depth画像を縦に走査して、列ごとの最高点を見つける */

			// 各列
			IO::mutex.lock();
			Point<int>& p1 = IO::configData.deskRect.p1;
			Point<int>& p2 = IO::configData.deskRect.p2;
			IO::mutex.unlock();

			vector< pair<int,int> > tempOrbit;
			int zeroCount = 0;
			for(int x = p1.x ; x < p2.x; x++){
				int minDepth = INT_MAX;
				int yloc = -1;
				// 各行
				int y0 = 0;
				/* 最高値を求める */
				for(int y = p1.y ; y < p2.y; y++){
					// 0mm-10000mmまでの深さ情報を取得
					///int dep = (int)HERE::depthImage.at<ushort>(y, x);
					int dep = mycv::at_ifOutOgRange_zero<ushort>(HERE::depthImage,y,x);
					if( dep > 0 && dep < minDepth ){
						minDepth = dep;
						yloc = y0;
					}
					y0++;
				}
				y0 = 0;
				/* 「最も奥の点」を見つける */
				for(int y = p1.y ; y < p2.y; y++){
					///int dep = (int)HERE::depthImage.at<ushort>(y, x);
					int dep = mycv::at_ifOutOgRange_zero<ushort>(HERE::depthImage,y,x);
					if( abs( minDepth-dep ) < 20 ){
						yloc = y0;
						break; /// 1点見つかったら終了
					}
					y0++;
				}
				/// x,loc のセットが、あるyにおける最高の点
				tempOrbit.push_back( make_pair(yloc,minDepth) );
				/* 「最も奥の点」が0だったら、あとで補完するためにバッファしておく */
				if( yloc < 10 ){
					zeroCount++;
					// 終点だったら
					if( x == p2.x - 1 ){
						zeroCount++;
						x++;
						goto shuuten;
					}
				}
				el{
shuuten:
					try{
						if( zeroCount > 0 ){
							int beginX = x - zeroCount -1;
							int endX   = x;
							int beginZ=0, endZ=0;
							// 認識領域が全部0
							if( beginX <= p1.x && p2.x <= endX ){
								goto zeroume_end;
							}
							// 始点が認識左端より左
							ef( beginX <= p1.x ){
								endZ   = tempOrbit.at(endX-p1.x).first;
								beginZ = endZ;
							}
							// 終点が認識右端より右
							ef( p2.x <= endX ){
								beginZ = tempOrbit.at(beginX-p1.x).first;
								endZ   = beginZ;
							}
							el{
								beginZ = tempOrbit.at(beginX-p1.x).first;
								endZ   = tempOrbit.at(endX-p1.x).first;
							}
							FOR( xx, beginX + 1, endX){
								tempOrbit.at(xx-p1.x).first = beginZ + (double)(endZ-beginZ)*(xx-beginX)/(endX-beginX);	
							}
zeroume_end:
							zeroCount = 0;
						}
					}catch(...){
						cout << endl;
					}
				}
			}

			/* orbitの、横xと高さyに対してメディアンフィルタをかけて、HERE::v_pa_drawOrbitに格納 */
			HERE::mutex.lock();
			static int const MEDIAN_BLUR_RANGE = IO::getConst("MEDIAN_BLUR_RANGE");
			HERE::medianBlur( tempOrbit, HERE::v_pa_drawOrbit, MEDIAN_BLUR_RANGE );
			HERE::mutex.unlock();

			/* フィルタ後のorbitを走査して、yが最も高い点を見つける */
			HERE::highestOrbitPoint.x = HERE::highestOrbitPoint.y = HERE::highestOrbitPoint.z = 99999999;
			int i = 0;
			foreach(it,HERE::v_pa_drawOrbit){
				if( it->second < HERE::highestOrbitPoint.y ){
					HERE::highestOrbitPoint.x = i;
					HERE::highestOrbitPoint.y = it->second;
					HERE::highestOrbitPoint.z = it->first;
				}
				i++;
			}
			
			{
				static int const MAX_HEIGHT_RANGE_mm = IO::getConst("MAX_HEIGHT_RANGE_mm");
				int count=1;
				orbitPoint3D average;
				average.x=HERE::highestOrbitPoint.x;
				average.y=HERE::highestOrbitPoint.y;
				average.z=HERE::highestOrbitPoint.z;
				int i=0;
				int outofAveCount=0;
				foreach(it,HERE::v_pa_drawOrbit){
					if( abs(HERE::highestOrbitPoint.y-it->second) < MAX_HEIGHT_RANGE_mm ){
						count++;
						average.x+=i;
						average.y+=it->second;
						average.z+=it->first;
						outofAveCount=0;
					}
					else{
						outofAveCount++;
						if( outofAveCount > 20 ){
							break;
						}
					}
					i++;
				}
				HERE::highestOrbitPoint.x = average.x / count;
				HERE::highestOrbitPoint.y = average.y / count;
				HERE::highestOrbitPoint.z = average.z / count;
			}

			if( HERE::dispCvImageFlag ){
				// 0-10000mmまでのデータを0-255にする
				HERE::depthImage.convertTo( HERE::depthImage, CV_8U, 255.0 / 10000 );

				// 最高点の線を表示
				int x = p1.x;
				int y = p1.y;
				HERE::mutex.lock();
				foreach(it,HERE::v_pa_drawOrbit){
					auto it2 = ++it;
					it--;
					if( it2 != HERE::v_pa_drawOrbit.end() ){
						cv::Point p1(x,it->first+y);
						cv::Point p2(x+1,it2->first+y);
						cv::line( HERE::depthImage, p1, p2, cv::Scalar(255,0,0) );
					}
					x++;
				}
				HERE::mutex.unlock();

				// 表示領域の矩形を表示
				IO::mutex.lock();
				cv::Point lu = utils::cvPoint( IO::configData.deskRect.p1 );
				cv::Point rd = utils::cvPoint( IO::configData.deskRect.p2 );
				cv::Point ld( lu.x, rd.y );
				cv::Point ru( rd.x, lu.y );
				cv::Scalar color(255,255,255);
				cv::line( HERE::depthImage, lu, ld, color );
				cv::line( HERE::depthImage, ld, rd, color );
				cv::line( HERE::depthImage, rd, ru, color );
				cv::line( HERE::depthImage, ru, lu, color );
				IO::mutex.unlock();

				// キャリブレーションモードのとき
				if( HERE::calibModeFlag ){
					cv::putText( HERE::depthImage, "<<calibration mode>>", cv::Point(10,20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar( 255, 0, 0 ),1 );
				}

				static int const DBG_MODE = (int)IO::getConst("DBG_MODE");
				if( DBG_MODE ){
					cv::imshow( DEPTH_WINDOW_NAME, depthImage );
				}
				// キー操作
				int key = cv::waitKey( 1 );
			}
        }
    }

	
	return 0;
}

// 色画像を更新する
void HERE::updateColorImage()
{
	HERE::mutex.lock();

    int changedIndex;
    openni::OpenNI::waitForAnyStream( &HERE::v_pColorStream[0], HERE::v_pColorStream.size(), &changedIndex );
    if ( changedIndex == 0 ) {
        openni::VideoFrameRef colorFrame;
        HERE::colorStream.readFrame( &colorFrame );
        if ( colorFrame.isValid() ) {
            colorImage = cv::Mat( colorStream.getVideoMode().getResolutionY(),
                                    colorStream.getVideoMode().getResolutionX(),
                                    CV_8UC3, (char*)colorFrame.getData() );
        }

        cv::cvtColor( colorImage, colorImage, CV_BGR2RGB );
       

		cv::imshow( "Color Camera", HERE::colorImage );
		int key = cv::waitKey( 10 );
    }

	HERE::mutex.unlock();
}

// 深さ画像を解析してゲームフィールドの情報を更新する
void HERE::updateFieldInfo( int _line )
{
	static double DETECT_TH = IO::getConst("DETECT_TH");

	VZERO(HERE::v_blockDan[_line],HERE::blockNumX,0);

	int* a_dan = new int[HERE::blockNumY+1];

	// ブロック情報を読み取る場所の矩形
	for( int X = 0; X < HERE::blockNumX; X++ ){
		cv::Point p1( IO::configData.startPoint.x + X*HERE::v_blockSideScr.at(0), IO::configData.startPoint.y + _line*IO::configData.zPixel );
		cv::Point p2( IO::configData.startPoint.x + (X+1)*HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + _line*IO::configData.zPixel );
		NZERO(a_dan,HERE::blockNumY+1);
		// 矩形内の各ピクセル
		for( int x = p1.x; x < p2.x; x++ ){
			int depthSum = 0;
			int activeYcount = 0;
			for( int y = p1.y; y < p2.y; y++ ){
				// 0mm-10000mmまでの深さ情報を取得
				///int dep = (int)HERE::depthImage.at<ushort>(y, x);
				int dep = mycv::at_ifOutOgRange_zero<ushort>(HERE::depthImage,y,x);
				/* FieldState で使う　離散的なブロック位置　に関するパラメータ */

				// 距離0（黒詰め）だったら
				if( dep == 0 ){
					// 無視
					continue;
				}
				// 机より遠かったら
				else if( dep > HERE::deskDepth + HERE::errLen ){
					activeYcount++;
					depthSum += dep;
					// 無視
					continue;
				}
				// 机より手前にあったら
				else {
					activeYcount++;
					depthSum += dep;
					double hei = (HERE::deskDepth-dep+HERE::blockSide/2);
					// 段数
					int dan = (int)(hei/HERE::blockSide);
					// 認識できる高さだったら
					if( dan <= HERE::blockNumY ){
						a_dan[dan]++;
					}
				}
			}
			HERE::heightWave.m_r[x] = HERE::deskDepth - (depthSum / ((activeYcount==0)?1:activeYcount));
		}
		// 段数配列の中で、１つの値が6割以上を占めていたら、ブロックの段数をその値とする。
		for( int i = 0; i <= HERE::blockNumY; i++ ){
			if( a_dan[i] > powf( (double)HERE::v_blockSideScr.at(0), 2 ) * DETECT_TH / 100 ){
				HERE::v_blockDan[_line][X] = i;
				break;
			}
		}
	}
	delete [] a_dan;

	// 波形を平滑化
	HERE::smoothHeightWave();
}

// キー入力を元にfieldArrを更新する
void HERE::updateFieldArrWithoutKinect()
{
	HERE::mutex.lock();

	for(int i=0;i<3;i++){
		VZERO(HERE::v_blockDan[i],HERE::blockNumX,0);
		for( int X = 0; X < HERE::blockNumX; X++ ){
			HERE::v_blockDan[i][X] = 2;
		}
	}
	
	HERE::mutex.unlock();
}

// 段数をdepthImageに表示
void RealField::drawDanToDepthImage( int _line )
{

	int maxHeight = 0; // 一番高いところの高さ
	HERE::m_highest.clear();

	// すべての認識マスについて
	for( int X = 0; X < HERE::blockNumX; X++ ){
		cv::Point p1( IO::configData.startPoint.x + X*HERE::v_blockSideScr.at(0), IO::configData.startPoint.y + _line*IO::configData.zPixel );
		cv::Point p2( IO::configData.startPoint.x + (X+1)*HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + _line*IO::configData.zPixel );
		// 段数を表示する
		std::stringstream ss;
		ss << HERE::v_blockDan[_line][X];
		cv::putText( depthImage, ss.str(), cv::Point( (p1.x+p2.x)/2-10, p1.y ),
			cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar( 255, 0, 0 ), 1 );
		// 最高点より高い
		if( HERE::v_blockDan[_line].at(X) > maxHeight ){
			HERE::m_highest.clear();
			MINSERT(HERE::m_highest,X,HERE::v_blockDan[_line].at(X));
		}
		// 最高点と同じ
		ef( HERE::v_blockDan[_line].at(X) == maxHeight ){
			MINSERT(HERE::m_highest,X,HERE::v_blockDan[_line].at(X));
		}
	}
}

// 高さ波形を平滑化
void RealField::smoothHeightWave(  )
{
	//FT::FFT( HERE::heightWave, HERE::smoothedWave, powf(2,utils::upperPow( HERE::heightWave.size(),2 )) );

	double before=0,after=0;
	foreach(it,HERE::heightWave.m_r){
		before = it->second;
		after = it->second;
		if( (++it)!=HERE::heightWave.m_r.end() ){
			after = it->second;
		}
		--it;
		if( it!=HERE::heightWave.m_r.begin() ){
			before = (--it)->second;
		}
		++it;

		double averaged = (before+it->second+after)/3;
		HERE::smoothedWave.m_r[it->first] = averaged;
	}

	//FT::IFFT( HERE::smoothedWave, HERE::smoothedWave, powf(2,utils::upperPow( HERE::smoothedWave.size(),2 )) );
}

int RealField::getDan( int _line, int _x )
{
	int ret = 0;

	HERE::mutex.lock();
	// フィールドの範囲内	
	if( 0 <= _x && _x < HERE::blockNumX ){
		try{
			ret = HERE::v_blockDan[_line].at(_x);
		}
		catch(std::exception& exp){
			ret = 0;
			//cout << exp.what() << endl;
		}
	}
	HERE::mutex.unlock();
	return ret;
}

pair<int,int> RealField::getmm_Z_Y( int _xmm )
{
	pair<int,int> ret;
	HERE::mutex.lock();
	try{
		IO::mutex.lock();
		static int const SCR_W = IO::getConst("SCR_W"); /// 800px
		static int const SHOW_X = IO::getConst("SHOW_X"); /// モニタ横幅 378mm
		static int const DESK_DEPTH = IO::getConst("DESK_DEPTH");
		int const rectx = IO::configData.deskRect.getWidth(); /// 認識矩形の幅 任意px
		int xdot = _xmm * (double)SCR_W/SHOW_X * (double)rectx/SCR_W;
		IO::mutex.unlock();
		pair<int,int> const & z_y =  HERE::v_pa_drawOrbit.at(xdot);
		ret.first  = z_y.first  * (double)SCR_W/rectx * (double)SHOW_X/SCR_W;
		ret.second = DESK_DEPTH - z_y.second;
	}
	catch( std::exception ){
		ret = pair<int,int>(macro::WRONG_INT,macro::WRONG_INT);
	}
	HERE::mutex.unlock();
	return ret;
}

Point3D_mm HERE::getHighestPoint()
{
	HERE::mutex.lock();
	IO::mutex.lock();
	static int const SCR_W = IO::getConst("SCR_W"); /// 800px
	static int const SHOW_X = IO::getConst("SHOW_X"); /// モニタ横幅 378mm
	static int const DESK_DEPTH = IO::getConst("DESK_DEPTH");
	int const rectx = IO::configData.deskRect.getWidth(); /// 認識矩形の幅 任意px
	IO::mutex.unlock();
	int xmm =  (double)HERE::highestOrbitPoint.x/((double)SCR_W/SHOW_X * (double)rectx/SCR_W);
	pair<int,int> z_y;
	z_y =  HERE::getmm_Z_Y(xmm);
	if( z_y.first == macro::WRONG_INT && z_y.second == macro::WRONG_INT ){
		z_y.first = 0;
		z_y.second = 0;
	}
	Point3D_mm ret( xmm, z_y.second, z_y.first );
	HERE::mutex.unlock();
	return ret;
}

void HERE::inv_dispCvImageFlag()
{
	HERE::mutex.lock();

	HERE::dispCvImageFlag = !HERE::dispCvImageFlag;
	if( dispCvImageFlag ){ 
		cv::namedWindow( DEPTH_WINDOW_NAME, CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO );
	}
	el{
		cv::destroyWindow( DEPTH_WINDOW_NAME );
	}

	HERE::mutex.unlock();
}


void HERE::onMouse( int evt, int x, int y, int flag, void* )
{
	static int const SHOW_X = IO::getConst("SHOW_X");
	static int const SHOW_Z = IO::getConst("SHOW_Z");

	static Point<int> rightDownPoint;
	static Point<int> leftDownPoint;
	static Point<int> leftUpPoint;

	// キャリブレーションモードではない
	if( HERE::calibModeFlag == false ){
		switch(evt){
		case cv::EVENT_MBUTTONDOWN: // ホイール押す
			{
				HERE::calibModeFlag = !HERE::calibModeFlag;
			}
			break;
		}
	}
	// キャリブレーションモード
	el{
		// マウスイベントを取得
		switch(evt) {
		case cv::EVENT_MOUSEMOVE:   // マウス移動
			{

			}
			break;
		case cv::EVENT_LBUTTONDOWN: // 左クリック押す
			{
				//IO::mutex.lock();
				//IO::configData.deskRect.p1 = Point<int>(x,y);
				//IO::configData.deskRect.p2 = Point<int>(x,y);
				leftDownPoint = Point<int>(x,y);
				leftUpPoint = Point<int>(x,y);
				//IO::mutex.unlock();
			}
			break;
		case cv::EVENT_RBUTTONDOWN: // 右クリック押す
			{
				IO::mutex.lock();
				rightDownPoint = Point<int>(x,y); /// クリックした点を保存
				IO::mutex.unlock();
			}
			break;
		case cv::EVENT_MBUTTONDOWN: // ホイール押す
			{
				HERE::calibModeFlag = !HERE::calibModeFlag;
			}
			break;
		case cv::EVENT_LBUTTONUP:   // 左クリック離す
			{
			}
			break;
		case cv::EVENT_RBUTTONUP:   // 右クリック離す
			{
			}
			break;
		case cv::EVENT_MBUTTONUP:   // ホイール離す
			{
		
			}
			break;
		case cv::EVENT_LBUTTONDBLCLK: // 左ダブルクリック
			{
		
			}
			break;
		case cv::EVENT_RBUTTONDBLCLK: // 右ダブルクリック
			{
		
			}
			break;
		case cv::EVENT_MBUTTONDBLCLK: // ホイールダブルクリック
			{
			}
			break;
		}

		// マウスボタン，及び修飾キーを取得
		if(flag & cv::EVENT_FLAG_LBUTTON)  // 左クリック押してるとき
		{
			IO::mutex.lock();
			//int p1x = IO::configData.deskRect.p1.x;
			//int p1y = IO::configData.deskRect.p1.y;
			//IO::configData.deskRect.p2 = Point<int>(x,p1y+(int)((x-p1x)*((double)SHOW_Z/SHOW_X)));
			int pm = (y<leftDownPoint.y) ?  -1 : +1 ;
			leftUpPoint = Point<int>(x,leftDownPoint.y+pm*(int)abs((x-leftDownPoint.x)*((double)SHOW_Z/SHOW_X)));
			IO::configData.deskRect.p1.x = min(leftDownPoint.x,leftUpPoint.x);
			IO::configData.deskRect.p1.y = min(leftDownPoint.y,leftUpPoint.y);
			IO::configData.deskRect.p2.x = max(leftDownPoint.x,leftUpPoint.x);
			IO::configData.deskRect.p2.y = max(leftDownPoint.y,leftUpPoint.y);
			IO::mutex.unlock();
		}
		if(flag & cv::EVENT_FLAG_RBUTTON)  // 右クリック押してるとき
		{
			IO::mutex.lock();
			int dx = x - rightDownPoint.x; /// マウスの移動分
			int dy = y - rightDownPoint.y; /// マウスの移動分
			rightDownPoint = Point<int>(x,y);
			IO::configData.deskRect.p1.x += dx;
			IO::configData.deskRect.p1.y += dy;
			IO::configData.deskRect.p2.x += dx;
			IO::configData.deskRect.p2.y += dy;
			IO::mutex.unlock();
		}
		if(flag & cv::EVENT_FLAG_MBUTTON)  // ホイール押してるとき
		{

		}
		if(flag & cv::EVENT_FLAG_CTRLKEY)  // ctrl
		{

		}
		if(flag & cv::EVENT_FLAG_SHIFTKEY) // shift
		{

		}
		if(flag & cv::EVENT_FLAG_ALTKEY)   // alt
		{
		
		}
	}
}

// メディアンフィルタをかける
void RealField::medianBlur( vector< pair<int,int> > const & _vec, vector< pair<int,int> > & _dst, int _range )
{
	_dst.clear();
	for( int i=0; i<_vec.size(); i++ ){
		int sum_first=0, sum_second=0;
		int sumCount_first=0, sumCount_second=0;
		for( int iter = i-_range; iter < i+_range; iter++ ){
			try{
				int var_second = _vec.at(iter).second; /// out_of_range例外出ることある
				int var_first  = _vec.at(iter).first;
				sum_first += var_first;
				sum_second += var_second;
				sumCount_first++;
				sumCount_second++;
			}
			catch(std::out_of_range){
				continue;
			}
		}
		_dst.push_back( make_pair(sum_first/sumCount_first,sum_second/sumCount_second) );
	}
}
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

/* Kinect����Ƃ������� */
cv::Mat                       HERE::depthImage;
cv::Mat                       HERE::colorImage;
/* �J�����֘A */
openni::VideoStream           HERE::depthStream;
vector<openni::VideoStream*>  HERE::v_pDepthStream;
openni::VideoStream           HERE::colorStream;
vector<openni::VideoStream*>  HERE::v_pColorStream;
openni::Device                HERE::device;
/* ����Ԃ̃p�����[�^ */
int                           HERE::deskDepth; // ���܂ł̐[��(mm)
int                           HERE::blockSide; // �u���b�N�̈��(mm)
int                           HERE::errLen;    // �F���̋��e�덷(mm)
int                           HERE::blockNumX;          // x�����̃u���b�N�̍ő吔
int                           HERE::blockNumY;          // y�����̃u���b�N�̍ő吔
int                           HERE::blockNumZ;          // z�����̃u���b�N�̍ő吔
vector<int>                   HERE::v_blockSideScr;      // ���e�ϊ���̉�ʏ�ł̃u���b�N��1��(key:�����琔�����u���b�N�̐�)
vector<int>                   HERE::v_blockDan[3];           // ���ς܂�Ă�u���b�N�̒i��
map<int,int>                  HERE::m_highest;        // �ł������ꏊ�ꗗ
vector< pair<int,int> >       HERE::v_pa_drawOrbit; // �`��\�ȏꏊ�̋O��
HERE::orbitPoint3D            HERE::highestOrbitPoint;
Wave                          HERE::heightWave;     // �A���I�ȍ������z
Wave                          HERE::smoothedWave;     // �A���I�ȍ������z�𕽊�����������
/* �X���b�h�֘A */
Mutex                         HERE::mutex;
/* ���̑� */
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

// �J�����̏�����
void HERE::initCamera()
{
	try {
		/* OpenNI�̏����� */
		openni::OpenNI::initialize();
		auto ret = HERE::device.open( openni::ANY_DEVICE );
		if ( ret != openni::STATUS_OK ) {
			throw std::runtime_error( "" );
		}

		/* �[���J�����̏����� */
		HERE::depthStream.create( device, openni::SensorType::SENSOR_DEPTH );
		HERE::depthStream.start();
		HERE::v_pDepthStream.push_back( &HERE::depthStream );

		/* �J���[�J�����̏����� */
		HERE::colorStream.create( device, openni::SensorType::SENSOR_COLOR );
		HERE::colorStream.start();
		HERE::v_pColorStream.push_back( &HERE::colorStream );

		/* �E�B���h�E�̏����� */
		static const int DBG_MODE = IO::getConst("DBG_MODE");
		if( DBG_MODE ){
			cv::namedWindow( DEPTH_WINDOW_NAME, CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO );
		}
		// �}�E�X�C�x���g�ɑ΂���R�[���o�b�N�֐���o�^
		cv::setMouseCallback( DEPTH_WINDOW_NAME, HERE::onMouse, 0 );
	}
	catch ( std::exception& ) {
		//std::cout << openni::OpenNI::getExtendedError() << std::endl;
		cout << "Kinect��������܂���ł����BKinect�������[�h�ŋN�����܂��B" << endl;
		HERE::kinectUseFlag = false;
	}
}

// ����ԂɊւ���e��p�����[�^�̏�����
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
		VZERO(HERE::v_blockDan[i],HERE::blockNumX,0); // �v�f�m��
	}
	/*
	����W(mm)�̕��̂��AKinect�̖ʂ���L(mm)�̋����ɂ���Ƃ��A��ʏ�ł̕�Wm(px)�́A
	Wm = W * 526.1146341 / (L+30)
	�ƂȂ�B�i���̎���13/05/10�̎����Ɋ�Â��j
	*/
	int W = HERE::blockSide;
	int L = 0;
	for( int i = 0; i <= HERE::blockNumY; i++ ){
		L = HERE::deskDepth - i * HERE::blockSide;
		HERE::v_blockSideScr.push_back( (int)( W * 526.1146341 / (L+30) ) );
	}

	HERE::mutex.unlock();
}

// �[���摜���X�V����Digital�t�B�[���h�����X�V
int HERE::updateDepthImageAndDigitalFieldInfo()
{
	/// lock����ƃf�b�h���b�N�ɂȂ�
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
			// �i����\��
			for( int i = 0; i < 3; i++ ){
				HERE::updateFieldInfo( i );
			}

			if( HERE::dispCvImageFlag ){
				// 0-10000mm�܂ł̃f�[�^��0-255�ɂ���
				HERE::depthImage.convertTo( depthImage, CV_8U, 255.0 / 10000 );

				// �u���b�N����ǂݎ��ꏊ�ɐ���`��
				for( int i = 0; i < 3; i++ ){
					for( int x = 1; x <= HERE::blockNumX; x++ ){
						cv::Point p1( IO::configData.startPoint.x , IO::configData.startPoint.y + i*IO::configData.zPixel );
						cv::Point p2( IO::configData.startPoint.x + x * HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + i*IO::configData.zPixel );
						cv::rectangle( HERE::depthImage, p1, p2, cv::Scalar(255), 1 );
					}
				}

				// �i���̐�����`��
				for( int i = 0; i < 3; i++ ){
					HERE::drawDanToDepthImage(i);
				}
				static int const DBG_MODE = (int)IO::getConst("DBG_MODE");
				if( DBG_MODE ){
					// �摜�\��
					cv::imshow( DEPTH_WINDOW_NAME, depthImage );
				}
					// �L�[����
				int key = cv::waitKey( 1 );
			}
		}
	}

	//HERE::mutex.unlock();
	return 0;
}

// �[���摜���X�V����Analog�t�B�[���h�����X�V
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
			
			/* depth�摜���c�ɑ������āA�񂲂Ƃ̍ō��_�������� */

			// �e��
			IO::mutex.lock();
			Point<int>& p1 = IO::configData.deskRect.p1;
			Point<int>& p2 = IO::configData.deskRect.p2;
			IO::mutex.unlock();

			vector< pair<int,int> > tempOrbit;
			int zeroCount = 0;
			for(int x = p1.x ; x < p2.x; x++){
				int minDepth = INT_MAX;
				int yloc = -1;
				// �e�s
				int y0 = 0;
				/* �ō��l�����߂� */
				for(int y = p1.y ; y < p2.y; y++){
					// 0mm-10000mm�܂ł̐[�������擾
					///int dep = (int)HERE::depthImage.at<ushort>(y, x);
					int dep = mycv::at_ifOutOgRange_zero<ushort>(HERE::depthImage,y,x);
					if( dep > 0 && dep < minDepth ){
						minDepth = dep;
						yloc = y0;
					}
					y0++;
				}
				y0 = 0;
				/* �u�ł����̓_�v�������� */
				for(int y = p1.y ; y < p2.y; y++){
					///int dep = (int)HERE::depthImage.at<ushort>(y, x);
					int dep = mycv::at_ifOutOgRange_zero<ushort>(HERE::depthImage,y,x);
					if( abs( minDepth-dep ) < 20 ){
						yloc = y0;
						break; /// 1�_����������I��
					}
					y0++;
				}
				/// x,loc �̃Z�b�g���A����y�ɂ�����ō��̓_
				tempOrbit.push_back( make_pair(yloc,minDepth) );
				/* �u�ł����̓_�v��0��������A���Ƃŕ⊮���邽�߂Ƀo�b�t�@���Ă��� */
				if( yloc < 10 ){
					zeroCount++;
					// �I�_��������
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
							// �F���̈悪�S��0
							if( beginX <= p1.x && p2.x <= endX ){
								goto zeroume_end;
							}
							// �n�_���F�����[��荶
							ef( beginX <= p1.x ){
								endZ   = tempOrbit.at(endX-p1.x).first;
								beginZ = endZ;
							}
							// �I�_���F���E�[���E
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

			/* orbit�́A��x�ƍ���y�ɑ΂��ă��f�B�A���t�B���^�������āAHERE::v_pa_drawOrbit�Ɋi�[ */
			HERE::mutex.lock();
			static int const MEDIAN_BLUR_RANGE = IO::getConst("MEDIAN_BLUR_RANGE");
			HERE::medianBlur( tempOrbit, HERE::v_pa_drawOrbit, MEDIAN_BLUR_RANGE );
			HERE::mutex.unlock();

			/* �t�B���^���orbit�𑖍����āAy���ł������_�������� */
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
				// 0-10000mm�܂ł̃f�[�^��0-255�ɂ���
				HERE::depthImage.convertTo( HERE::depthImage, CV_8U, 255.0 / 10000 );

				// �ō��_�̐���\��
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

				// �\���̈�̋�`��\��
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

				// �L�����u���[�V�������[�h�̂Ƃ�
				if( HERE::calibModeFlag ){
					cv::putText( HERE::depthImage, "<<calibration mode>>", cv::Point(10,20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar( 255, 0, 0 ),1 );
				}

				static int const DBG_MODE = (int)IO::getConst("DBG_MODE");
				if( DBG_MODE ){
					cv::imshow( DEPTH_WINDOW_NAME, depthImage );
				}
				// �L�[����
				int key = cv::waitKey( 1 );
			}
        }
    }

	
	return 0;
}

// �F�摜���X�V����
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

// �[���摜����͂��ăQ�[���t�B�[���h�̏����X�V����
void HERE::updateFieldInfo( int _line )
{
	static double DETECT_TH = IO::getConst("DETECT_TH");

	VZERO(HERE::v_blockDan[_line],HERE::blockNumX,0);

	int* a_dan = new int[HERE::blockNumY+1];

	// �u���b�N����ǂݎ��ꏊ�̋�`
	for( int X = 0; X < HERE::blockNumX; X++ ){
		cv::Point p1( IO::configData.startPoint.x + X*HERE::v_blockSideScr.at(0), IO::configData.startPoint.y + _line*IO::configData.zPixel );
		cv::Point p2( IO::configData.startPoint.x + (X+1)*HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + _line*IO::configData.zPixel );
		NZERO(a_dan,HERE::blockNumY+1);
		// ��`���̊e�s�N�Z��
		for( int x = p1.x; x < p2.x; x++ ){
			int depthSum = 0;
			int activeYcount = 0;
			for( int y = p1.y; y < p2.y; y++ ){
				// 0mm-10000mm�܂ł̐[�������擾
				///int dep = (int)HERE::depthImage.at<ushort>(y, x);
				int dep = mycv::at_ifOutOgRange_zero<ushort>(HERE::depthImage,y,x);
				/* FieldState �Ŏg���@���U�I�ȃu���b�N�ʒu�@�Ɋւ���p�����[�^ */

				// ����0�i���l�߁j��������
				if( dep == 0 ){
					// ����
					continue;
				}
				// ����艓��������
				else if( dep > HERE::deskDepth + HERE::errLen ){
					activeYcount++;
					depthSum += dep;
					// ����
					continue;
				}
				// ������O�ɂ�������
				else {
					activeYcount++;
					depthSum += dep;
					double hei = (HERE::deskDepth-dep+HERE::blockSide/2);
					// �i��
					int dan = (int)(hei/HERE::blockSide);
					// �F���ł��鍂����������
					if( dan <= HERE::blockNumY ){
						a_dan[dan]++;
					}
				}
			}
			HERE::heightWave.m_r[x] = HERE::deskDepth - (depthSum / ((activeYcount==0)?1:activeYcount));
		}
		// �i���z��̒��ŁA�P�̒l��6���ȏ���߂Ă�����A�u���b�N�̒i�������̒l�Ƃ���B
		for( int i = 0; i <= HERE::blockNumY; i++ ){
			if( a_dan[i] > powf( (double)HERE::v_blockSideScr.at(0), 2 ) * DETECT_TH / 100 ){
				HERE::v_blockDan[_line][X] = i;
				break;
			}
		}
	}
	delete [] a_dan;

	// �g�`�𕽊���
	HERE::smoothHeightWave();
}

// �L�[���͂�����fieldArr���X�V����
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

// �i����depthImage�ɕ\��
void RealField::drawDanToDepthImage( int _line )
{

	int maxHeight = 0; // ��ԍ����Ƃ���̍���
	HERE::m_highest.clear();

	// ���ׂĂ̔F���}�X�ɂ���
	for( int X = 0; X < HERE::blockNumX; X++ ){
		cv::Point p1( IO::configData.startPoint.x + X*HERE::v_blockSideScr.at(0), IO::configData.startPoint.y + _line*IO::configData.zPixel );
		cv::Point p2( IO::configData.startPoint.x + (X+1)*HERE::v_blockSideScr.at(0) , IO::configData.startPoint.y + HERE::v_blockSideScr.at(0) + _line*IO::configData.zPixel );
		// �i����\������
		std::stringstream ss;
		ss << HERE::v_blockDan[_line][X];
		cv::putText( depthImage, ss.str(), cv::Point( (p1.x+p2.x)/2-10, p1.y ),
			cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar( 255, 0, 0 ), 1 );
		// �ō��_��荂��
		if( HERE::v_blockDan[_line].at(X) > maxHeight ){
			HERE::m_highest.clear();
			MINSERT(HERE::m_highest,X,HERE::v_blockDan[_line].at(X));
		}
		// �ō��_�Ɠ���
		ef( HERE::v_blockDan[_line].at(X) == maxHeight ){
			MINSERT(HERE::m_highest,X,HERE::v_blockDan[_line].at(X));
		}
	}
}

// �����g�`�𕽊���
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
	// �t�B�[���h�͈͓̔�	
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
		static int const SHOW_X = IO::getConst("SHOW_X"); /// ���j�^���� 378mm
		static int const DESK_DEPTH = IO::getConst("DESK_DEPTH");
		int const rectx = IO::configData.deskRect.getWidth(); /// �F����`�̕� �C��px
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
	static int const SHOW_X = IO::getConst("SHOW_X"); /// ���j�^���� 378mm
	static int const DESK_DEPTH = IO::getConst("DESK_DEPTH");
	int const rectx = IO::configData.deskRect.getWidth(); /// �F����`�̕� �C��px
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

	// �L�����u���[�V�������[�h�ł͂Ȃ�
	if( HERE::calibModeFlag == false ){
		switch(evt){
		case cv::EVENT_MBUTTONDOWN: // �z�C�[������
			{
				HERE::calibModeFlag = !HERE::calibModeFlag;
			}
			break;
		}
	}
	// �L�����u���[�V�������[�h
	el{
		// �}�E�X�C�x���g���擾
		switch(evt) {
		case cv::EVENT_MOUSEMOVE:   // �}�E�X�ړ�
			{

			}
			break;
		case cv::EVENT_LBUTTONDOWN: // ���N���b�N����
			{
				//IO::mutex.lock();
				//IO::configData.deskRect.p1 = Point<int>(x,y);
				//IO::configData.deskRect.p2 = Point<int>(x,y);
				leftDownPoint = Point<int>(x,y);
				leftUpPoint = Point<int>(x,y);
				//IO::mutex.unlock();
			}
			break;
		case cv::EVENT_RBUTTONDOWN: // �E�N���b�N����
			{
				IO::mutex.lock();
				rightDownPoint = Point<int>(x,y); /// �N���b�N�����_��ۑ�
				IO::mutex.unlock();
			}
			break;
		case cv::EVENT_MBUTTONDOWN: // �z�C�[������
			{
				HERE::calibModeFlag = !HERE::calibModeFlag;
			}
			break;
		case cv::EVENT_LBUTTONUP:   // ���N���b�N����
			{
			}
			break;
		case cv::EVENT_RBUTTONUP:   // �E�N���b�N����
			{
			}
			break;
		case cv::EVENT_MBUTTONUP:   // �z�C�[������
			{
		
			}
			break;
		case cv::EVENT_LBUTTONDBLCLK: // ���_�u���N���b�N
			{
		
			}
			break;
		case cv::EVENT_RBUTTONDBLCLK: // �E�_�u���N���b�N
			{
		
			}
			break;
		case cv::EVENT_MBUTTONDBLCLK: // �z�C�[���_�u���N���b�N
			{
			}
			break;
		}

		// �}�E�X�{�^���C�y�яC���L�[���擾
		if(flag & cv::EVENT_FLAG_LBUTTON)  // ���N���b�N�����Ă�Ƃ�
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
		if(flag & cv::EVENT_FLAG_RBUTTON)  // �E�N���b�N�����Ă�Ƃ�
		{
			IO::mutex.lock();
			int dx = x - rightDownPoint.x; /// �}�E�X�̈ړ���
			int dy = y - rightDownPoint.y; /// �}�E�X�̈ړ���
			rightDownPoint = Point<int>(x,y);
			IO::configData.deskRect.p1.x += dx;
			IO::configData.deskRect.p1.y += dy;
			IO::configData.deskRect.p2.x += dx;
			IO::configData.deskRect.p2.y += dy;
			IO::mutex.unlock();
		}
		if(flag & cv::EVENT_FLAG_MBUTTON)  // �z�C�[�������Ă�Ƃ�
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

// ���f�B�A���t�B���^��������
void RealField::medianBlur( vector< pair<int,int> > const & _vec, vector< pair<int,int> > & _dst, int _range )
{
	_dst.clear();
	for( int i=0; i<_vec.size(); i++ ){
		int sum_first=0, sum_second=0;
		int sumCount_first=0, sumCount_second=0;
		for( int iter = i-_range; iter < i+_range; iter++ ){
			try{
				int var_second = _vec.at(iter).second; /// out_of_range��O�o�邱�Ƃ���
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
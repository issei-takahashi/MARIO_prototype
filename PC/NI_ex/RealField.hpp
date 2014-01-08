#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include "Block.hpp"
#include "Wave.hpp"
#include "Mutex.hpp"
#include "Point3D_mm.h"
#include <opencv2\opencv.hpp>

class RealField
{
interface__:
	static void mainLoop();
	// �J�����̏�����
	static void initCamera();
	// ����ԂɊւ���e��p�����[�^�̏�����
	static void initParams();

	// get
	// ������x�Ԗ�(X==0,1,2..)�̒i�����擾
	static int getDan( int _line, int _x );
	static pair<int,int> getmm_Z_Y( int _xmm );
	static Point3D_mm getHighestPoint();

	// get_
	static int get_blockNumX() { return blockNumX; } 
	static vector<int> get_v_blockDan( int _line ) { return v_blockDan[_line]; }
	static map<int,int> get_m_highest() { return m_highest; }
	static int getBlockSideScr0() { return v_blockSideScr.at(0); }

	static void inv_dispCvImageFlag();

inner__:
	// �[���摜���X�V����Digital�t�B�[���h�����X�V
	static int updateDepthImageAndDigitalFieldInfo();
	// �[���摜���X�V����Analog�t�B�[���h�����X�V
	static int updateDepthImageAndAnalogFieldInfo();
	// �F�摜���X�V����
	static void updateColorImage();
	// �[���摜����͂���fieldArr���X�V����
	static void updateFieldInfo( int _line );
	// �L�[���͂�����fieldArr���X�V����
	static void updateFieldArrWithoutKinect();
	// �i����depthImage�ɕ\��
	static void drawDanToDepthImage( int _line );
	// �����g�`�𕽊���
	static void smoothHeightWave(  );
	static void onMouse( int evt, int x, int y, int flag, void* );
	// ���f�B�A���t�B���^��������
	static void medianBlur( vector< pair<int,int> > const & _vec, vector< pair<int,int> > & _dst, int _range );

open__:
	static Wave heightWave;   // �����g�`
	static Wave smoothedWave; // �����g�`�𕽊�����������
	static Mutex mutex;       // mutex

capcelled__:
	static vector<int> v_blockDan[3];           // ���ς܂�Ă�u���b�N�̒i��


inner__:
	/* Kinect����Ƃ������� */
	static cv::Mat depthImage;        // �[���摜
	static cv::Mat colorImage;        // �J���[�摜
	/* �J�����֘A */
	static openni::VideoStream            depthStream;
	static vector<openni::VideoStream*>   v_pDepthStream;
	static openni::VideoStream            colorStream;
	static vector<openni::VideoStream*>   v_pColorStream;
	static openni::Device                 device;
	/* ����Ԃ̃p�����[�^ */
	static int deskDepth; // ���܂ł̐[��(mm)
	static int blockSide; // �u���b�N�̈��(mm)
	static int errLen;    // �F���̋��e�덷(mm)
	static int blockNumX;          // x�����̃u���b�N�̍ő吔
	static int blockNumY;          // y�����̃u���b�N�̍ő吔
	static int blockNumZ;          // z�����̃u���b�N�̍ő吔
	static vector<int> v_blockSideScr;      // ���e�ϊ���̉�ʏ�ł̃u���b�N��1��(key:�����琔�����u���b�N�̐�)
	static map<int,int> m_highest;        // �ł������ꏊ�ꗗ
	static vector< pair<int,int> > v_pa_drawOrbit; // �`��\�ȏꏊ�̋O�� @x -> (z,y(minDepth))
	class orbitPoint3D
	{
	public:
		orbitPoint3D()
		:x(0),y(0),z(0){}
		int x,y,z;
	};
	static orbitPoint3D highestOrbitPoint;           // �O�Տ�ōł������iy���傫���j�_
	/* ���̑� */
	static bool kinectUseFlag; // Kinect���g�����ǂ����Bfalse�̂Ƃ��̓Q�[���݂̂œ��삷��B
	static bool calibModeFlag; // 
	/* ���׌y�� */
	static bool dispCvImageFlag; // cv::Mat��\�����邩�ǂ���
};

#define OPENCV_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define OPENCV_VERSION_CODE OPENCV_VERSION(CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION)

#if OPENCV_VERSION_CODE < OPENCV_VERSION(2,3,1)
namespace cv
{
  enum {
	  EVENT_MOUSEMOVE      =CV_EVENT_MOUSEMOVE,
	  EVENT_LBUTTONDOWN    =CV_EVENT_LBUTTONDOWN,
	  EVENT_RBUTTONDOWN    =CV_EVENT_RBUTTONDOWN,
	  EVENT_MBUTTONDOWN    =CV_EVENT_MBUTTONDOWN,
	  EVENT_LBUTTONUP      =CV_EVENT_LBUTTONUP,
	  EVENT_RBUTTONUP      =CV_EVENT_RBUTTONUP,
	  EVENT_MBUTTONUP      =CV_EVENT_MBUTTONUP,
	  EVENT_LBUTTONDBLCLK  =CV_EVENT_LBUTTONDBLCLK,
	  EVENT_RBUTTONDBLCLK  =CV_EVENT_RBUTTONDBLCLK,
	  EVENT_MBUTTONDBLCLK  =CV_EVENT_MBUTTONDBLCLK
  };
  enum {
	  EVENT_FLAG_LBUTTON   =CV_EVENT_FLAG_LBUTTON,
	  EVENT_FLAG_RBUTTON   =CV_EVENT_FLAG_RBUTTON,
	  EVENT_FLAG_MBUTTON   =CV_EVENT_FLAG_MBUTTON,
	  EVENT_FLAG_CTRLKEY   =CV_EVENT_FLAG_CTRLKEY,
	  EVENT_FLAG_SHIFTKEY  =CV_EVENT_FLAG_SHIFTKEY,
	  EVENT_FLAG_ALTKEY    =CV_EVENT_FLAG_ALTKEY
  };
}
#endif


namespace real
{
	
};
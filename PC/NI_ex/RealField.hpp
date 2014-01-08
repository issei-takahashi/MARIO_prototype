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
	// カメラの初期化
	static void initCamera();
	// 実空間に関する各種パラメータの初期化
	static void initParams();

	// get
	// 左からx番目(X==0,1,2..)の段数を取得
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
	// 深さ画像を更新してDigitalフィールド情報を更新
	static int updateDepthImageAndDigitalFieldInfo();
	// 深さ画像を更新してAnalogフィールド情報を更新
	static int updateDepthImageAndAnalogFieldInfo();
	// 色画像を更新する
	static void updateColorImage();
	// 深さ画像を解析してfieldArrを更新する
	static void updateFieldInfo( int _line );
	// キー入力を元にfieldArrを更新する
	static void updateFieldArrWithoutKinect();
	// 段数をdepthImageに表示
	static void drawDanToDepthImage( int _line );
	// 高さ波形を平滑化
	static void smoothHeightWave(  );
	static void onMouse( int evt, int x, int y, int flag, void* );
	// メディアンフィルタをかける
	static void medianBlur( vector< pair<int,int> > const & _vec, vector< pair<int,int> > & _dst, int _range );

open__:
	static Wave heightWave;   // 高さ波形
	static Wave smoothedWave; // 高さ波形を平滑化したもの
	static Mutex mutex;       // mutex

capcelled__:
	static vector<int> v_blockDan[3];           // 今積まれてるブロックの段数


inner__:
	/* Kinectからとったもの */
	static cv::Mat depthImage;        // 深さ画像
	static cv::Mat colorImage;        // カラー画像
	/* カメラ関連 */
	static openni::VideoStream            depthStream;
	static vector<openni::VideoStream*>   v_pDepthStream;
	static openni::VideoStream            colorStream;
	static vector<openni::VideoStream*>   v_pColorStream;
	static openni::Device                 device;
	/* 実空間のパラメータ */
	static int deskDepth; // 机までの深さ(mm)
	static int blockSide; // ブロックの一辺(mm)
	static int errLen;    // 認識の許容誤差(mm)
	static int blockNumX;          // x方向のブロックの最大数
	static int blockNumY;          // y方向のブロックの最大数
	static int blockNumZ;          // z方向のブロックの最大数
	static vector<int> v_blockSideScr;      // 投影変換後の画面上でのブロックの1辺(key:下から数えたブロックの数)
	static map<int,int> m_highest;        // 最も高い場所一覧
	static vector< pair<int,int> > v_pa_drawOrbit; // 描画可能な場所の軌跡 @x -> (z,y(minDepth))
	class orbitPoint3D
	{
	public:
		orbitPoint3D()
		:x(0),y(0),z(0){}
		int x,y,z;
	};
	static orbitPoint3D highestOrbitPoint;           // 軌跡上で最も高い（yが大きい）点
	/* その他 */
	static bool kinectUseFlag; // Kinectを使うかどうか。falseのときはゲームのみで動作する。
	static bool calibModeFlag; // 
	/* 負荷軽減 */
	static bool dispCvImageFlag; // cv::Matを表示するかどうか
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
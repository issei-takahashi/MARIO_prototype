#include "Mouse.hpp"
#include "Timer.hpp"
#include "IO.hpp"
#include <DxLib.h>

//static変数の定義
Point<int>        Mouse::position;
bool              Mouse::isLeftClickStart  = false;
bool              Mouse::isLeftClickEnd    = false;
double            Mouse::leftClickFrame;
bool              Mouse::isRightClickStart = false;
bool              Mouse::isRightClickEnd   = false;
double            Mouse::rightClickFrame;
bool              Mouse::isWheelClickStart = false;
double            Mouse::wheelClickFrame;
int               Mouse::wheelRot;
bool              Mouse::doesMouseMove     = false;
double            Mouse::lastMouseMoveFrame = 0;
int               Mouse::leftNotClickFrame;              // 左クリックがされていない時間
int               Mouse::rightNotClickFrame;            // 右クリックがされていない時間

#define FPtoIP(p) Point<int>((int)p.x,(int)p.y)
#define IPtoFP(p) Point<float>((float)p.x,(float)p.y)

//マウスの状態更新(毎フレーム行う)
void Mouse::updateMouse()
{
	/* マウスの座標更新 */

	//一次バッファ
	Point<int> tmp;
	DxLib::GetMousePoint( &tmp.x, &tmp.y );


	//座標が更新されていない場合
	if( tmp == position )
	{ 
		doesMouseMove = false;
	}
	//座標が更新されている場合
	el
	{
		doesMouseMove = true;
		position = tmp;
		//座標が更新された時間を記録
		lastMouseMoveFrame = Timer::GetLastTime();
	}
	

	/* 左クリック情報更新 */
	Mouse::isLeftClickStart = false;
	Mouse::isLeftClickEnd = false;
	Mouse::isRightClickStart = false;
	Mouse::isRightClickEnd = false;
	Mouse::isWheelClickStart = false;

	// 押されている
	if( ( DxLib::GetMouseInput() & MOUSE_INPUT_LEFT ) != 0 ) {
		leftNotClickFrame = 0;
		//今押され始めた
		if( leftClickFrame == 0 ) {
			isLeftClickStart = true;
		}
		leftClickFrame ++;
	}
	// 押されていない
	el{
		leftClickFrame = 0;
		// 今離された
		if( leftNotClickFrame == 0 ){
			isLeftClickEnd = true;
		}
		leftNotClickFrame++;
	}


	/* 右クリック情報更新 */
	// 押されている
	if( ( DxLib::GetMouseInput() & MOUSE_INPUT_RIGHT ) != 0 ){
		rightNotClickFrame = 0;
		// 今押され始めた
		if( rightClickFrame == 0 ){
			isRightClickStart = true;
		}
		rightClickFrame ++;
	}
	// 押されていない
	el {
		rightClickFrame = 0;
		// 今離された
		if( rightNotClickFrame == 0 ){
			isRightClickEnd = true;
		}
		rightNotClickFrame++;
	}

	/* ホイールクリック情報更新 */
	if( ( DxLib::GetMouseInput() & MOUSE_INPUT_MIDDLE ) != 0 ) // 押されている
	{
		if( wheelClickFrame == 0 )  //今押され始めた
		{
			isWheelClickStart = true;

		}
		wheelClickFrame ++;
	}
	el // 押されていない
	{
		wheelClickFrame = 0;
	}

	/* ホイール情報更新 */
	wheelRot = DxLib::GetMouseWheelRotVol( ) ;
}

bool Mouse::isInScreen()
{
	static int const SCR_W = IO::getConst("SCR_W"); /// 800px
	static int const SCR_H = IO::getConst("SCR_H"); /// モニタ横幅 378mm
	
	// x方向条件
	if( 0<position.x && position.x<SCR_W ){
		// y方向条件
		if(0<position.y && position.y<SCR_H ){
			return true;
		}
	}
	return false;
}
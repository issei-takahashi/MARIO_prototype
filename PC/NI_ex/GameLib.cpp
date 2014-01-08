#include "GameLib.hpp"
#include "IO.hpp"

#include <DxLib.h> // ここでのみインクルードする

#define HERE GameLib

// ライブラリ初期化
int HERE::initLib()
{

	IO::mutex.lock();
	static int const SCR_W = IO::getConst("SCR_W");
	static int const SCR_H = IO::getConst("SCR_H");
	IO::mutex.unlock();

	//ウィンドウモードにする
	DxLib::ChangeWindowMode(TRUE);
	int ret = DxLib::DxLib_Init();
	if( ret == -1 ){
		exit(1);
		return ret;
	}

	DxLib::SetMainWindowText( "Moving Display" ) ;

	DxLib::SetGraphMode(  SCR_W, SCR_H , 16 ) ;
	//DxLib::SetBackgroundColor(16,16,16);

	// ウィンドウが非アクティブでも動作
	DxLib::SetAlwaysRunFlag(true);


	//裏側描画に設定
	DxLib::SetDrawScreen(DX_SCREEN_BACK);

	// 窓のサイズを可変に
	//DxLib::SetWindowSizeChangeEnableFlag( true ) ;


	// サブモニタに描画するように指定
	int MONITOR_INDEX = IO::getConst("MONITOR_INDEX");
	DxLib::SetUseDirectDrawDeviceIndex( MONITOR_INDEX ) ;

	//マウスを非表示状態にする
	//DxLib::SetMouseDispFlag(false);

	static int const DBG_MODE = (int)IO::getConst("DBG_MODE");
	if( DBG_MODE ){
		// デバッグ用コンソールを呼び出す
		AllocConsole();
		freopen("CONOUT$", "w", stdout); 
		freopen("CONIN$", "r", stdin);
	}

	// フォント初期化
	DxLib::ChangeFontType( DX_FONTTYPE_EDGE ) ;

	// フルスクリーン切り替え時にハンドルをリセットしない
	DxLib::SetChangeScreenModeGraphicsSystemResetFlag( FALSE ) ;

	return ret;
}

// ライブラリ終了
int HERE::endLib()
{
	return DxLib::DxLib_End();
}


/* 描画 */

int HERE::draw::DrawCircle( int _x, int _y, int _r, int _col, int _fillFlag )
{
	return DxLib::DrawCircle(_x,_y,_r,_col,_fillFlag);
}

int HERE::draw::ScreenFlip()
{
	return DxLib::ScreenFlip();
}
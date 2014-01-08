#pragma once

#include "includes.hpp"

#define KEY_NUM 256

/*------------------- Class Keyboard ---------------------------

目的：キーが何フレーム押されているかを保存するクラス
属性：モノステート
使用方法：getKeyCount( keyCode )で、keyCodeに対応するキーが何ms押されているか取得する。

--------------------------------------------------------*/

class Keyboard
{
interface__:
	// キーの入力状態を更新する
	static void updateKey();
	// 特定のキーの入力フレームを取得
	static int getKeyInputFrame( int keynum );
	// 特定のキーの非入力フレームを取得
	static int getKeyInputFrameN( int keynum );
	// 特定のキーが入力開始したかどうかを取得
	static bool getIsKeyInputStart( int keynum );
	// 特定のキーが入力終了したかどうかを取得
	static bool getIsKeyInputEnd( int keynum );

outofuse__:
	//隠しコンストラクタ
	Keyboard(){}

inner__:
	// キーが押されているフレーム数を格納する配列
	static int keyInput[KEY_NUM]; 
	// キーが押されていないフレーム数を格納する配列
	static int keyInputN[KEY_NUM];
};
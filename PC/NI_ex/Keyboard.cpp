#include "Keyboard.hpp"
#include "Timer.hpp"
#include "macro.hpp"
#include <DxLib.h>

//static変数の初期化
int Keyboard::keyInput[KEY_NUM] = {0};
int Keyboard::keyInputN[KEY_NUM] = {0};

// キーの入力状態を更新する
void Keyboard::updateKey()
{
	char tmpKey[KEY_NUM];                    // 現在のキーの入力状態を格納する
	DxLib::GetHitKeyStateAll( tmpKey );             // 全てのキーの入力状態を得る
	for( int i=0; i < KEY_NUM; i++ ){ 
		if( tmpKey[i] != 0 ){            // i番のキーコードに対応するキーが押されていたら
			keyInputN[i] = 0;
			keyInput[i]++;     // 1フレームを加算
			keyInput[i] = min( keyInput[i], macro::KEY_INPUT_FRAME_MAX ); // 大きくなり過ぎないように
		}
		else{                         // 押されていなければ
			keyInput[i] = 0;   // 0にする
			keyInputN[i]++;
			keyInputN[i] = min( keyInputN[i], macro::KEY_INPUT_FRAME_MAX );
		}
	}
}

//keyInputをget
int Keyboard::getKeyInputFrame( int keynum )
{
	assert( keynum < KEY_NUM );
	return keyInput[keynum];
}

//keyInputをget
int Keyboard::getKeyInputFrameN( int keynum )
{
	assert( keynum < KEY_NUM );
	return keyInputN[keynum];
}

// isKeyInputStartをget
bool Keyboard::getIsKeyInputStart( int keynum )
{
	assert( keynum < KEY_NUM );
	return ( keyInput[keynum] == 1 ) ? 1 : 0;
}

// isKeyInputEndをget
bool Keyboard::getIsKeyInputEnd( int keynum )
{
	assert( keynum < KEY_NUM );
	return ( keyInputN[keynum] == 1 ) ? 1 : 0;
}
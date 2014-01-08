#include "SoundManager.hpp"
#include <DxLib.h>

#define HERE SoundManager

map< string, int > HERE::m_soundHandle;

void SoundManager::play( string _fileName )
{
	auto it = HERE::m_soundHandle.find( _fileName );
	if( it == HERE::m_soundHandle.end() ){
		int h = DxLib::LoadSoundMem( _fileName.c_str() );
		HERE::m_soundHandle[_fileName] = h;
	}
	it = HERE::m_soundHandle.find( _fileName );
	DxLib::PlaySoundMem( it->second, DX_PLAYTYPE_BACK );
}
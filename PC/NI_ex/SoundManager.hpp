#pragma once
#include "includes.hpp"

class SoundManager
{
interface__:
	static void play( string _fileName );

inner__:
	static map< string, int > m_soundHandle;
};
#pragma once

#include "includes.hpp"

template<class T>
class SingleMutexer
{
interface__:

	// Mutexをロック
	static void lockMutex()
	{
		
	}
	
	// Mutexをアンロック
	static void unlockMutex()
	{

	}

inner__:
	/* Mutex関連 */
	static HANDLE hMutex; // ミューテックスハンドル
	static bool isMutexLocked; // ミューテックスがロックされているかどうか
};

#define HERE SingleMutexer

HANDLE HERE::hMutex;
bool HERE::isMutexLocked;
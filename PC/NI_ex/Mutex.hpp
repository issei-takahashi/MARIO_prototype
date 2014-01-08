#pragma once
#include "includes.hpp"

class Mutex
{
interface__:
	Mutex()
		:handle( CreateMutex(NULL,FALSE,NULL)), isLocked(false)
	{}
	// Mutexをロック
	void lock()
	{
		if( WaitForSingleObject( this->handle, INFINITE ) == WAIT_FAILED ){ // ミューテックスのロック
			cout << "Mutexのロック失敗" << endl;
			exit(1);
		}
		this->isLocked = true;
	}

	// アンロック
	void unlock()
	{
		if( ReleaseMutex( this->handle ) == 0 ){ // ミューテックスを解放
			cout << "Mutexアンロック失敗" << endl;
			exit(1);
		}
	}

inner__:
	/* スレッド関連 */
	HANDLE handle; // ミューテックスハンドル
	bool isLocked; // ミューテックスがロックされているかどうか
};
#include "SingleMutexer.hpp"

#define HERE SingleMutexer

HERE::SingleMutexer()
	:hMutex( CreateMutex( NULL, FALSE, NULL) ), isMutexLocked(false)
{

}

void HERE::lockMutex()
{
	if( WaitForSingleObject( HERE::hMutex, INFINITE ) == WAIT_FAILED ){ // ミューテックスのロック
		cout << "Mutexのロック失敗" << endl;
		exit(1);
	}
	HERE::isMutexLocked = true;
}

void HERE::unlockMutex()
{
	if( ReleaseMutex( HERE::hMutex ) == 0 ){ // ミューテックスを解放
		cout << "Mutexアンロック失敗" << endl;
		exit(1);
	}
}
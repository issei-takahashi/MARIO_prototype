#pragma once 

#include "includes.hpp"
#include "Mutex.hpp"

class Main
{
interface__:
	static void main();
	static bool get_endFlag(){return endFlag;}
	static void set_endFlag(bool b){mutex.lock();endFlag=b;mutex.unlock();}
inner__:
	static void init();
	static void createAndExecuteThread();

open__:
	static shared_ptr<class ProjCtrlWinClass> pSysProjCtrlWinClass;
	static Mutex mutex;
inner__:
	static bool endFlag;
};

/* 各種スレッド */

// リアルフィールドスレッド
unsigned __stdcall  realFieldThread(void *);
// プロジェクションスレッド
unsigned __stdcall projectionThread(void *);


/*







*/
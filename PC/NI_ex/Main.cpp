#include "Main.hpp"
#include "link.hpp"
#include "IO.hpp"
#include "RealField.hpp"
#include "GameMain.hpp"
#include "GameLib.hpp"
// windows�X���b�h�֘A
#include <windows.h>
#include <process.h>
#include "MemMapFile.hpp"
// DDT
#include "ProjecterControl.h"

#define HERE Main

shared_ptr<class ProjCtrlWinClass> Main::pSysProjCtrlWinClass=NULL;
bool Main::endFlag = false;
Mutex Main::mutex;

static HINSTANCE hinst;
int WINAPI WinMain(HINSTANCE _hinst,HINSTANCE,LPSTR,int)
{
	::hinst = _hinst;
	Main::main(); 
	return 0; 
}

void HERE::main()
{

	HERE::init();
	
	try{
		HERE::createAndExecuteThread();
	}
	catch(...){
		cout << "Main::main() error" << endl;
		exit(1);
	}

	//MemMapFile mmfile;
	//mmfile.openFile( "data/testmm.txt" );
	//iostream* pStream = mmfile.getFileStream();
	//*pStream << "aaaaaaaa" << endl;
}

void HERE::init()
{
	IO::loadAllData();
	RealField::initCamera();
	RealField::initParams();
	GameLib::initLib();
}

void HERE::createAndExecuteThread()
{
	HANDLE hThread;     // �X���b�h�̃n���h��
	unsigned int thID;  // �X���b�h��ID


	/* Kinect�֘A�̃X���b�h */
	if( ( hThread = (HANDLE)_beginthreadex( NULL, 0, ::realFieldThread, NULL, CREATE_SUSPENDED, &thID ) ) == 0 ){  
		cout << "Main.cpp \t ���A���t�B�[���h�X���b�h�쐬���s" << endl;
		exit(0);
	}
	ResumeThread( hThread );

	/* �v���W�F�N�^�֘A�̃X���b�h */
	HERE::pSysProjCtrlWinClass = shared_ptr<ProjCtrlWinClass> (new ProjCtrlWinClass( ::hinst ) );
	DWORD dwThId;
	Thread proj_thrd(ProjCtrlFuncThread, &dwThId);
	
	try{
		GameMain::mainLoop();
	}
	catch( std::exception & exp ){
		cout << exp.what() << endl;
	}

	// �ҋ@
	//WaitForMultipleObjects( 2, hThread, true, INFINITE );

	//�n���h���N���[�Y
	CloseHandle( hThread );
	

}

/* �e�X���b�h */

// ���A���t�B�[���h�X���b�h
unsigned __stdcall realFieldThread(void *)
{
	try{
		RealField::mainLoop();
	}
	catch( std::exception & exp ){
		cout << exp.what() << endl;
	}
	return 0;
}

// �v���W�F�N�V�����X���b�h
unsigned __stdcall projectionThread(void *)
{
	while(1){
		break;
	}
	return 0;
}
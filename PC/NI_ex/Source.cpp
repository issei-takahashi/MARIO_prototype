#include <windows.h>
#include "ProjecterControl.h"
#include <DxLib.h>

int WINAPI aaWinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int nCmdShow)
{
	// kore
	DDT::SysProjCtrlWinClass = std::shared_ptr<ProjCtrlWinClass> (new ProjCtrlWinClass( hinst ) );
	DWORD dwThId;
	Thread proj_thrd(ProjCtrlFuncThread, &dwThId);
	



	ChangeWindowMode( TRUE );
	DxLib_Init();
	static int s[3] = {50,50,50};
	static int a[3] = {50,50,50};
	while(1)
	{
		if(	ProcessMessage()  == -1 )
		{
			break;
		}
		if( GetMouseInput() && MOUSE_INPUT_LEFT )
		{
			static int f = 0;
			++f;
			f %= 120;
			if( f== 0 )
			{
				++a[0];
				a[1];
				//++a[2];
			}
			if( a[0] >= 250 )
			{
				a[0] = 50 ;
			}
		}
		if( CheckHitKey(KEY_INPUT_R))
		{
			a[0] = s[0];
			a[1] = s[1];
			a[2] = s[2];
		}

		/// kore
		DDT::SysProjCtrlWinClass->SendMessageToProjecter( 12 , &a );

	}
	DxLib_End();
	return 0;
}


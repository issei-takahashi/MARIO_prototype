#pragma once

#include "WinClass.h"
#include "Thread.h"
#include "Singleton.h"
#include <memory>

LRESULT CALLBACK ProjWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD CALLBACK ProjCtrlFuncThread(LPVOID args);

class ProjCtrlWinClass : public DDT::WinClass
{
public:
	ProjCtrlWinClass(HINSTANCE hinst)
		: WinClass(TEXT("Proj_Ctrl") , TEXT("Projecter Control Window") , hinst , ProjWindowProc )
	{};

	void SendMessageToProjecter( size_t size , PVOID pData )
	{
		///WinClass::SendMessageToClass((LPCSTR) TEXT("Proj_Window_Class") , size , pData); /// DDT
		WinClass::SendMessageToClass( L"Proj_Window_Class" , size , pData);
	}

};

//typedef Loki::SingletonHolder<ProjCtrlWinClass> SysProjCtrlWinClass;
namespace DDT
{
	static std::shared_ptr<ProjCtrlWinClass> SysProjCtrlWinClass = NULL;
}


static DWORD WINAPI ProjCtrlFuncThread(LPVOID args)
{

	MSG        msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


static LRESULT CALLBACK ProjWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndTarget = NULL;

	switch (uMsg) {

	case WM_CREATE:
		{
			return 0;
		}

	case WM_USER:
		{
			DDT::SysProjCtrlWinClass->SendMessageToProjecter( (int) wParam, (void*) lParam );
		}


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
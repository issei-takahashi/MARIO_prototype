#pragma once

#include <Windows.h>
#include <tchar.h>

namespace DDT
{

	class ERROR_INIT
	{
	public:
		ERROR_INIT()
		{

		}
	};

	class WinClass
	{
	public:
		TCHAR lpClassName[1000];
		TCHAR lpWindowName[1000];
		HWND       hwnd;
		MSG        msg;
		WNDCLASSEX wc;

	private:
		WinClass();

	public:
		WinClass( TCHAR class_name[] , TCHAR window_name[] , HINSTANCE hinst , LRESULT CALLBACK WindowProc(HWND , UINT, WPARAM , LPARAM) )
		{
			lstrcpy( this->lpClassName  , class_name  );
			lstrcpy( this->lpWindowName , window_name );

			this->wc.cbSize        = sizeof(WNDCLASSEX);
			this->wc.style         = 0;
			this->wc.lpfnWndProc   = WindowProc;
			this->wc.cbClsExtra    = 0;
			this->wc.cbWndExtra    = 0;
			this->wc.hInstance     = hinst;
			this->wc.hIcon         = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
			this->wc.hCursor       = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
			this->wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			this->wc.lpszMenuName  = NULL;
			this->wc.lpszClassName = lpClassName;
			this->wc.hIconSm       = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);

			if (RegisterClassEx(&(this->wc)) == 0)
				throw ERROR_INIT();

			hwnd = CreateWindowEx
				( 0, this->lpClassName, this->lpWindowName
				, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hinst, NULL);

			if (hwnd == NULL)
				throw ERROR_INIT();
		}

		void ShowWindow(int nCmdShow)
		{
			::ShowWindow(hwnd, nCmdShow);
			UpdateWindow(hwnd);
		}

		LRESULT SendMessageToClass( LPCWSTR target_class_name , size_t size , PVOID pData )
		{
			///HWND hwndTarget = FindWindow( target_class_name , NULL); /// DDT
			HWND hwndTarget = FindWindowW( target_class_name , NULL);   /// Sekai
			if (hwndTarget != NULL)
			{
				//MessageBox(NULL, TEXT("Projecter Windowにメッセージを送信します。"), NULL, MB_ICONWARNING);

				COPYDATASTRUCT data;
				data.dwData = 0; // tekito
				data.cbData = size;
				data.lpData = pData;
				return SendMessage( hwndTarget, WM_COPYDATA , (WPARAM) this->hwnd, (LPARAM) &data);
			}
			else
			{ 
				//MessageBox(NULL, TEXT("プロジェクタのウィンドウを見つけることができませんでした。先にプロジェクタのプログラムを実行してください。"), NULL, MB_ICONWARNING);
				//exit(1);
				return 0;
			}

		}

	};

}
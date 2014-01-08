#pragma once

#include <Windows.h>


class ERROR_INIT
{
public:
	ERROR_INIT(){};
};

class Window
{
private:
	Window();

private:
	WNDCLASSEX wc;

public:
	HWND hWnd;

	Window( LPCSTR class_name , LPCSTR window_name , LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM) ,  HINSTANCE hInst )
	{
		if( init( class_name , window_name , WndProc , hInst ) == 1 )
			throw ERROR_INIT();
	}

	int init( LPCSTR class_name , LPCSTR window_name , LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM) ,  HINSTANCE hInst )
	{
		// ウィンドウクラスの情報を設定
		wc.cbSize = sizeof(wc);               // 構造体サイズ
		wc.style = CS_HREDRAW | CS_VREDRAW;   // スタイル
		wc.lpfnWndProc = WndProc;             // ウィンドウプロシージャ
		wc.cbClsExtra = 0;                    // 拡張情報１
		wc.cbWndExtra = 0;                    // 拡張情報２
		wc.hInstance = hInst;                 // インスタンスハンドル
		wc.hIcon = (HICON)LoadImage(          // アイコン
			NULL, MAKEINTRESOURCE(IDI_APPLICATION), IMAGE_ICON,
			0, 0, LR_DEFAULTSIZE | LR_SHARED
			);
		wc.hIconSm = wc.hIcon;                // 子アイコン
		wc.hCursor = (HCURSOR)LoadImage(      // マウスカーソル
			NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
			0, 0, LR_DEFAULTSIZE | LR_SHARED
			);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // ウィンドウ背景
		wc.lpszMenuName = NULL;                     // メニュー名
		wc.lpszClassName = class_name;// ウィンドウクラス名

		// ウィンドウクラスを登録する
		if( RegisterClassEx( &wc ) == 0 ){ return 1; }

		// ウィンドウを作成する
		hWnd = CreateWindow(
			wc.lpszClassName,      // ウィンドウクラス名
			window_name,  // タイトルバーに表示する文字列
			WS_OVERLAPPEDWINDOW,   // ウィンドウの種類
			CW_USEDEFAULT,         // ウィンドウを表示する位置（X座標）
			CW_USEDEFAULT,         // ウィンドウを表示する位置（Y座標）
			CW_USEDEFAULT,         // ウィンドウの幅
			CW_USEDEFAULT,         // ウィンドウの高さ
			NULL,                  // 親ウィンドウのウィンドウハンドル
			NULL,                  // メニューハンドル
			hInst,                 // インスタンスハンドル
			NULL                   // その他の作成データ
			);
		if( hWnd == NULL ){ return 1; }

		// ウィンドウを表示する
		ShowWindow( hWnd, SW_SHOW );
		UpdateWindow( hWnd );

		return 0;
	}

};
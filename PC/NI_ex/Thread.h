#pragma once

#include <Windows.h>
#include <iostream>

class Thread
{
public:
	HANDLE hThread;

public:
	Thread( DWORD WINAPI funcThread(LPVOID) , DWORD* thID )
	{
		HANDLE hThread = CreateThread
			( NULL, 0 , funcThread
			, NULL, CREATE_SUSPENDED, thID
			);
		if( hThread == 0 )
		{
			std::cout << "Miss to create thread." << std::endl;
			exit(0);
		}

		ResumeThread( hThread );
	}
};
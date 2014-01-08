#pragma once
#include "includes.hpp"

class Mutex
{
interface__:
	Mutex()
		:handle( CreateMutex(NULL,FALSE,NULL)), isLocked(false)
	{}
	// Mutex�����b�N
	void lock()
	{
		if( WaitForSingleObject( this->handle, INFINITE ) == WAIT_FAILED ){ // �~���[�e�b�N�X�̃��b�N
			cout << "Mutex�̃��b�N���s" << endl;
			exit(1);
		}
		this->isLocked = true;
	}

	// �A�����b�N
	void unlock()
	{
		if( ReleaseMutex( this->handle ) == 0 ){ // �~���[�e�b�N�X�����
			cout << "Mutex�A�����b�N���s" << endl;
			exit(1);
		}
	}

inner__:
	/* �X���b�h�֘A */
	HANDLE handle; // �~���[�e�b�N�X�n���h��
	bool isLocked; // �~���[�e�b�N�X�����b�N����Ă��邩�ǂ���
};
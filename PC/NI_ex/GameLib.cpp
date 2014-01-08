#include "GameLib.hpp"
#include "IO.hpp"

#include <DxLib.h> // �����ł̂݃C���N���[�h����

#define HERE GameLib

// ���C�u����������
int HERE::initLib()
{

	IO::mutex.lock();
	static int const SCR_W = IO::getConst("SCR_W");
	static int const SCR_H = IO::getConst("SCR_H");
	IO::mutex.unlock();

	//�E�B���h�E���[�h�ɂ���
	DxLib::ChangeWindowMode(TRUE);
	int ret = DxLib::DxLib_Init();
	if( ret == -1 ){
		exit(1);
		return ret;
	}

	DxLib::SetMainWindowText( "Moving Display" ) ;

	DxLib::SetGraphMode(  SCR_W, SCR_H , 16 ) ;
	//DxLib::SetBackgroundColor(16,16,16);

	// �E�B���h�E����A�N�e�B�u�ł�����
	DxLib::SetAlwaysRunFlag(true);


	//�����`��ɐݒ�
	DxLib::SetDrawScreen(DX_SCREEN_BACK);

	// ���̃T�C�Y���ς�
	//DxLib::SetWindowSizeChangeEnableFlag( true ) ;


	// �T�u���j�^�ɕ`�悷��悤�Ɏw��
	int MONITOR_INDEX = IO::getConst("MONITOR_INDEX");
	DxLib::SetUseDirectDrawDeviceIndex( MONITOR_INDEX ) ;

	//�}�E�X���\����Ԃɂ���
	//DxLib::SetMouseDispFlag(false);

	static int const DBG_MODE = (int)IO::getConst("DBG_MODE");
	if( DBG_MODE ){
		// �f�o�b�O�p�R���\�[�����Ăяo��
		AllocConsole();
		freopen("CONOUT$", "w", stdout); 
		freopen("CONIN$", "r", stdin);
	}

	// �t�H���g������
	DxLib::ChangeFontType( DX_FONTTYPE_EDGE ) ;

	// �t���X�N���[���؂�ւ����Ƀn���h�������Z�b�g���Ȃ�
	DxLib::SetChangeScreenModeGraphicsSystemResetFlag( FALSE ) ;

	return ret;
}

// ���C�u�����I��
int HERE::endLib()
{
	return DxLib::DxLib_End();
}


/* �`�� */

int HERE::draw::DrawCircle( int _x, int _y, int _r, int _col, int _fillFlag )
{
	return DxLib::DrawCircle(_x,_y,_r,_col,_fillFlag);
}

int HERE::draw::ScreenFlip()
{
	return DxLib::ScreenFlip();
}
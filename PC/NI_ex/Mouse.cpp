#include "Mouse.hpp"
#include "Timer.hpp"
#include "IO.hpp"
#include <DxLib.h>

//static�ϐ��̒�`
Point<int>        Mouse::position;
bool              Mouse::isLeftClickStart  = false;
bool              Mouse::isLeftClickEnd    = false;
double            Mouse::leftClickFrame;
bool              Mouse::isRightClickStart = false;
bool              Mouse::isRightClickEnd   = false;
double            Mouse::rightClickFrame;
bool              Mouse::isWheelClickStart = false;
double            Mouse::wheelClickFrame;
int               Mouse::wheelRot;
bool              Mouse::doesMouseMove     = false;
double            Mouse::lastMouseMoveFrame = 0;
int               Mouse::leftNotClickFrame;              // ���N���b�N������Ă��Ȃ�����
int               Mouse::rightNotClickFrame;            // �E�N���b�N������Ă��Ȃ�����

#define FPtoIP(p) Point<int>((int)p.x,(int)p.y)
#define IPtoFP(p) Point<float>((float)p.x,(float)p.y)

//�}�E�X�̏�ԍX�V(���t���[���s��)
void Mouse::updateMouse()
{
	/* �}�E�X�̍��W�X�V */

	//�ꎟ�o�b�t�@
	Point<int> tmp;
	DxLib::GetMousePoint( &tmp.x, &tmp.y );


	//���W���X�V����Ă��Ȃ��ꍇ
	if( tmp == position )
	{ 
		doesMouseMove = false;
	}
	//���W���X�V����Ă���ꍇ
	el
	{
		doesMouseMove = true;
		position = tmp;
		//���W���X�V���ꂽ���Ԃ��L�^
		lastMouseMoveFrame = Timer::GetLastTime();
	}
	

	/* ���N���b�N���X�V */
	Mouse::isLeftClickStart = false;
	Mouse::isLeftClickEnd = false;
	Mouse::isRightClickStart = false;
	Mouse::isRightClickEnd = false;
	Mouse::isWheelClickStart = false;

	// ������Ă���
	if( ( DxLib::GetMouseInput() & MOUSE_INPUT_LEFT ) != 0 ) {
		leftNotClickFrame = 0;
		//��������n�߂�
		if( leftClickFrame == 0 ) {
			isLeftClickStart = true;
		}
		leftClickFrame ++;
	}
	// ������Ă��Ȃ�
	el{
		leftClickFrame = 0;
		// �������ꂽ
		if( leftNotClickFrame == 0 ){
			isLeftClickEnd = true;
		}
		leftNotClickFrame++;
	}


	/* �E�N���b�N���X�V */
	// ������Ă���
	if( ( DxLib::GetMouseInput() & MOUSE_INPUT_RIGHT ) != 0 ){
		rightNotClickFrame = 0;
		// ��������n�߂�
		if( rightClickFrame == 0 ){
			isRightClickStart = true;
		}
		rightClickFrame ++;
	}
	// ������Ă��Ȃ�
	el {
		rightClickFrame = 0;
		// �������ꂽ
		if( rightNotClickFrame == 0 ){
			isRightClickEnd = true;
		}
		rightNotClickFrame++;
	}

	/* �z�C�[���N���b�N���X�V */
	if( ( DxLib::GetMouseInput() & MOUSE_INPUT_MIDDLE ) != 0 ) // ������Ă���
	{
		if( wheelClickFrame == 0 )  //��������n�߂�
		{
			isWheelClickStart = true;

		}
		wheelClickFrame ++;
	}
	el // ������Ă��Ȃ�
	{
		wheelClickFrame = 0;
	}

	/* �z�C�[�����X�V */
	wheelRot = DxLib::GetMouseWheelRotVol( ) ;
}

bool Mouse::isInScreen()
{
	static int const SCR_W = IO::getConst("SCR_W"); /// 800px
	static int const SCR_H = IO::getConst("SCR_H"); /// ���j�^���� 378mm
	
	// x��������
	if( 0<position.x && position.x<SCR_W ){
		// y��������
		if(0<position.y && position.y<SCR_H ){
			return true;
		}
	}
	return false;
}
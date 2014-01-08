#pragma once

#include "includes.hpp"
#include "Point.hpp"


/*----------------------class Mouse------------------------------

�ړI�F�}�E�X�̈ʒu�A�N���b�N���A�z�C�[�������擾����N���X�B

�����F���m�X�e�[�g

--------------------------------------------------------------------*/

class Mouse
{
	friend class EditorUI;


public:

	//�}�E�X�̏�ԍX�V(���t���[���s��)
	static void updateMouse();
	// ��ʓ��ɂ��邩�ǂ���
	static bool isInScreen();

	/* get */
	static Point<int> get_position() { return Mouse::position; }
	static bool get_isLeftClickStart() { return Mouse::isLeftClickStart; }
	static bool get_isLeftClickEnd() { return Mouse::isLeftClickEnd; }
	static double get_leftClickFrame() { return Mouse::leftClickFrame; }
	static bool get_isRightClickStart() { return Mouse::isRightClickStart; }
	static bool get_isRightClickEnd() { return Mouse::isRightClickEnd; }
	static double get_rightClickFrame() { return Mouse::rightClickFrame; }
	static bool get_isWheelClickStart() { return Mouse::isWheelClickStart; }
	static double get_wheelClickTime() { return Mouse::wheelClickFrame; }
	static int get_wheelRot() { return Mouse::wheelRot; }
	static bool get_doesMouseMove() { return Mouse::doesMouseMove; }
	static double get_lastMouseMoveFrame() { return Mouse::lastMouseMoveFrame; }

capcelled__:
	static Point<int> position;                // �}�E�X�̍��W
	static bool isLeftClickStart;              // ���N���b�N���n�܂����t���[�����ǂ���
	static bool isLeftClickEnd;                // ���N���b�N���I������t���[��������
	static double leftClickFrame;               // ���N���b�N����Ă��鎞��(s)
	static bool isRightClickStart;             // �E�N���b�N���n�܂����t���[�����ǂ���
	static bool isRightClickEnd;               // �E�N���b�N���I������t���[�����ǂ���
	static double rightClickFrame;              // �E�N���b�N����Ă��鎞��(s)
	static bool isWheelClickStart;             // �z�C�[���N���b�N���n�܂����t���[�����ǂ���
	static double wheelClickFrame;              // �E�N���b�N����Ă��鎞��(s)
	static int wheelRot;                       // �z�C�[����]��
	static bool doesMouseMove;                 // ���t���[���Ƀ}�E�X�����������ǂ���
	static double lastMouseMoveFrame;           // �Ō�Ƀ}�E�X���ړ���������
	static int leftNotClickFrame;              // ���N���b�N������Ă��Ȃ�����
	static int rightNotClickFrame;            // �E�N���b�N������Ă��Ȃ�����

};
#pragma once
#include "includes.hpp"
#include "Rect.hpp"
#include "Singleton.hpp"
#include "Point3D_mm.h"

enum eGameState{ GS_FIELD, GS_ANALOG, GS_FIELD3D };

class GameMain
{
public:
	// �Q�[���̃��C�����[�v
	static void mainLoop();
	static void changeState( class GameState * _pState );
	static eGameState getGameState();
inner__:
	static class GameState* pState;
};


class GameState
{
interface__:
	GameState();
	virtual void mainFlow() = 0;
	virtual eGameState getGameState() const = 0;
protected:
};

// �J�n���
class TitleState : public GameState, public Singleton<TitleState>
{
	friend class Singleton<TitleState>;
interface__:
	void mainFlow();
};

// �t�B�[���h
class FieldState : public GameState, public Singleton<FieldState>
{
	friend class Singleton<FieldState>;
	friend class AnalogState;
	friend class FieldState3D;
interface__:
	FieldState() : calibFlag(false), blockDispFlag(true), depthWaveDispFlag(true), line(0) {}
	// @ovr ���C���t���[
	void mainFlow();
	// @ovr
	eGameState getGameState() const { return GS_FIELD; }

	// ����̏ꏊ�̒i�����擾
	int getDan( int _line, int _x ) const;
	// RealField�̒i���ɂ��ƂÂ��ău���b�N�ꗗ���X�V
	void updateBlock();
	// Mover�ƃu���b�N�Ƃ̓����蔻���S�T���ōs���đ��x��␳����
	bool hitBlockAndChangeVelEvent( class Mover* _pMover );
	// Chara��Object�Ƃ̓����蔻���S�T���ōs��
	bool hitObjectEvent( class Chara * _pChara );

inner__:
	// �L�[����
	void keyInputAction();
	// �u���b�N�`��
	void drawBlock();
	// Chara���W�F�l���[�g
	void generateChara( int _num, Point<int> _pos );
	// Object���W�F�l���[�g
	void generateObject( int _index, Point<int> _pos );
	// �S�Ă�Mover�����s
	void executeAllMovers();
	// ������p���ăI�u�W�F�N�g���W�F�l���[�g
	void generateObjectEvent();

inner__:
	bool calibFlag; // �L�����u���[�V�������[�h���ǂ���
	bool blockDispFlag; // �u���b�N��\�����邩�ǂ���
	bool depthWaveDispFlag; // �[���g�`��\�����邩�ǂ���
	map<int,sp<class Chara> > m_spChara; // ���̈ꗗ
	map< class Object *, sp<class Object> > m_spObject; // �I�u�W�F�N�g�ꗗ
	vector<vector<class Block> > v_v_Block[3]; // �u���b�N�ꗗ
	int line;    // �����郉�C��
};

// �A�i���O�F�����[�h
class AnalogState : public GameState, public Singleton<AnalogState>
{
	friend class Singleton<AnalogState>;
interface__:
	AnalogState();
	// @ovr 
	void mainFlow();
	// @ovr
	eGameState getGameState() const { return GS_ANALOG; }

inner__:
	void loadAndPrepareMovie();
	void updateHighestPoint();
	void drawMovie();

inner__:
	Point<int> highestPoint;
	map<string,sp<class Image> > m_spImage;
};

// ���̔F�����[�h
class FieldState3D : public GameState
{
interface__:
	FieldState3D();
	// @ovr 
	void mainFlow();
	// @ovr
	eGameState getGameState() const { return GS_FIELD3D; }

protected:
	virtual void showChara() = 0;
	void keyAndMouseInputAction();
	virtual void init() = 0;
	static void initArduino();

protected:
	bool initDoneFlag;   // �������ł��Ă��邩�ǂ���
	bool calibModeFlag; // �L�����u���[�V�������[�h���ǂ����̃t���O
	static shared_ptr<class WinRS>  pPort; // Arduino�p�̃|�[�g
	shared_ptr<class Chara> pHiyoko; // �Ђ悱
	Point<int> clickedPoint;         // ���N���b�N���ꂽ�ꏊ
	map< string, int > m_soundHandle;
};

// ���̔F�����[�h�i�W���p�j
class FieldState3D_show : public FieldState3D, public Singleton<FieldState3D_show>
{
	friend class Singleton<FieldState3D_show>;
interface__:
	FieldState3D_show();
inner__:
	enum eShowType{ ORBIT, JUMP };
	eShowType showType;
	Point3D_mm getObjectPoint( eShowType );
	// @ovr
	void showChara();
	void init();
inner__:
	int onBlockCount;
	int flyingCount;
	Point3D_mm charaVel;
	Point3D_mm charaPos;
	Point3D_mm nextPoint;
};

// ���̔F�����[�h�i�_���̂��߂̎B�e�p�j
class FieldState3D_essay : public FieldState3D, public Singleton<FieldState3D_essay>
{
	friend class Singleton<FieldState3D_essay>;
interface__:
	FieldState3D_essay();
inner__:
	// @ovr
	void showChara();
	void init();
	// Hiyoko��Fire���ɉ����ĕς���
	enum eShowObjectType { HIYOKO, FIRE } showObjectType;
	Point3D_mm getObjectPoint( eShowObjectType );
inner__:
	vector< Point3D_mm > v_Points;
	vector< Point3D_mm >::iterator it_point;
	Point3D_mm charaPos; // �L�����N�^�[�̌��݈ʒu
	Point3D_mm charaVel; // �L�����N�^�[�̌��݂̑��x�imm/frame�j
	int onBlockCount;   // �u���b�N�̏�ɂ���b���̃J�E���g
	int flyingCount;    // ���ł���b���̃J�E���g
	// ���֘A
	shared_ptr< class Image > pFireImage;
	Point3D_mm firePos; // ���̈ʒu
	double fireAngle_rad;
};
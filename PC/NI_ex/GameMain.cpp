#include "GameMain.hpp"
#include "RealField.hpp"
#include "GameLib.hpp"
#include "IO.hpp"
#include "Keyboard.hpp"
#include "Mover.hpp"
#include "Block.hpp"
#include "Camera.hpp"
#include "Image.hpp"
#include "Mouse.hpp"
#include <DxLib.h>
#include "MyDxLib.hpp"
#include "ProjecterControl.h"
#include "Main.hpp"
#include "Arduino/winrs.h"
#include "utils.hpp"
#include "macro.hpp"

#define BLOCK_PIX 100
#define BLOCK_CENTER(X,Y) Point<int>((int)(BLOCK_PIX*(X+0.5)),5*BLOCK_PIX+(int)(-BLOCK_PIX*(Y+0.5)))
#define SIZE(x,y) sqrtf((float)(x*x+y*y))
#define PM(X) (X>=0)?+1.0:-1.0

GameState* GameMain::pState = FieldState3D_show::get();
shared_ptr<class WinRS>  FieldState3D::pPort = NULL; // Arduino�p�̃|�[�g


void GameMain::mainLoop()
{
	while( ProcessMessage()==0 && Main::get_endFlag() == false ){
		Keyboard::updateKey();
		Mouse::updateMouse();
		GameMain::pState->mainFlow();
	}
}

void GameMain::changeState( class GameState * _pState )
{
	GameMain::pState = _pState;
}

eGameState GameMain::getGameState()
{
	passert("GameState��pState��NULL�ł�",GameMain::pState!=NULL);
	return GameMain::pState->getGameState();
}

GameState::GameState()
{

}

void TitleState::mainFlow()
{

}

void FieldState::mainFlow()
{

	DxLib::ClearDrawScreen();
	DxLib::clsDx();

	this->generateObjectEvent();
	this->keyInputAction();
	this->executeAllMovers();
	this->updateBlock();

	if( this->blockDispFlag ){
		this->drawBlock();
	}

	DxLib::ScreenFlip();
}

// ����̏ꏊ�̒i�����擾
int FieldState::getDan( int _line, int _x ) const
{
	return RealField::getDan( _line, _x );
}

// RealField�̒i���ɂ��ƂÂ��ău���b�N�ꗗ���X�V
void FieldState::updateBlock()
{
	static int const blockNumX = (int)IO::getConst("BLOCK_NUM_X");
	static Block blockBuf;
	
	try{
	for(int i=0; i<3; i++){
		this->v_v_Block[i].clear();
		for( int x = 0; x < blockNumX; x++ ){
			vector<Block> tmpv;
			for( int y = -1; y < RealField::getDan(i,x); y++ ){
				blockBuf.p1 = Point<int>((int)(+BLOCK_PIX*x)    , 5*BLOCK_PIX+(int)(-BLOCK_PIX*(y+1)));
				blockBuf.p2 = Point<int>((int)(BLOCK_PIX*(x+1)), 5*BLOCK_PIX+(int)(-BLOCK_PIX*y));
				tmpv.push_back( blockBuf );
			}
			this->v_v_Block[i].push_back( tmpv );
		}
	}
	}
	catch(std::exception& exp){
		cout << exp.what() << endl;
	}
}

// Mover�ƃu���b�N�Ƃ̓����蔻���S�T���ōs���đ��x��␳����
bool FieldState::hitBlockAndChangeVelEvent( Mover* _pMover )
{
	bool ret = false;

	/* ���ɂ��Ă̓����蔻�� */
	static Rect<int> myRect1,myRect2;
	myRect1 = _pMover->getBodyRectBeforeVelAdded();
	myRect2 = _pMover->getBodyRectAfterVelAdded();

	//cout << _pMover->pos.x << "," << _pMover->pos.y << endl;

	/// ���܂��郉�C���ɂ���
	/* �S�Ẵu���b�N�ɂ��Ă̓����蔻�� */
	foreach(it,this->v_v_Block[this->line]){
		foreach(it2,*it){
			int locTypeAfter = myRect2.getLocationType(*it2);
			// �����蔻��
			if((locTypeAfter&LOC_UDHIT)&&(locTypeAfter&LOC_LRHIT)){
				ret = true;
				int locTypeBefore = myRect1.getLocationType(*it2);
				/* ���n�̃P�[�X */
				if( locTypeBefore&LOC_UP ){
					//_pMover->setStandingBlockAddr(&*it2);
					_pMover->vel.y -= (myRect2.p2.y-it2->p1.y);
				}
				/* �����Ă���P�[�X */
				ef( locTypeBefore&LOC_UP_FIT ){
					_pMover->vel.y = 0;
				}
				/* ������q�b�g�̃P�[�X */
				ef( (locTypeBefore&LOC_DOWN) || (locTypeBefore&LOC_DOWN_FIT) ){
					_pMover->vel.y += (it2->p2.y-myRect2.p1.y);
				}
				/* ������q�b�g�̃P�[�X */
				ef( (locTypeBefore&LOC_LEFT) || (locTypeBefore&LOC_LEFT_FIT) ){
					_pMover->vel.x -= (myRect2.p2.x-it2->p1.x);
				}
				/* �E����q�b�g�̃P�[�X */
				ef( (locTypeBefore&LOC_RIGHT) || (locTypeBefore&LOC_RIGHT_FIT) ){
					_pMover->vel.x += (it2->p2.x-myRect2.p1.x);
				}
				el{
					//passert("��`����Ă��Ȃ������蔻��ł�",0);
				}
			}
		}
	}

	return ret;
}

// Chara��Object�Ƃ̓����蔻���S�T���ōs��
bool FieldState::hitObjectEvent( class Chara * _pChara )
{
	bool ret = false;

	static Rect<int> myRect2;
	myRect2 = _pChara->getBodyRectAfterVelAdded();

	/* �S�Ă�Object�ɂ��Ă̓����蔻�� */
	for(auto it=this->m_spObject.begin(); it!=this->m_spObject.end(); ){
		int locTypeAfter = myRect2.getLocationType(it->second->getBodyRectAfterVelAdded());
		// �����蔻��
		if((locTypeAfter&LOC_UDHIT)&&(locTypeAfter&LOC_LRHIT)){
			it->second->hitAction(_pChara);
			it = this->m_spObject.erase( it );
			ret = true;
		}
		el{
			it++;
		}
	}
	return ret;
}

// �u���b�N�`��
void FieldState::drawBlock()
{
	foreach(it,this->v_v_Block[this->line]){
		foreach(it2,*it){
			it2->libDraw();
			
		}
	}
}

// Mover���W�F�l���[�g
void FieldState::generateChara( int _index, Point<int> _pos )
{
	this->m_spChara[_index] = (sp<Chara>)(new Chara(_index,_pos));
}

// Object���W�F�l���[�g
void FieldState::generateObject( int _index, Point<int> _pos )
{
	sp<Object> p = (sp<Object>)(new Object(_index,_pos));
	this->m_spObject[ p.get() ] = p;
}

// �S�Ă�Mover�����s
void FieldState::executeAllMovers()
{
	static int count = 0;
	foreach(it,this->m_spChara){
		it->second->mainFlow();
		this->hitObjectEvent(it->second.get());
	}
	foreach(it,this->m_spObject){
		it->second->mainFlow();
	}
}

// ������p���ăI�u�W�F�N�g���W�F�l���[�g
void FieldState::generateObjectEvent()
{
	switch( rand() % 53 ){
	case 0: // �R�C��
		{
			static int const SCR_W = IO::getConst("SCR_W");
			this->generateObject(4,Point<int>(rand()%SCR_W,0));
			break;
		}
	}
}

void FieldState::keyInputAction()
{
	// W�L�[�F�E�B���h�E���[�h�؂�ւ�
	if( Keyboard::getIsKeyInputStart(KEY_INPUT_W) ){
		static bool windowFlag = TRUE;
		windowFlag = !windowFlag;
		DxLib::ChangeWindowMode(windowFlag);
	}
	// S�L�[�F�R���t�B�O�f�[�^�Z�[�u
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_S) ){
		IO::saveConfigData();
	}
	// C�L�[�F�L�����u���[�V�������[�h�ύX
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_C) ){
		this->calibFlag = !this->calibFlag;
	}
	// B�L�[�F�u���b�N�\���؂�ւ�
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_B) ){
		this->blockDispFlag = !this->blockDispFlag;
	}
	// L�L�[�F�f�v�X�g�`�\���ؑ�
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_L) ){
		this->depthWaveDispFlag = ! this->depthWaveDispFlag;
	}
	// 1�L�[�FMover 1���W�F�l���[�g
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_1) ){
		this->generateChara(1,Point<int>(0,0));
	}
	// 2�L�[�FMover 2���W�F�l���[�g
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_2) ){
		this->generateChara(2,Point<int>(50,0));
	}
	// 3�L�[�FMover 3���W�F�l���[�g
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_3) ){
		this->generateChara(3,Point<int>(150,0));
	}
	// A�L�[�FAnalogState�ɐ؂�ւ�
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_A) ){
		GameMain::changeState( AnalogState::get() );
	}
	// F�L�[�FFieldState�ɐ؂�ւ�
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_F) ){
		GameMain::changeState( FieldState::get() );
	}
	// D�L�[�FFieldState3D�ɐ؂�ւ�
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_D) ){
		GameMain::changeState( FieldState3D_show::get() );
	}
	/// �̏ᒆ...
	//// I�L�[�Fopencv��Image��disp���邩�ǂ����̐؂�ւ�
	//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_I) ){
	//	RealField::inv_dispCvImageFlag();
	//}


	/* �L�����u���[�V�����̎��̂ݍs������ */
	if( this->calibFlag ){
		IO::mutex.lock();
		DxLib::printfDx("<< calibration mode >>\n�����L�[�Ńu���b�N�̈ʒu����\n[�L�[��]�L�[�ŕ\���{������\nShift�𓯎��ɉ����ƒ������x0.1�{\n");
		double shiftTime = (Keyboard::getKeyInputFrame(KEY_INPUT_LSHIFT)!=0) ? 0.1 : 1.0;
		// Ctrl�����J�����L�����u���[�V����
		if( Keyboard::getKeyInputFrame(KEY_INPUT_LCONTROL) ){
			if( Keyboard::getKeyInputFrame(KEY_INPUT_LBRACKET) ){
				IO::configData.zPixel += 1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_RBRACKET) ){
				IO::configData.zPixel -=1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_UP) ){
				IO::configData.startPoint.y -=1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_DOWN) ){
				IO::configData.startPoint.y +=1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_LEFT) ){
				IO::configData.startPoint.x -=1;
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_RIGHT) ){
				IO::configData.startPoint.x +=1;
			}
		}
		el{
			if( Keyboard::getKeyInputFrame(KEY_INPUT_LBRACKET) ){
				IO::configData.zoom += (0.1*shiftTime);
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_RBRACKET) ){
				IO::configData.zoom -= (0.1*shiftTime);
			}

			if( IO::configData.zoom < 0.1 ){
				IO::configData.zoom = 0.1;
			}

			if( Keyboard::getKeyInputFrame(KEY_INPUT_UP) ){
				IO::configData.cameraPos.y -= (int)(10*shiftTime);
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_DOWN) ){
				IO::configData.cameraPos.y += (int)(10*shiftTime);
			}
			if( Keyboard::getKeyInputFrame(KEY_INPUT_LEFT) ){
				IO::configData.cameraPos.x -= (int)(10*shiftTime);
			}
			ef( Keyboard::getKeyInputFrame(KEY_INPUT_RIGHT) ){
				IO::configData.cameraPos.x += (int)(10*shiftTime);
			}
		}
		DxLib::printfDx("\n(startX,startY,zoom)=%d,%d,%.3f\n",IO::configData.cameraPos.x,IO::configData.cameraPos.y,IO::configData.zoom);
		IO::mutex.unlock();
	}
}


AnalogState::AnalogState()
{
	this->loadAndPrepareMovie();
}

void AnalogState::mainFlow()
{
	DxLib::ClearDrawScreen();
	DxLib::clsDx();

	//FieldState::get()->generateObjectEvent();
	FieldState::get()->keyInputAction();
	//FieldState::get()->executeAllMovers();
	FieldState::get()->updateBlock();
	if( FieldState::get()->blockDispFlag ){
		FieldState::get()->drawBlock();
	}
	this->updateHighestPoint();
	this->drawMovie();

	DxLib::ScreenFlip();
}

void AnalogState::loadAndPrepareMovie()
{
	this->m_spImage["fire_finger"] = (sp<Image>)(new Image("image/fire/fire_finger.png",27,9,3,125,180,4,2,5,1));
	this->m_spImage["fire_finger"]->loadImage();
}

void AnalogState::updateHighestPoint()
{
	RealField::mutex.lock();

	int sideScr = RealField::getBlockSideScr0();
	int startX = IO::getConst("START_POINT_X");
	Point<int> p1,p2,dp1,dp2;
	p1.y = max(0.0, BLOCK_PIX*5-RealField::heightWave.m_r.begin()->second*((double)BLOCK_PIX/60));
	Point<int> hPoint(-1,99999);
	foreach(it,RealField::smoothedWave.m_r){
		int p2y = max(0.0, BLOCK_PIX*5-it->second*((double)BLOCK_PIX/60) );
		if( p2y != 0 ){
			p2.x = (it->first-startX) * ((double)BLOCK_PIX/sideScr);
			p2.y = p2y;
			dp1 = Camera::getDrawPos(p1);
			dp2 = Camera::getDrawPos(p2);
			if( FieldState::get()->depthWaveDispFlag ){
				DxLib::DrawLine(dp1.x,dp1.y,dp2.x,dp2.y,GetColor(0,0,255),2);
			}
			p1 = p2;
			if( p1.y < hPoint.y ){
				hPoint.x = p1.x;
				hPoint.y = p1.y;
			}
		}
	}
	static int count = 0;
	if( (++count)%10 == 0 ){
		this->highestPoint = hPoint;
	}

	RealField::mutex.unlock();
}

void AnalogState::drawMovie()
{
	auto it = this->m_spImage.find("fire_finger");
	assert(it!=this->m_spImage.end());
	it->second->setCenter( this->highestPoint );
	it->second->libDraw();
}

FieldState3D::FieldState3D()
	:calibModeFlag(false), initDoneFlag(false)
{

}

void FieldState3D::mainFlow()
{
	if( this->initDoneFlag == false ){
		this->init();
	}

	DxLib::ClearDrawScreen();
	DxLib::clsDx();

	this->keyAndMouseInputAction();
	this->showChara(); /// FieldState3d_show �ŃI�[�o�[���C�h

	if( this->calibModeFlag ){
		DxLib::printfDx("<<calibration mode>>");
		static int const SCR_W = IO::getConst("SCR_W");
		static int const SCR_H = IO::getConst("SCR_H");
		IO::mutex.lock();
		Point<int> bias = IO::configData.displayBiasPoint;
		IO::mutex.unlock();
		static int const col = DxLib::GetColor(0,0,255);
		Point<int> center(SCR_W/2+bias.x,SCR_H/2+bias.y);
		MyDxLib::DrawCross(center.x,center.y,col,1,2000);
		DxLib::DrawCircle(center.x,center.y,11,col,0);
		static int const white = DxLib::GetColor(255,255,255);
		DxLib::DrawFormatString(center.x+10,center.y+10,white,"(%d,%d)",center.x,center.y);
	}

	DxLib::ScreenFlip();
}

void FieldState3D::initArduino()
{
	if( FieldState3D::pPort == NULL ){
		static int const ARDUINO_COM_NUM = IO::getConst("ARDUINO_COM_NUM");
		FieldState3D::pPort = (shared_ptr<WinRS>)( new WinRS(ARDUINO_COM_NUM, 9600, ifLine::cr, "8N1", false) );
		passert("Arduino�̃|�[�g�I�[�v�����s",pPort);
	}
}

void FieldState3D::keyAndMouseInputAction()
{
	static int const SCR_W = IO::getConst("SCR_W"); /// 800px
	static int const SCR_H = IO::getConst("SCR_H"); /// ���j�^���� 378mm
	bool shiftFlag = (Keyboard::getKeyInputFrame(KEY_INPUT_LSHIFT)!=0) ? true : false;

	if( /*( Mouse::get_wheelClickTime() == 1 && Mouse::isInScreen() ) ||*/ Keyboard::getIsKeyInputStart(KEY_INPUT_C) ){
		this->calibModeFlag = !this->calibModeFlag;
	}

	// Esc�L�[
	if( Keyboard::getIsKeyInputStart(KEY_INPUT_ESCAPE) ){
		Main::set_endFlag(true);
	}

	if( this->calibModeFlag ){
		// ���N���b�N�J�n
		if( Mouse::get_isLeftClickStart() ){
			this->clickedPoint = Mouse::get_position();
		}
		// ���N���b�N��
		ef( Mouse::get_leftClickFrame() ){
			IO::mutex.lock();
			Point<int> mp = Mouse::get_position();
			IO::configData.displayBiasPoint += (mp-this->clickedPoint) ;
			this->clickedPoint = mp;
			IO::mutex.unlock();
		}
		// S�L�[�F�Z�[�u
		ef( Keyboard::getIsKeyInputStart(KEY_INPUT_S) ){
			IO::saveConfigData();
		}
		//// A�L�[�FAnalogState�ɐ؂�ւ�
		//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_A) ){
		//	GameMain::changeState( AnalogState::get() );
		//}
		//// F�L�[�FFieldState�ɐ؂�ւ�
		//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_F) ){
		//	GameMain::changeState( FieldState::get() );
		//}
		//// D�L�[�FFieldState3D�ɐ؂�ւ�
		//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_D) ){
		//	GameMain::changeState( FieldState3D_show::get() );
		//}
		//// E(Essay:�_��)�L�[�FFieldState3D_essay�ɐ؂�ւ�
		//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_E) ){
		//	GameMain::changeState( FieldState3D_essay::get() );
		//}
		// �}�E�X�z�C�[���F���s��+
		ef( Mouse::get_wheelRot() == 1 ){
			IO::mutex.lock();
			if( shiftFlag ){
				IO::configData.additionalActuatorDown_mm -= 10;
			}
			el{
				IO::configData.additionalShadowNear_mm -= 1;
			}
			IO::mutex.unlock();
		}
		// �}�E�X�z�C�[���F���s��-
		ef( Mouse::get_wheelRot() == -1 ){
			IO::mutex.lock();
			if( shiftFlag ){
				IO::configData.additionalActuatorDown_mm += 10;
			}
			el{
				IO::configData.additionalShadowNear_mm += 1;
			}
			IO::mutex.unlock();
		}
		// W�L�[�F�E�B���h�E�\���ؑ�
		ef( Keyboard::getIsKeyInputStart(KEY_INPUT_W) ){
			static bool windowFlag = TRUE;
			windowFlag = !windowFlag;
			DxLib::ChangeWindowMode((int)windowFlag);
		}
	}
}

FieldState3D_show::FieldState3D_show()
	:showType(JUMP), onBlockCount(0), flyingCount(0)
{

}

void FieldState3D_show::init()
{
	this->pHiyoko = (shared_ptr<Chara>)(new Chara(4,Point<int>(-100,-100)));
	this->initDoneFlag = true;
	this->m_soundHandle["jump"] = DxLib::LoadSoundMem("sound/jump.ogg");
	this->m_soundHandle["land"] = DxLib::LoadSoundMem("sound/land.ogg");
	this->m_soundHandle["pyoko"] = DxLib::LoadSoundMem("sound/pyoko.ogg");
	FieldState3D::initArduino();
}

Point3D_mm FieldState3D_show::getObjectPoint( eShowType _type )
{
	switch( _type ){
	case ORBIT:
		{
			static Point3D_mm ret;
			{
				RealField::mutex.lock();
				pair<int,int> z_y = RealField::getmm_Z_Y(ret.x);
				RealField::mutex.unlock();
				ret.y= z_y.second;
				ret.z = z_y.first;
			}
			// ���W����ʔ͈͊O
			if( ret.z == macro::WRONG_INT && ret.y == macro::WRONG_INT ){
				ret.x = 0;
			}
			el{
				ret.x++;
			}
			return ret;
			break;
		}
	case JUMP:
		{
			// �u���b�N�̏�ɗ����Ă�Ƃ�
			if( this->onBlockCount > 0 ){
				static int const WAIT_FRAME = IO::getConst("WAIT_FRAME");
				pair<int,int> z_y = RealField::getmm_Z_Y(this->charaPos.x);
				double tmpVel = z_y.second - this->charaPos.y; /// ���������낦��
				static int soundCount = 0;
				soundCount++;
				if( abs(tmpVel) > 5 ){
					this->charaPos.y = z_y.second;
				}
				if( tmpVel > 20 ){  /// ������ɑ傫�����������璵��
					if( soundCount > 30 ){
						DxLib::PlaySoundMem(this->m_soundHandle.at("jump"),DX_PLAYTYPE_BACK);
						soundCount = 0;
					}
					this->onBlockCount = 0;
					this->charaVel.y = 6.0;
					this->charaPos.y = z_y.second;
				}
				ef( (++this->onBlockCount) == WAIT_FRAME ){ /// 5�b����������
					if( soundCount > 30 ){
						DxLib::PlaySoundMem(this->m_soundHandle.at("jump"),DX_PLAYTYPE_BACK);
						soundCount = 0;
					}
					this->onBlockCount = 0;
					static double const GRAVITY = IO::getConst("GRAVITY");
					static int const FLYING_FRAME = (int)IO::getConst("FLYING_FRAME");
					this->flyingCount = 1;
					this->nextPoint = RealField::getHighestPoint();
					IO::writeInputData( this->nextPoint );
					// ���̂��Ă���ꏊ�ƁA���̏ꏊ�����ԃx�N�g��
					Point3D_mm vec = nextPoint-this->charaPos;
					/* �΂ߍ��W�n�ł̑��x */
					double Xd = (PM(vec.x)) * SIZE(vec.x,vec.z);
					double Yd = (double)vec.y;
					double Vx = Xd/FLYING_FRAME;
					double Vy = (GRAVITY*FLYING_FRAME)/2 + (Yd)/(FLYING_FRAME);
					/* ���ۂ̍��W�n�ł̑��x */
					if( Xd != 0 ){
						double cos = (double)vec.z/Xd;
						double sin = (double)vec.x/Xd;
						double vx = Vx * sin;
						double vy = Vy;
						double vz = Vx * cos;
						/* ���̂ɑ��x��ݒ� */
						this->charaVel.x = vx;
						this->charaVel.y = vy;
						this->charaVel.z = vz;
					}
					el{
						this->charaVel.x = 0;
						this->charaVel.y = Vy;
						this->charaVel.z = 0;
					}
				}
			}
			// ��s���̂Ƃ�
			el{
				static double const GRAVITY = IO::getConst("GRAVITY");
				// �d�͉���
				this->charaVel.y -= GRAVITY;
				this->flyingCount++;
				pair<int,int> z_y = RealField::getmm_Z_Y(this->charaPos.x);
				// �n�`�Ƃ̔��聨���n
				if( this->charaPos.y <= z_y.second ){
					DxLib::PlaySoundMem(this->m_soundHandle.at("land"),DX_PLAYTYPE_BACK);
					this->flyingCount = 0;
					this->onBlockCount = 1;
					this->charaVel = Point3D_mm(0,0,0);
					this->charaPos.y = z_y.second;
					this->charaPos.z = z_y.first;
				}
			}
			this->charaPos += this->charaVel;
			static int const SHOW_X = IO::getConst("SHOW_X");
			static int const SHOW_Y = IO::getConst("SHOW_Y");
			//static int const SHOW_OUT_mm = IO::getConst("SHOW_OUT_mm");
			// ��ʊO�ɍs���߂��Ă��܂�����
			if( this->charaPos.x < 0 ){
				this->charaPos.x = 0;
				this->charaVel.x = 0;
				this->charaVel.z = 0;
			}
			ef( this->charaPos.x > SHOW_X ){
				this->charaPos.x = SHOW_X-1;
				this->charaVel.x = 0;
				this->charaVel.z = 0;
			}
			if( this->charaPos.y < 0 ){
				this->charaPos.y = 0;
				this->charaVel.y = 0;
				this->charaVel.z = 0;
			}
			ef( this->charaPos.y > SHOW_Y ){
				this->charaPos.y = SHOW_Y-1;
				this->charaVel.y = 0;
				this->charaVel.z = 0;
			}
			return this->charaPos;
			break;
		}
	}
}

void FieldState3D_show::showChara()
{
	IO::mutex.lock();
	static int const SCR_W = IO::getConst("SCR_W"); /// 800px
	static int const SCR_H = IO::getConst("SCR_H"); /// 600px
	static int const SHOW_X = IO::getConst("SHOW_X"); /// ���j�^���� 378mm
	static int const DESK_DEPTH = IO::getConst("DESK_DEPTH");
	IO::mutex.unlock();

	/// v_pa_drawOrbit >>>> x(px), z(px), deskDepth-y(mm)
	static int xmm, ymm, zmm;
	static Point3D_mm tmp;
	tmp = this->getObjectPoint(this->showType);
	xmm = tmp.x;
	ymm = tmp.y;
	zmm = tmp.z;

	/*------------�����W�n�̈Ⴂ�ɒ��ӁI�I��--------------

	�@�@�@�@�@DCRA��
	�@�@�Q�Q�Q�Q�Q�Q�Q�Q�Q
	�@�^�@�@�@�@�@�@�@�@�^
	�^�@�@�@�@�@�@�@�@�^
	�P�P�P�P�P�P�P�P�P
	�@�@�l��

	Sekai: �P�ʂ͊�{mm�B
	  y
	  |�Q x
	 �^
	z

	DDT:�P�ʂ�px�B�u���b�N����B100px=60mm�B
	  z
	  |�Q x
	 �^
	y

	-----------------------*/

	 
	/// �����܂łŎ���Ԃɂ�����x,y,z(�P�ʂ͑S��mm)���������
	/// x,z�͕`��Ɏg���By�̓A�N�`���G�[�^�𓮂����̂Ɏg��

	/* �L�����N�^�[��`�悷��(x,z�g�p) */
	Point<int> drawPoint;  /// drawPoint��y�̓V�X�e�����W�n��z�Ȃ̂Œ���
	drawPoint.x =     (int)( xmm * (double)SCR_W/SHOW_X);
	drawPoint.y = SCR_H-(int)( ymm * (double)SCR_W/SHOW_X); /// �L�����̒��S�_�Ȃ̂ŁA�O�Ղ���50������ɏグ��

	IO::mutex.lock();
	Point<int> dbp = IO::configData.displayBiasPoint;
	IO::mutex.unlock();

	drawPoint += dbp;
	this->pHiyoko->mainFlow( drawPoint );

	/* �A�N�`���G�[�^�𓮂���(y�g�p) */
	/// z(mm)�̒l�����̂܂܃A�N�`���G�[�^�𓮂��������B
	/// �������A�A�N�`���G�[�^���ő�(300mm)�܂ŐL�т����Ă����Ԃ�0�Ƃ��āA���̏�Ԃ���z(mm)����������B
	/// Arduino::actuate(zmm);  �݂����ȃR�[�h�������ɂP�s�����B
	static int const SHOW_Z = IO::getConst("SHOW_Z");
	IO::mutex.lock();
	int addz = IO::configData.additionalActuatorDown_mm;
	IO::mutex.unlock();
	int actExtend_mm = SHOW_Z-(zmm+addz);
	FieldState3D::pPort->putc1( max(0,actExtend_mm) );

	/* �v���W�F�N�^�ɏ��𑗂� */
	static int const BLOCK_SIDE = IO::getConst("BLOCK_SIDE");
	IO::mutex.lock();
	int addNear = IO::configData.additionalShadowNear_mm;
	Point<int> bias = IO::configData.displayBiasPoint;
	IO::mutex.unlock();
	int bias_x_mm = (bias.x)/((double)SCR_W/SHOW_X);
	int bias_y_mm = (-bias.y)/((double)SCR_W/SHOW_X);
	static int a[3] = {50,50,50};
	a[0] = (xmm+bias_x_mm) * (100.0/BLOCK_SIDE); /// DDT::x(100px) == SEKAI::x
	a[1] = (zmm+addNear)   * (100.0/BLOCK_SIDE); /// DDT::y(100px) == SEKAI::z
	/// ��+30���Ă���̂́A�e�̈ʒu���u���b�N��ɍ��킹�邽��
	a[2] = (ymm+bias_y_mm) * (100.0/BLOCK_SIDE); /// DDT::z(100px) == SEKAI::y
	Main::pSysProjCtrlWinClass->SendMessageToProjecter( 12 , &a );

	/* �g�`��\������(�f�o�b�O�p) */
	RealField::mutex.lock();
	if( this->calibModeFlag ){
		static int const SHOW_W = IO::getConst("SHOW_X");
		static Point<int> dp;
		static Point<int> dp0;
		for( int x=0; x<SHOW_X; x++ ){
			pair<int,int> z_y = RealField::getmm_Z_Y(x);
			int y = z_y.second;
			dp0 = dp;
			dp.x =     (int)( x * (double)SCR_W/SHOW_X);
			dp.y = SCR_H-(int)( y * (double)SCR_W/SHOW_X); /// �L�����̒��S�_�Ȃ̂ŁA�O�Ղ���50������ɏグ��
			static int const col = DxLib::GetColor(0,255,0);
			if( x!=0 && dp0.x != macro::WRONG_INT && dp0.y != macro::WRONG_INT && dp.x != macro::WRONG_INT && dp.y != macro::WRONG_INT ){
				static Point<int> _dp, _dp0;
				_dp = dp+dbp;
				_dp0 = dp0+dbp;
				DxLib::DrawLine(_dp0.x,_dp0.y,_dp.x,_dp.y,col);
			}
		}
	}
	RealField::mutex.unlock();
}





FieldState3D_essay::FieldState3D_essay()
	:onBlockCount(1), flyingCount(0), showObjectType(HIYOKO), fireAngle_rad(0.0)
{

}


// @ovr
#define TYPE_NUM 2
void FieldState3D_essay::showChara()
{
	if( this->calibModeFlag ){
		if( Keyboard::getIsKeyInputStart(KEY_INPUT_0) ){
			static int i = 0;
			static eShowObjectType a_type[TYPE_NUM] = { HIYOKO, FIRE };
			i = (i+1)%TYPE_NUM;
			this->showObjectType = a_type[i];
		}
	}

	IO::mutex.lock();
	static int const SCR_W = IO::getConst("SCR_W"); /// 800px
	static int const SCR_H = IO::getConst("SCR_H"); /// 600px
	static int const SHOW_X = IO::getConst("SHOW_X"); /// ���j�^���� 378mm
	static int const DESK_DEPTH = IO::getConst("DESK_DEPTH");
	IO::mutex.unlock();

	/// v_pa_drawOrbit >>>> x(px), z(px), deskDepth-y(mm)
	int xmm=0,ymm=0,zmm=0;
	Point3D_mm tmpp = this->getObjectPoint( this->showObjectType );
	xmm = (int)tmpp.x;
	ymm = (int)tmpp.y;
	zmm = (int)tmpp.z;

	/*------------�����W�n�̈Ⴂ�ɒ��ӁI�I��--------------

	�@�@�@�@�@DCRA��
	�@�@�Q�Q�Q�Q�Q�Q�Q�Q�Q
	�@�^�@�@�@�@�@�@�@�@�^
	�^�@�@�@�@�@�@�@�@�^
	�P�P�P�P�P�P�P�P�P
	�@�@�l��

	Sekai: �P�ʂ͊�{mm�B
	  y
	  |�Q x
	 �^
	z

	DDT:�P�ʂ�px�B�u���b�N����B100px=60mm�B
	  z
	  |�Q x
	 �^
	y

	-----------------------*/

	 
	/// �����܂łŎ���Ԃɂ�����x,y,z(�P�ʂ͑S��mm)���������
	/// x,z�͕`��Ɏg���By�̓A�N�`���G�[�^�𓮂����̂Ɏg��

	/* �L�����N�^�[��`�悷��(x,z�g�p) */
	Point<int> drawPoint;  /// drawPoint��y�̓V�X�e�����W�n��z�Ȃ̂Œ���
	drawPoint.x =     (int)( xmm * (double)SCR_W/SHOW_X);
	drawPoint.y = SCR_H-(int)( ymm * (double)SCR_W/SHOW_X); /// �L�����̒��S�_�Ȃ̂ŁA�O�Ղ���50������ɏグ��

	IO::mutex.lock();
	Point<int> dbp = IO::configData.displayBiasPoint;
	IO::mutex.unlock();

	drawPoint += dbp;
	switch( this->showObjectType ){
	case HIYOKO:
		{
			this->pHiyoko->mainFlow( drawPoint );
			break;
		}
	case FIRE:
		{
			this->pFireImage->setCenter( drawPoint );
			this->pFireImage->simpleDraw();
			break;
		}
	}


	/* �A�N�`���G�[�^�𓮂���(y�g�p) */
	/// z(mm)�̒l�����̂܂܃A�N�`���G�[�^�𓮂��������B
	/// �������A�A�N�`���G�[�^���ő�(300mm)�܂ŐL�т����Ă����Ԃ�0�Ƃ��āA���̏�Ԃ���z(mm)����������B
	/// Arduino::actuate(zmm);  �݂����ȃR�[�h�������ɂP�s�����B
	static int const SHOW_Z = IO::getConst("SHOW_Z");
	IO::mutex.lock();
	int addz = IO::configData.additionalActuatorDown_mm;
	IO::mutex.unlock();
	FieldState3D::pPort->putc1(SHOW_Z-(zmm+addz));
	cout << SHOW_Z-(zmm+addz) << endl;
	
	/* �v���W�F�N�^�ɏ��𑗂� */
	static int const BLOCK_SIDE = IO::getConst("BLOCK_SIDE");
	int addNear = IO::configData.additionalShadowNear_mm;
	static int a[3] = {50,50,50};
	a[0] = xmm           * (100.0/BLOCK_SIDE); /// DDT::x(100px) == SEKAI::x
	a[1] = (zmm+addNear) * (100.0/BLOCK_SIDE); /// DDT::y(100px) == SEKAI::z
	/// ��+30���Ă���̂́A�e�̈ʒu���u���b�N��ɍ��킹�邽��
	a[2] = ymm           * (100.0/BLOCK_SIDE); /// DDT::z(100px) == SEKAI::y
	Main::pSysProjCtrlWinClass->SendMessageToProjecter( 12 , &a );
}

Point3D_mm FieldState3D_essay::getObjectPoint( eShowObjectType _type )
{
	Point3D_mm ret;
	switch( _type ){
	case HIYOKO:
		{

			// �u���b�N�̏�ɗ����Ă�Ƃ�
			if( this->onBlockCount > 0 ){
				static int const WAIT_FRAME = IO::getConst("WAIT_FRAME");
				if( (++this->onBlockCount) == WAIT_FRAME ){ /// 5�b����������
					this->onBlockCount = 0;
				}
			}
			// ��s���̂Ƃ�
			el{
				static double const GRAVITY = IO::getConst("GRAVITY");
				static int const FLYING_FRAME = (int)IO::getConst("FLYING_FRAME");
				// ��юn��
				if( this->flyingCount == 0 ){
					this->flyingCount = 1;
					auto it_next = this->it_point;
					it_next++;
					// ���̃u���b�N���Ȃ�
					if( it_next == this->v_Points.end() ){
						it_next = this->v_Points.begin(); /// �ŏ��̓_�ɐݒ�
					}
					// ������Ă���u���b�N�ƁA���̃u���b�N�����ԃx�N�g��
					Point3D_mm vec = (*it_next) - (*(this->it_point));
					/* �΂ߍ��W�n�ł̑��x */
					double Xd = (PM(vec.x)) * SIZE(vec.x,vec.z);
					double Yd = (double)vec.y;
					double Vx = Xd/FLYING_FRAME;
					double Vy = (GRAVITY*FLYING_FRAME)/2 + (Yd)/(FLYING_FRAME);
					/* ���ۂ̍��W�n�ł̑��x */
					double cos = (double)vec.z/Xd;
					double sin = (double)vec.x/Xd;
					double vx = Vx * sin;
					double vy = Vy;
					double vz = Vx * cos;
					/* ���̂ɑ��x��ݒ� */
					this->charaVel.x = vx;
					this->charaVel.y = vy;
					this->charaVel.z = vz;
				}
				// ��яI���
				ef( this->flyingCount > FLYING_FRAME ){
					this->flyingCount = 0;
					this->onBlockCount = 1;
					this->charaVel = Point3D_mm(0,0,0);
					if( (++(this->it_point)) == this->v_Points.end() ){
						this->it_point = this->v_Points.begin();
					}
				}
				// ��s��
				el{
					// �d�͉���
					this->charaVel.y -= GRAVITY;
					this->flyingCount++;
				}
			}
			this->charaPos += this->charaVel;
			ret = this->charaPos;
			break;
		}
	case FIRE:
		{
			static int const R_XZ = (int)IO::getConst("R_XZ");
			static int const R_Y = (int)IO::getConst("R_Y");
			static int const CYCLE_FRAME = (int)IO::getConst("CYCLE_FRAME");
			static int const FIRE_CENTER_X = (int)IO::getConst("FIRE_CENTER_X");
			static int const FIRE_CENTER_Y = (int)IO::getConst("FIRE_CENTER_Y");
			static int const FIRE_CENTER_Z = (int)IO::getConst("FIRE_CENTER_Z");
			static double const ANGLE_SPEED = macro::PI/CYCLE_FRAME;

			this->fireAngle_rad += ANGLE_SPEED;
			ret.x = FIRE_CENTER_X + R_XZ*sin(this->fireAngle_rad);
			ret.y = FIRE_CENTER_Y + R_Y*cos(this->fireAngle_rad);
			ret.z = FIRE_CENTER_Z + R_XZ*cos(this->fireAngle_rad);

			break;
		}
	}
	return ret;
}

#define getcell(i,cells) cells.at(i)
#define getcelli(i,cells) utils::s2i(cells.at(i))
#define getcelld(i,cells) utils::s2d(cells.at(i))

// @ovr
void FieldState3D_essay::init()
{	
	this->pHiyoko = (shared_ptr<Chara>)(new Chara(4,Point<int>(-100,-100)));
	this->initDoneFlag = true;
	FieldState3D::initArduino();

	ifstream ifs( "data/auto_move_orbit.csv" );
	//1�s���̃o�b�t�@
	string line;
	//�ŏ��̂P�s�͎̂Ă�
	getline( ifs, line );

	int lineCount = -1;
	while( ifs && getline( ifs, line ) ){
		lineCount++;
		vector<string> cells;
		utils::cutLine( line, cells );
		int i=0;
		if( cells.size() <= 1 ){ 
			continue;
		}
		int x_ = getcelli(i++,cells);
		int y_ = getcelli(i++,cells);
		int z_ = getcelli(i++,cells);
		this->v_Points.push_back( Point3D_mm(x_,y_,z_) );
	}

	this->it_point = this->v_Points.begin();
	this->charaPos = this->v_Points.front();

	this->pFireImage = (sp<Image>)(new Image("image/fire/fire_finger.png",27,9,3,125,180,4,2));
	this->pFireImage->loadImage();
}
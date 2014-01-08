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
shared_ptr<class WinRS>  FieldState3D::pPort = NULL; // Arduino用のポート


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
	passert("GameStateのpStateがNULLです",GameMain::pState!=NULL);
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

// 特定の場所の段数を取得
int FieldState::getDan( int _line, int _x ) const
{
	return RealField::getDan( _line, _x );
}

// RealFieldの段数にもとづいてブロック一覧を更新
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

// Moverとブロックとの当たり判定を全探索で行って速度を補正する
bool FieldState::hitBlockAndChangeVelEvent( Mover* _pMover )
{
	bool ret = false;

	/* 床についての当たり判定 */
	static Rect<int> myRect1,myRect2;
	myRect1 = _pMover->getBodyRectBeforeVelAdded();
	myRect2 = _pMover->getBodyRectAfterVelAdded();

	//cout << _pMover->pos.x << "," << _pMover->pos.y << endl;

	/// いまいるラインについて
	/* 全てのブロックについての当たり判定 */
	foreach(it,this->v_v_Block[this->line]){
		foreach(it2,*it){
			int locTypeAfter = myRect2.getLocationType(*it2);
			// 当たり判定
			if((locTypeAfter&LOC_UDHIT)&&(locTypeAfter&LOC_LRHIT)){
				ret = true;
				int locTypeBefore = myRect1.getLocationType(*it2);
				/* 着地のケース */
				if( locTypeBefore&LOC_UP ){
					//_pMover->setStandingBlockAddr(&*it2);
					_pMover->vel.y -= (myRect2.p2.y-it2->p1.y);
				}
				/* 立っているケース */
				ef( locTypeBefore&LOC_UP_FIT ){
					_pMover->vel.y = 0;
				}
				/* 下からヒットのケース */
				ef( (locTypeBefore&LOC_DOWN) || (locTypeBefore&LOC_DOWN_FIT) ){
					_pMover->vel.y += (it2->p2.y-myRect2.p1.y);
				}
				/* 左からヒットのケース */
				ef( (locTypeBefore&LOC_LEFT) || (locTypeBefore&LOC_LEFT_FIT) ){
					_pMover->vel.x -= (myRect2.p2.x-it2->p1.x);
				}
				/* 右からヒットのケース */
				ef( (locTypeBefore&LOC_RIGHT) || (locTypeBefore&LOC_RIGHT_FIT) ){
					_pMover->vel.x += (it2->p2.x-myRect2.p1.x);
				}
				el{
					//passert("定義されていないあたり判定です",0);
				}
			}
		}
	}

	return ret;
}

// CharaとObjectとの当たり判定を全探索で行う
bool FieldState::hitObjectEvent( class Chara * _pChara )
{
	bool ret = false;

	static Rect<int> myRect2;
	myRect2 = _pChara->getBodyRectAfterVelAdded();

	/* 全てのObjectについての当たり判定 */
	for(auto it=this->m_spObject.begin(); it!=this->m_spObject.end(); ){
		int locTypeAfter = myRect2.getLocationType(it->second->getBodyRectAfterVelAdded());
		// 当たり判定
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

// ブロック描画
void FieldState::drawBlock()
{
	foreach(it,this->v_v_Block[this->line]){
		foreach(it2,*it){
			it2->libDraw();
			
		}
	}
}

// Moverをジェネレート
void FieldState::generateChara( int _index, Point<int> _pos )
{
	this->m_spChara[_index] = (sp<Chara>)(new Chara(_index,_pos));
}

// Objectをジェネレート
void FieldState::generateObject( int _index, Point<int> _pos )
{
	sp<Object> p = (sp<Object>)(new Object(_index,_pos));
	this->m_spObject[ p.get() ] = p;
}

// 全てのMoverを実行
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

// 乱数を用いてオブジェクトをジェネレート
void FieldState::generateObjectEvent()
{
	switch( rand() % 53 ){
	case 0: // コイン
		{
			static int const SCR_W = IO::getConst("SCR_W");
			this->generateObject(4,Point<int>(rand()%SCR_W,0));
			break;
		}
	}
}

void FieldState::keyInputAction()
{
	// Wキー：ウィンドウモード切り替え
	if( Keyboard::getIsKeyInputStart(KEY_INPUT_W) ){
		static bool windowFlag = TRUE;
		windowFlag = !windowFlag;
		DxLib::ChangeWindowMode(windowFlag);
	}
	// Sキー：コンフィグデータセーブ
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_S) ){
		IO::saveConfigData();
	}
	// Cキー：キャリブレーションモード変更
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_C) ){
		this->calibFlag = !this->calibFlag;
	}
	// Bキー：ブロック表示切り替え
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_B) ){
		this->blockDispFlag = !this->blockDispFlag;
	}
	// Lキー：デプス波形表示切替
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_L) ){
		this->depthWaveDispFlag = ! this->depthWaveDispFlag;
	}
	// 1キー：Mover 1をジェネレート
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_1) ){
		this->generateChara(1,Point<int>(0,0));
	}
	// 2キー：Mover 2をジェネレート
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_2) ){
		this->generateChara(2,Point<int>(50,0));
	}
	// 3キー：Mover 3をジェネレート
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_3) ){
		this->generateChara(3,Point<int>(150,0));
	}
	// Aキー：AnalogStateに切り替え
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_A) ){
		GameMain::changeState( AnalogState::get() );
	}
	// Fキー：FieldStateに切り替え
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_F) ){
		GameMain::changeState( FieldState::get() );
	}
	// Dキー：FieldState3Dに切り替え
	ef( Keyboard::getIsKeyInputStart(KEY_INPUT_D) ){
		GameMain::changeState( FieldState3D_show::get() );
	}
	/// 故障中...
	//// Iキー：opencvのImageをdispするかどうかの切り替え
	//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_I) ){
	//	RealField::inv_dispCvImageFlag();
	//}


	/* キャリブレーションの時のみ行うこと */
	if( this->calibFlag ){
		IO::mutex.lock();
		DxLib::printfDx("<< calibration mode >>\n方向キーでブロックの位置調整\n[キーと]キーで表示倍率調整\nShiftを同時に押すと調整速度0.1倍\n");
		double shiftTime = (Keyboard::getKeyInputFrame(KEY_INPUT_LSHIFT)!=0) ? 0.1 : 1.0;
		// Ctrl中→カメラキャリブレーション
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
	this->showChara(); /// FieldState3d_show でオーバーライド

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
		passert("Arduinoのポートオープン失敗",pPort);
	}
}

void FieldState3D::keyAndMouseInputAction()
{
	static int const SCR_W = IO::getConst("SCR_W"); /// 800px
	static int const SCR_H = IO::getConst("SCR_H"); /// モニタ横幅 378mm
	bool shiftFlag = (Keyboard::getKeyInputFrame(KEY_INPUT_LSHIFT)!=0) ? true : false;

	if( /*( Mouse::get_wheelClickTime() == 1 && Mouse::isInScreen() ) ||*/ Keyboard::getIsKeyInputStart(KEY_INPUT_C) ){
		this->calibModeFlag = !this->calibModeFlag;
	}

	// Escキー
	if( Keyboard::getIsKeyInputStart(KEY_INPUT_ESCAPE) ){
		Main::set_endFlag(true);
	}

	if( this->calibModeFlag ){
		// 左クリック開始
		if( Mouse::get_isLeftClickStart() ){
			this->clickedPoint = Mouse::get_position();
		}
		// 左クリック中
		ef( Mouse::get_leftClickFrame() ){
			IO::mutex.lock();
			Point<int> mp = Mouse::get_position();
			IO::configData.displayBiasPoint += (mp-this->clickedPoint) ;
			this->clickedPoint = mp;
			IO::mutex.unlock();
		}
		// Sキー：セーブ
		ef( Keyboard::getIsKeyInputStart(KEY_INPUT_S) ){
			IO::saveConfigData();
		}
		//// Aキー：AnalogStateに切り替え
		//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_A) ){
		//	GameMain::changeState( AnalogState::get() );
		//}
		//// Fキー：FieldStateに切り替え
		//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_F) ){
		//	GameMain::changeState( FieldState::get() );
		//}
		//// Dキー：FieldState3Dに切り替え
		//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_D) ){
		//	GameMain::changeState( FieldState3D_show::get() );
		//}
		//// E(Essay:論文)キー：FieldState3D_essayに切り替え
		//ef( Keyboard::getIsKeyInputStart(KEY_INPUT_E) ){
		//	GameMain::changeState( FieldState3D_essay::get() );
		//}
		// マウスホイール：奥行き+
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
		// マウスホイール：奥行き-
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
		// Wキー：ウィンドウ表示切替
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
			// 座標が画面範囲外
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
			// ブロックの上に立ってるとき
			if( this->onBlockCount > 0 ){
				static int const WAIT_FRAME = IO::getConst("WAIT_FRAME");
				pair<int,int> z_y = RealField::getmm_Z_Y(this->charaPos.x);
				double tmpVel = z_y.second - this->charaPos.y; /// 高さをそろえる
				static int soundCount = 0;
				soundCount++;
				if( abs(tmpVel) > 5 ){
					this->charaPos.y = z_y.second;
				}
				if( tmpVel > 20 ){  /// 上向きに大きく加速したら跳ぶ
					if( soundCount > 30 ){
						DxLib::PlaySoundMem(this->m_soundHandle.at("jump"),DX_PLAYTYPE_BACK);
						soundCount = 0;
					}
					this->onBlockCount = 0;
					this->charaVel.y = 6.0;
					this->charaPos.y = z_y.second;
				}
				ef( (++this->onBlockCount) == WAIT_FRAME ){ /// 5秒たったら飛ぶ
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
					// 今のっている場所と、次の場所を結ぶベクトル
					Point3D_mm vec = nextPoint-this->charaPos;
					/* 斜め座標系での速度 */
					double Xd = (PM(vec.x)) * SIZE(vec.x,vec.z);
					double Yd = (double)vec.y;
					double Vx = Xd/FLYING_FRAME;
					double Vy = (GRAVITY*FLYING_FRAME)/2 + (Yd)/(FLYING_FRAME);
					/* 実際の座標系での速度 */
					if( Xd != 0 ){
						double cos = (double)vec.z/Xd;
						double sin = (double)vec.x/Xd;
						double vx = Vx * sin;
						double vy = Vy;
						double vz = Vx * cos;
						/* 物体に速度を設定 */
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
			// 飛行中のとき
			el{
				static double const GRAVITY = IO::getConst("GRAVITY");
				// 重力加速
				this->charaVel.y -= GRAVITY;
				this->flyingCount++;
				pair<int,int> z_y = RealField::getmm_Z_Y(this->charaPos.x);
				// 地形との判定→着地
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
			// 画面外に行き過ぎてしまったら
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
	static int const SHOW_X = IO::getConst("SHOW_X"); /// モニタ横幅 378mm
	static int const DESK_DEPTH = IO::getConst("DESK_DEPTH");
	IO::mutex.unlock();

	/// v_pa_drawOrbit >>>> x(px), z(px), deskDepth-y(mm)
	static int xmm, ymm, zmm;
	static Point3D_mm tmp;
	tmp = this->getObjectPoint(this->showType);
	xmm = tmp.x;
	ymm = tmp.y;
	zmm = tmp.z;

	/*------------※座標系の違いに注意！！※--------------

	　　　　　DCRA側
	　　＿＿＿＿＿＿＿＿＿
	　／　　　　　　　　／
	／　　　　　　　　／
	￣￣￣￣￣￣￣￣￣
	　　人側

	Sekai: 単位は基本mm。
	  y
	  |＿ x
	 ／
	z

	DDT:単位はpx。ブロックが基準。100px=60mm。
	  z
	  |＿ x
	 ／
	y

	-----------------------*/

	 
	/// ここまでで実空間におけるx,y,z(単位は全部mm)がそろった
	/// x,zは描画に使う。yはアクチュエータを動かすのに使う

	/* キャラクターを描画する(x,z使用) */
	Point<int> drawPoint;  /// drawPointのyはシステム座標系のzなので注意
	drawPoint.x =     (int)( xmm * (double)SCR_W/SHOW_X);
	drawPoint.y = SCR_H-(int)( ymm * (double)SCR_W/SHOW_X); /// キャラの中心点なので、軌跡よりも50だけ上に上げる

	IO::mutex.lock();
	Point<int> dbp = IO::configData.displayBiasPoint;
	IO::mutex.unlock();

	drawPoint += dbp;
	this->pHiyoko->mainFlow( drawPoint );

	/* アクチュエータを動かす(y使用) */
	/// z(mm)の値がそのままアクチュエータを動かす長さ。
	/// ただし、アクチュエータが最大(300mm)まで伸びきっている状態を0として、その状態からz(mm)だけ下げる。
	/// Arduino::actuate(zmm);  みたいなコードをここに１行書く。
	static int const SHOW_Z = IO::getConst("SHOW_Z");
	IO::mutex.lock();
	int addz = IO::configData.additionalActuatorDown_mm;
	IO::mutex.unlock();
	int actExtend_mm = SHOW_Z-(zmm+addz);
	FieldState3D::pPort->putc1( max(0,actExtend_mm) );

	/* プロジェクタに情報を送る */
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
	/// ↑+30しているのは、影の位置をブロック上に合わせるため
	a[2] = (ymm+bias_y_mm) * (100.0/BLOCK_SIDE); /// DDT::z(100px) == SEKAI::y
	Main::pSysProjCtrlWinClass->SendMessageToProjecter( 12 , &a );

	/* 波形を表示する(デバッグ用) */
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
			dp.y = SCR_H-(int)( y * (double)SCR_W/SHOW_X); /// キャラの中心点なので、軌跡よりも50だけ上に上げる
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
	static int const SHOW_X = IO::getConst("SHOW_X"); /// モニタ横幅 378mm
	static int const DESK_DEPTH = IO::getConst("DESK_DEPTH");
	IO::mutex.unlock();

	/// v_pa_drawOrbit >>>> x(px), z(px), deskDepth-y(mm)
	int xmm=0,ymm=0,zmm=0;
	Point3D_mm tmpp = this->getObjectPoint( this->showObjectType );
	xmm = (int)tmpp.x;
	ymm = (int)tmpp.y;
	zmm = (int)tmpp.z;

	/*------------※座標系の違いに注意！！※--------------

	　　　　　DCRA側
	　　＿＿＿＿＿＿＿＿＿
	　／　　　　　　　　／
	／　　　　　　　　／
	￣￣￣￣￣￣￣￣￣
	　　人側

	Sekai: 単位は基本mm。
	  y
	  |＿ x
	 ／
	z

	DDT:単位はpx。ブロックが基準。100px=60mm。
	  z
	  |＿ x
	 ／
	y

	-----------------------*/

	 
	/// ここまでで実空間におけるx,y,z(単位は全部mm)がそろった
	/// x,zは描画に使う。yはアクチュエータを動かすのに使う

	/* キャラクターを描画する(x,z使用) */
	Point<int> drawPoint;  /// drawPointのyはシステム座標系のzなので注意
	drawPoint.x =     (int)( xmm * (double)SCR_W/SHOW_X);
	drawPoint.y = SCR_H-(int)( ymm * (double)SCR_W/SHOW_X); /// キャラの中心点なので、軌跡よりも50だけ上に上げる

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


	/* アクチュエータを動かす(y使用) */
	/// z(mm)の値がそのままアクチュエータを動かす長さ。
	/// ただし、アクチュエータが最大(300mm)まで伸びきっている状態を0として、その状態からz(mm)だけ下げる。
	/// Arduino::actuate(zmm);  みたいなコードをここに１行書く。
	static int const SHOW_Z = IO::getConst("SHOW_Z");
	IO::mutex.lock();
	int addz = IO::configData.additionalActuatorDown_mm;
	IO::mutex.unlock();
	FieldState3D::pPort->putc1(SHOW_Z-(zmm+addz));
	cout << SHOW_Z-(zmm+addz) << endl;
	
	/* プロジェクタに情報を送る */
	static int const BLOCK_SIDE = IO::getConst("BLOCK_SIDE");
	int addNear = IO::configData.additionalShadowNear_mm;
	static int a[3] = {50,50,50};
	a[0] = xmm           * (100.0/BLOCK_SIDE); /// DDT::x(100px) == SEKAI::x
	a[1] = (zmm+addNear) * (100.0/BLOCK_SIDE); /// DDT::y(100px) == SEKAI::z
	/// ↑+30しているのは、影の位置をブロック上に合わせるため
	a[2] = ymm           * (100.0/BLOCK_SIDE); /// DDT::z(100px) == SEKAI::y
	Main::pSysProjCtrlWinClass->SendMessageToProjecter( 12 , &a );
}

Point3D_mm FieldState3D_essay::getObjectPoint( eShowObjectType _type )
{
	Point3D_mm ret;
	switch( _type ){
	case HIYOKO:
		{

			// ブロックの上に立ってるとき
			if( this->onBlockCount > 0 ){
				static int const WAIT_FRAME = IO::getConst("WAIT_FRAME");
				if( (++this->onBlockCount) == WAIT_FRAME ){ /// 5秒たったら飛ぶ
					this->onBlockCount = 0;
				}
			}
			// 飛行中のとき
			el{
				static double const GRAVITY = IO::getConst("GRAVITY");
				static int const FLYING_FRAME = (int)IO::getConst("FLYING_FRAME");
				// 飛び始め
				if( this->flyingCount == 0 ){
					this->flyingCount = 1;
					auto it_next = this->it_point;
					it_next++;
					// 次のブロックがない
					if( it_next == this->v_Points.end() ){
						it_next = this->v_Points.begin(); /// 最初の点に設定
					}
					// 今乗っているブロックと、次のブロックを結ぶベクトル
					Point3D_mm vec = (*it_next) - (*(this->it_point));
					/* 斜め座標系での速度 */
					double Xd = (PM(vec.x)) * SIZE(vec.x,vec.z);
					double Yd = (double)vec.y;
					double Vx = Xd/FLYING_FRAME;
					double Vy = (GRAVITY*FLYING_FRAME)/2 + (Yd)/(FLYING_FRAME);
					/* 実際の座標系での速度 */
					double cos = (double)vec.z/Xd;
					double sin = (double)vec.x/Xd;
					double vx = Vx * sin;
					double vy = Vy;
					double vz = Vx * cos;
					/* 物体に速度を設定 */
					this->charaVel.x = vx;
					this->charaVel.y = vy;
					this->charaVel.z = vz;
				}
				// 飛び終わり
				ef( this->flyingCount > FLYING_FRAME ){
					this->flyingCount = 0;
					this->onBlockCount = 1;
					this->charaVel = Point3D_mm(0,0,0);
					if( (++(this->it_point)) == this->v_Points.end() ){
						this->it_point = this->v_Points.begin();
					}
				}
				// 飛行中
				el{
					// 重力加速
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
	//1行分のバッファ
	string line;
	//最初の１行は捨てる
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
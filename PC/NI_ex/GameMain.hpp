#pragma once
#include "includes.hpp"
#include "Rect.hpp"
#include "Singleton.hpp"
#include "Point3D_mm.h"

enum eGameState{ GS_FIELD, GS_ANALOG, GS_FIELD3D };

class GameMain
{
public:
	// ゲームのメインループ
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

// 開始画面
class TitleState : public GameState, public Singleton<TitleState>
{
	friend class Singleton<TitleState>;
interface__:
	void mainFlow();
};

// フィールド
class FieldState : public GameState, public Singleton<FieldState>
{
	friend class Singleton<FieldState>;
	friend class AnalogState;
	friend class FieldState3D;
interface__:
	FieldState() : calibFlag(false), blockDispFlag(true), depthWaveDispFlag(true), line(0) {}
	// @ovr メインフロー
	void mainFlow();
	// @ovr
	eGameState getGameState() const { return GS_FIELD; }

	// 特定の場所の段数を取得
	int getDan( int _line, int _x ) const;
	// RealFieldの段数にもとづいてブロック一覧を更新
	void updateBlock();
	// Moverとブロックとの当たり判定を全探索で行って速度を補正する
	bool hitBlockAndChangeVelEvent( class Mover* _pMover );
	// CharaとObjectとの当たり判定を全探索で行う
	bool hitObjectEvent( class Chara * _pChara );

inner__:
	// キー操作
	void keyInputAction();
	// ブロック描画
	void drawBlock();
	// Charaをジェネレート
	void generateChara( int _num, Point<int> _pos );
	// Objectをジェネレート
	void generateObject( int _index, Point<int> _pos );
	// 全てのMoverを実行
	void executeAllMovers();
	// 乱数を用いてオブジェクトをジェネレート
	void generateObjectEvent();

inner__:
	bool calibFlag; // キャリブレーションモードかどうか
	bool blockDispFlag; // ブロックを表示するかどうか
	bool depthWaveDispFlag; // 深さ波形を表示するかどうか
	map<int,sp<class Chara> > m_spChara; // 物体一覧
	map< class Object *, sp<class Object> > m_spObject; // オブジェクト一覧
	vector<vector<class Block> > v_v_Block[3]; // ブロック一覧
	int line;    // 今いるライン
};

// アナログ認識モード
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

// 立体認識モード
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
	bool initDoneFlag;   // 初期化できているかどうか
	bool calibModeFlag; // キャリブレーションモードかどうかのフラグ
	static shared_ptr<class WinRS>  pPort; // Arduino用のポート
	shared_ptr<class Chara> pHiyoko; // ひよこ
	Point<int> clickedPoint;         // 左クリックされた場所
	map< string, int > m_soundHandle;
};

// 立体認識モード（展示用）
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

// 立体認識モード（論文のための撮影用）
class FieldState3D_essay : public FieldState3D, public Singleton<FieldState3D_essay>
{
	friend class Singleton<FieldState3D_essay>;
interface__:
	FieldState3D_essay();
inner__:
	// @ovr
	void showChara();
	void init();
	// HiyokoかFireかに応じて変える
	enum eShowObjectType { HIYOKO, FIRE } showObjectType;
	Point3D_mm getObjectPoint( eShowObjectType );
inner__:
	vector< Point3D_mm > v_Points;
	vector< Point3D_mm >::iterator it_point;
	Point3D_mm charaPos; // キャラクターの現在位置
	Point3D_mm charaVel; // キャラクターの現在の速度（mm/frame）
	int onBlockCount;   // ブロックの上にいる秒数のカウント
	int flyingCount;    // 飛んでいる秒数のカウント
	// 炎関連
	shared_ptr< class Image > pFireImage;
	Point3D_mm firePos; // 炎の位置
	double fireAngle_rad;
};
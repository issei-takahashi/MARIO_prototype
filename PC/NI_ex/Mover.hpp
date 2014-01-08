#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include "DrawBuffer.hpp"
#include "luas.hpp"
#include "Rect.hpp"
#include "Observer.hpp"

#define VEL_MAX_Y 15 
#define GRAV 1

/* 移動物体全般 */

class Mover : public DrawBuffer::DrawInterface //, public Observer
{
	friend class FieldState;
interface__:
	Mover( int _index, Point<int> _pos );
	~Mover();
	void mainFlow();
	void mainFlow (  Point<int> const & _pos );  // 今フレームの位置を設定して実行
	class Block* getStandingBlockAddr() const;
	Rect<int> getBodyRectBeforeVelAdded() const;  // 速度加算前のボディを取得
	Rect<int> getBodyRectAfterVelAdded() const;  // 速度加算後のボディを取得
open__:
	Point<int> pos;   // 座標
	Point<int> vel;   // 速度
inner__:
	int const bodyX; // 当たり判定幅
	int const bodyY; // 当たり判定高さ
	int        index; // 識別番号
	map<string,sp<class Image> > m_pImg; // 画像
	map<string,sp<class Image> >::iterator it_m_pImg; // 画像イテレータ
	int inAirFrame; // 空中にいるフレーム数

inner__:
	// @ovr
	void libDraw();    // カメラ位置を考慮した描画（2013五月祭実装）
	void simpleDraw(); // posの位置にそのまま描画（Miraikan用）
	// 移動
	virtual void move() = 0;
	// 描画する画像更新
	void updateImage();
	// 画像ロード
	void loadImage();
	// 地形との当たり判定
	bool hitGroundEvent();
};

/* キャラクター */

class Chara : public Mover
{
interface__:
	Chara( int _index, Point<int> _pos );
	
inner__:
	// @ovr
	void move();
	// オブジェクトとの当たり判定
	bool hitObjectEvent();
};

/* オブジェクト */

class Object : public Mover
{
interface__:
	Object( int _index, Point<int> _pos );
	void hitAction( class Chara * _pChara );
inner__:
	// @ovr
	void move();
};
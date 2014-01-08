#pragma once
#include "includes.hpp"
#include "Point.hpp"

class DrawBuffer
{
open__:
	class DrawInterface
	{
    interface__:
		virtual void libDraw()  = 0;
	};

interface__:
	// バッファを空に（全命令をクリア）
	static void clearBuffer();
	// 描画開始命令
	static void drawStartCall( int _layer, DrawInterface* _inter );
	// 描画停止命令
	static  void drawStopCall( int _layer, DrawInterface* _inter );
	// バッファにたまっているものを全部描画
	static void drawAll();
inner__:
	static map<int,DrawInterface*> m_inter;
};

class MovieImage : public DrawBuffer::DrawInterface
{
interface__:
	MovieImage( string const & _dir, string const & _fileName, int _intervalFrame, double _xEx=1.0, double _yEx=1.0 );

	void libDraw();

open__:
	Point<int> position;    // ゲーム内座標の中心
	double xExRate;
	double yExRate;
inner__:
	string const fileDir;     // 動画ファイルのあるディレクトリ
	string const fileName;    // 動画ファイル名
	int const handle;         // ムービーハンドル
	int const intervalFrame;  // 描画間隔フレーム
	int xSize;
	int ySize;
	int frameCount;           // 1フレーム描画してから経過した時間
};
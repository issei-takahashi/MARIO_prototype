#pragma once
#include "includes.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Mutex.hpp"
#include "Timer.hpp"
#include "Point3D_mm.h"

class IO
{
open__:
	static Mutex mutex;

interface__:
	// 全てのデータを読み込み
	static void loadAllData();
	// 定数の値を取得
	static double getConst( string const & _name );
	// コンフィグデータ(xml)書き込み
	static void saveConfigData();
	// 操作データの書き込み
	static void writeInputData( Point3D_mm _point );

	// configDataのget
	static double get_zoom(){ return configData.zoom; }
	static Point<int> get_cameraPos() { return configData.cameraPos; }


	// 実行中に書き換えてセーブするデータ
	class ConfigData
	{
		friend class boost::serialization::access;
	public:
		ConfigData( double _zoom, int _startx, int _starty ) 
			: zoom(_zoom), cameraPos( Point<int>(_startx,_starty) ) {}
		double zoom;           // ブロックを100pxから何倍に拡大するか
		Point<int> cameraPos;  // フィールドが開始する「描画座標」
		Point<int> startPoint; // フィールド上で認識を開始する位置
		int zPixel;            // ブロック列が、z方向に何ピクセルごとにあるか
		Rect<int> deskRect;    // デプス画像内で机の領域を表す矩形
		Point<int> displayBiasPoint; // FieldState3Dで描画の中心位置
		Timer::LocalTime savedTime;  // 保存された時間（1秒精度）
		int additionalActuatorDown_mm; // 映像を余分に何mm手前に出すか（アクチュエータを余分に何mm奥に移動するか）
		int additionalShadowNear_mm;      // 影を余分に何mm手前に出すか
	private:
		ConfigData(){}
		//シリアライズ用
		template<class Archive>
		void serialize( Archive& ar, unsigned int ver )
		{
			ar & boost::serialization::make_nvp("savedTime", this->savedTime);
			ar & boost::serialization::make_nvp("zoom", this->zoom);
			ar & boost::serialization::make_nvp("cameraPos", this->cameraPos);
			ar & boost::serialization::make_nvp("startPoint", this->startPoint);
			ar & boost::serialization::make_nvp("zPixel", this->zPixel);
			ar & boost::serialization::make_nvp("deskRect", this->deskRect);
			ar & boost::serialization::make_nvp("displayBiasPoint", this->displayBiasPoint);
			ar & boost::serialization::make_nvp("additionalActuatorDown_mm", this->additionalActuatorDown_mm);
			ar & boost::serialization::make_nvp("additionalShadowNear_mm", this->additionalShadowNear_mm);
		}
	};

	static ConfigData configData;


capcelled__:
	static map< string, double > m_const;   // 定数テーブル


inner__:
	// パスを指定して定数データ(csv)読み込み
	static void loadConst( stringc& _path );
	// パスを指定してコンフィグデータ(xml)読み込み
	static void loadConfigData( string const & _path );

inner__:

};
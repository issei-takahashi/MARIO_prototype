#pragma once

#include "includes.hpp"
#include "Point.hpp"


/*----------------------class Mouse------------------------------

目的：マウスの位置、クリック情報、ホイール情報を取得するクラス。

属性：モノステート

--------------------------------------------------------------------*/

class Mouse
{
	friend class EditorUI;


public:

	//マウスの状態更新(毎フレーム行う)
	static void updateMouse();
	// 画面内にいるかどうか
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
	static Point<int> position;                // マウスの座標
	static bool isLeftClickStart;              // 左クリックが始まったフレームかどうか
	static bool isLeftClickEnd;                // 左クリックが終わったフレームか同化
	static double leftClickFrame;               // 左クリックされている時間(s)
	static bool isRightClickStart;             // 右クリックが始まったフレームかどうか
	static bool isRightClickEnd;               // 右クリックが終わったフレームかどうか
	static double rightClickFrame;              // 右クリックされている時間(s)
	static bool isWheelClickStart;             // ホイールクリックが始まったフレームかどうか
	static double wheelClickFrame;              // 右クリックされている時間(s)
	static int wheelRot;                       // ホイール回転数
	static bool doesMouseMove;                 // 今フレームにマウスが動いたかどうか
	static double lastMouseMoveFrame;           // 最後にマウスが移動した時間
	static int leftNotClickFrame;              // 左クリックがされていない時間
	static int rightNotClickFrame;            // 右クリックがされていない時間

};
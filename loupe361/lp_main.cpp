//----------------------------------------------------------------------------------
//		loupe361
//		ソースファイル (メイン)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "lp_header.hpp"



//---------------------------------------------------------------------
//      グローバル変数
//---------------------------------------------------------------------
HDC			bufDC = NULL;
HBITMAP		bufBMP = NULL;
int			bmp_width = 0;
int			bmp_height = 0;
int			nnn = 2;            // 倍率
POINT		pos = { 16,16 };    // 座標
POINT		sub_pos = { 0,0 };  // サブ座標（小数点以下の誤差）
POINT		drag_pos = { 0,0 }; // 前回のドラッグ位置

BOOL		bFixFlag = FALSE;
BOOL		bReverse = FALSE; // ホイールの回転方向逆転
BOOL		bRuler = TRUE;  // ルーラーの表示
BOOL		bMenu = TRUE;  // メニューの表示
BOOL		bDragFlagWin = FALSE; // ウインドウ上でドラッグ中
BOOL		bDragFlagEdit = FALSE; // 編集画面上でドラッグ中
int			gGrid = 0; // グリッド線



//---------------------------------------------------------------------
//      フィルタ構造体定義
//---------------------------------------------------------------------
FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_MAIN_MESSAGE |
	FILTER_FLAG_WINDOW_SIZE |
	FILTER_FLAG_DISP_FILTER |
	FILTER_FLAG_EX_INFORMATION |
	FILTER_FLAG_WINDOW_THICKFRAME,
	WINDOW_W,WINDOW_H,
	LP_PLUGIN_NAME,
	0,NULL,NULL,
	NULL,NULL,
	0,NULL,NULL,
	filter_proc,
	NULL,
	NULL,
	NULL,
	filter_wndproc,
	NULL,NULL,
	NULL,
	0,
	LP_PLUGIN_INFO,
	NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,
};
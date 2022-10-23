//----------------------------------------------------------------------------------
//		loupe361
//		ヘッダファイル
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include <stdio.h>
#include "filter.h"



//---------------------------------------------------------------------
//      マクロ
//---------------------------------------------------------------------
#define WINDOW_W 288
#define WINDOW_H 288
#define MAG_MAX  16 // 最大倍率
#define RULER_SIZE 8 // ルーラーの幅

// メニューのID
#define IDM_REVERSE     101
#define IDM_RULER       102
#define IDM_GRID_NONE   103
#define IDM_GRID_BLACK  104
#define IDM_GRID_WHITE  105

// プラグイン情報
#define LP_PLUGIN_NAME		"loupe361"
#define LP_PLUGIN_DEVELOPER	"mimaraka"
#define LP_PLUGIN_VERSION	"v0.1"
#define LP_PLUGIN_INFO		LP_PLUGIN_NAME " " LP_PLUGIN_VERSION " by " LP_PLUGIN_DEVELOPER



//---------------------------------------------------------------------
//      extern
//---------------------------------------------------------------------
extern FILTER_DLL		filter;
extern HDC				bufDC;
extern HBITMAP			bufBMP;
extern int				bmp_width;
extern int				bmp_height;
extern int				nnn;			// 倍率
extern POINT			pos;			// 座標
extern POINT			sub_pos;		// サブ座標（小数点以下の誤差）
extern POINT			drag_pos;		// 前回のドラッグ位置

extern BOOL				bFixFlag;
extern BOOL				bReverse;		// ホイールの回転方向逆転
extern BOOL				bRuler;			// ルーラーの表示
extern BOOL				bMenu;			// メニューの表示
extern BOOL				bDragFlagWin;	// ウインドウ上でドラッグ中
extern BOOL				bDragFlagEdit;	// 編集画面上でドラッグ中
extern int				gGrid;			// グリッド線



//---------------------------------------------------------------------
//      関数定義
//---------------------------------------------------------------------

// フィルタ関数
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTableYUY2(void);

BOOL filter_proc(FILTER* fp, FILTER_PROC_INFO* fpip);


// ウィンドウプロシージャ
BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp);


// 描画
void mem_alloc(FILTER* fp);
void mem_free(void);

void disp(FILTER* fp, void* editp, POINT* pp);


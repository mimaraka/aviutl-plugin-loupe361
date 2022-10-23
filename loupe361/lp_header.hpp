//----------------------------------------------------------------------------------
//		loupe361
//		�w�b�_�t�@�C��
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include <stdio.h>
#include "filter.h"



//---------------------------------------------------------------------
//      �}�N��
//---------------------------------------------------------------------
#define WINDOW_W 288
#define WINDOW_H 288
#define MAG_MAX  16 // �ő�{��
#define RULER_SIZE 8 // ���[���[�̕�

// ���j���[��ID
#define IDM_REVERSE     101
#define IDM_RULER       102
#define IDM_GRID_NONE   103
#define IDM_GRID_BLACK  104
#define IDM_GRID_WHITE  105

// �v���O�C�����
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
extern int				nnn;			// �{��
extern POINT			pos;			// ���W
extern POINT			sub_pos;		// �T�u���W�i�����_�ȉ��̌덷�j
extern POINT			drag_pos;		// �O��̃h���b�O�ʒu

extern BOOL				bFixFlag;
extern BOOL				bReverse;		// �z�C�[���̉�]�����t�]
extern BOOL				bRuler;			// ���[���[�̕\��
extern BOOL				bMenu;			// ���j���[�̕\��
extern BOOL				bDragFlagWin;	// �E�C���h�E��Ńh���b�O��
extern BOOL				bDragFlagEdit;	// �ҏW��ʏ�Ńh���b�O��
extern int				gGrid;			// �O���b�h��



//---------------------------------------------------------------------
//      �֐���`
//---------------------------------------------------------------------

// �t�B���^�֐�
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTableYUY2(void);

BOOL filter_proc(FILTER* fp, FILTER_PROC_INFO* fpip);


// �E�B���h�E�v���V�[�W��
BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp);


// �`��
void mem_alloc(FILTER* fp);
void mem_free(void);

void disp(FILTER* fp, void* editp, POINT* pp);


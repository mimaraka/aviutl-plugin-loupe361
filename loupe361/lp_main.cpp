//----------------------------------------------------------------------------------
//		loupe361
//		�\�[�X�t�@�C�� (���C��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "lp_header.hpp"



//---------------------------------------------------------------------
//      �O���[�o���ϐ�
//---------------------------------------------------------------------
HDC			bufDC = NULL;
HBITMAP		bufBMP = NULL;
int			bmp_width = 0;
int			bmp_height = 0;
int			nnn = 2;            // �{��
POINT		pos = { 16,16 };    // ���W
POINT		sub_pos = { 0,0 };  // �T�u���W�i�����_�ȉ��̌덷�j
POINT		drag_pos = { 0,0 }; // �O��̃h���b�O�ʒu

BOOL		bFixFlag = FALSE;
BOOL		bReverse = FALSE; // �z�C�[���̉�]�����t�]
BOOL		bRuler = TRUE;  // ���[���[�̕\��
BOOL		bMenu = TRUE;  // ���j���[�̕\��
BOOL		bDragFlagWin = FALSE; // �E�C���h�E��Ńh���b�O��
BOOL		bDragFlagEdit = FALSE; // �ҏW��ʏ�Ńh���b�O��
int			gGrid = 0; // �O���b�h��



//---------------------------------------------------------------------
//      �t�B���^�\���̒�`
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
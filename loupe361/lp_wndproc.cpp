//----------------------------------------------------------------------------------
//		loupe361
//		�\�[�X�t�@�C�� (�E�B���h�E�v���V�[�W��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "lp_header.hpp"



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	char title[1024] = LP_PLUGIN_NAME;
	static char fixed[128];

	switch (message)
	{
	case WM_FILTER_INIT:
		// INI����ݒ��ǂݍ���
		bReverse = !!fp->exfunc->ini_load_int(fp, (LPSTR)"reverse", 0); // ���o�[�X���[�h
		nnn = fp->exfunc->ini_load_int(fp, (LPSTR)"nnn", nnn);   // �O��̔{��
		bRuler = !!fp->exfunc->ini_load_int(fp, (LPSTR)"ruler", 1);   // ���[���[�\���̗L��
		bMenu = !!fp->exfunc->ini_load_int(fp, (LPSTR)"menu", 1);   // ���j���[�\���̗L��
		gGrid = fp->exfunc->ini_load_int(fp, (LPSTR)"grid", 0);    // �O���b�h��
		if (nnn < 1)       nnn = 1;
		if (nnn > MAG_MAX) nnn = MAG_MAX;

		// ���j���[�쐬
		if (bMenu)
		{
			HMENU hmenu = CreateMenu();
			HMENU hsubm = CreateMenu();
			MENUITEMINFO menuInfo;
			menuInfo.cbSize = sizeof(MENUITEMINFO);

			menuInfo.fMask = MIIM_TYPE | MIIM_SUBMENU;;
			menuInfo.fType = MFT_STRING;
			menuInfo.hSubMenu = hsubm;
			menuInfo.dwTypeData = TEXT("�ݒ�");
			InsertMenuItem(hmenu, 0, TRUE, &menuInfo);

			menuInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;

			menuInfo.dwTypeData = TEXT("�z�C�[�����]");
			menuInfo.wID = IDM_REVERSE;
			menuInfo.fState = bReverse ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 0, TRUE, &menuInfo);

			menuInfo.dwTypeData = TEXT("���[���[");
			menuInfo.wID = IDM_RULER;
			menuInfo.fState = bRuler ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 1, TRUE, &menuInfo);

			menuInfo.dwTypeData = TEXT("�O���b�h��(�Ȃ�)");
			menuInfo.wID = IDM_GRID_NONE;
			menuInfo.fState = (gGrid == 0) ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 2, TRUE, &menuInfo);

			menuInfo.dwTypeData = TEXT("�O���b�h��(��)");
			menuInfo.wID = IDM_GRID_BLACK;
			menuInfo.fState = (gGrid == 1) ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 3, TRUE, &menuInfo);

			menuInfo.dwTypeData = TEXT("�O���b�h��(��)");
			menuInfo.wID = IDM_GRID_WHITE;
			menuInfo.fState = (gGrid == 2) ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 4, TRUE, &menuInfo);

			SetMenu(hwnd, hmenu);
		}

		// �E�C���h�E�̃^�C�g����ύX
		sprintf_s(title, LP_PLUGIN_NAME " - %d�{ %s", nnn, fixed);
		SetWindowText(hwnd, (LPCTSTR)title);
		break;

	case WM_FILTER_EXIT:
		mem_free();
		// INI�ɐݒ��ۑ�
		fp->exfunc->ini_save_int(fp, (LPSTR)"nnn", nnn);          // �{��
		fp->exfunc->ini_save_int(fp, (LPSTR)"reverse", bReverse ? 1 : 0); // �z�C�[�����]
		fp->exfunc->ini_save_int(fp, (LPSTR)"ruler", bRuler ? 1 : 0);   // ���[���[�\���̗L��
		fp->exfunc->ini_save_int(fp, (LPSTR)"grid", gGrid ? 1 : 0);    // �O���b�h��
		break;

	case WM_PAINT:
	case WM_FILTER_CHANGE_EDIT:
	case WM_SIZE:
		disp(fp, editp, NULL);
		break;

	case WM_FILTER_FILE_OPEN:
		if (fp->exfunc->is_filter_window_disp(fp))
		{
			mem_alloc(fp);
		}
		break;

	case WM_FILTER_CHANGE_WINDOW:
		// �t�B���^�� �L��<-->���� �Ń����������
		if (fp->exfunc->is_filter_window_disp(fp))
		{
			mem_alloc(fp);
			return TRUE;
		}
		else
		{
			mem_free();
		}
		break;

	case WM_FILTER_FILE_CLOSE:
		mem_free();
		disp(fp, editp, NULL);
		break;

	case WM_FILTER_MAIN_MOUSE_DOWN:
		// �ҏW��ʂŃ}�E�X�{�^����������
		if (!bFixFlag)
		{
			// ���W��ύX
			POINT p;
			p.x = (int)((short)LOWORD(lparam));
			p.y = (int)((short)HIWORD(lparam));
			sub_pos.x = 0;
			sub_pos.y = 0;
			// ���W���摜���Ȃ�A�E�C���h�E�\���X�V
			if (p.x >= 0 && p.x < bmp_width && p.y >= 0 && p.y < bmp_height)
			{
				if (bufDC) disp(fp, editp, &p);
				bDragFlagEdit = TRUE;
				bDragFlagWin = FALSE;
			}
			else
			{
				bDragFlagWin = FALSE;
				bDragFlagEdit = FALSE;
			}
		}
		else
		{
			bDragFlagWin = FALSE;;
			bDragFlagEdit = FALSE;
		}
		break;

	case WM_FILTER_MAIN_MOUSE_UP:
	case WM_FILTER_MAIN_MOVESIZE:
	case WM_FILTER_MAIN_MOUSE_DBLCLK:
		// �ҏW��ʂł̃h���b�O�I��
		bDragFlagEdit = FALSE;
		bDragFlagWin = FALSE;
		break;

	case WM_FILTER_MAIN_MOUSE_MOVE:
		// �ҏW��ʂŃ}�E�X���ړ�
		if (!bFixFlag && bDragFlagEdit && !bDragFlagWin)
		{
			// �h���b�O�����A���W��ύX
			POINT p;
			p.x = (int)((short)LOWORD(lparam));
			p.y = (int)((short)HIWORD(lparam));
			sub_pos.x = 0;
			sub_pos.y = 0;
			if (bufDC) disp(fp, editp, &p);
		}
		else
		{
			bDragFlagWin = FALSE;
			bDragFlagEdit = FALSE;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
		// �E�C���h�E��Ń}�E�X�{�^����������
		if (!bFixFlag)
		{
			// �h���b�O�J�n
			bDragFlagWin = TRUE;
			bDragFlagEdit = FALSE;
			drag_pos.x = (int)((short)LOWORD(lparam));
			drag_pos.y = (int)((short)HIWORD(lparam));
			SetCursor(LoadCursor(NULL, IDC_SIZEALL)); // �J�[�\���ύX
			::SetCapture(hwnd);
		}
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
		// �E�C���h�E��ł̃h���b�O�I��
		bDragFlagWin = FALSE;
		bDragFlagEdit = FALSE;
		::ReleaseCapture();
		break;

	case WM_ACTIVATE:
		// �E�C���h�E�̔�A�N�e�B�u���ɂ��O�̂��߃h���b�O�t���O���N���A
		if (LOWORD(wparam) == 0)
		{
			bDragFlagWin = FALSE;
		}
		break;

	case WM_MOUSEMOVE:
		// �E�C���h�E��Ń}�E�X���ړ�
		if (!bFixFlag && bDragFlagWin)
		{
			// �h���b�O����
			POINT p;
			p.x = (int)((short)LOWORD(lparam));
			p.y = (int)((short)HIWORD(lparam));
			sub_pos.x += p.x - drag_pos.x;
			sub_pos.y += p.y - drag_pos.y;
			drag_pos.x = p.x;
			drag_pos.y = p.y;
			if (bufDC) disp(fp, editp, NULL);
			SetCursor(LoadCursor(NULL, IDC_SIZEALL)); // �J�[�\���ύX
		}
		else
		{
			bDragFlagWin = FALSE;
			bDragFlagEdit = FALSE;
		}
		break;

	//case WM_LBUTTONUP:
	//	// ���N���b�N���ꂽ�̂ō��W�Œ�
	//	bFixFlag = !bFixFlag;
	//	if (bFixFlag)
	//	{
	//		strcpy_s(fixed, "[���W�Œ�]");
	//		bDragFlagWin = FALSE;
	//		bDragFlagEdit = FALSE;
	//	}
	//	else
	//	{
	//		strcpy_s(fixed, " ");
	//	}
	//	sprintf_s(title, LP_PLUGIN_NAME " - %d�{ %s", nnn, fixed);
	//	SetWindowText(hwnd, (LPCTSTR)title);
	//	break;

	case WM_MOUSEWHEEL:
	{
		// �}�E�X�z�C�[���̉�]
		int d = (short)HIWORD(wparam);
		if ((!bReverse && d < 0) || (bReverse && d > 0))
		{
			if (nnn > 1)
			{
				// �k��
				nnn--;
				sprintf_s(title, LP_PLUGIN_NAME " - %d�{ %s", nnn, fixed);
				SetWindowText(hwnd, (LPCTSTR)title);
				if (bufDC) disp(fp, editp, NULL);
			}
			bDragFlagWin = FALSE;
			bDragFlagEdit = FALSE;
		}
		else if ((!bReverse && d > 0) || (bReverse && d < 0))
		{
			if (nnn < MAG_MAX)
			{
				// �g��
				nnn++;
				sprintf_s(title, LP_PLUGIN_NAME " - %d�{ %s", nnn, fixed);
				SetWindowText(hwnd, (LPCTSTR)title);
				if (bufDC) disp(fp, editp, NULL);
			}
			bDragFlagWin = FALSE;
			bDragFlagEdit = FALSE;
		}
	}
	break;

	case WM_RBUTTONUP:
		// �E�C���h�E��ŉE�N���b�N
		if (nnn > 1)
		{
			// �k��
			nnn--;
			sprintf_s(title, LP_PLUGIN_NAME " - %d�{ %s", nnn, fixed);
			SetWindowText(hwnd, (LPCTSTR)title);
			if (bufDC) disp(fp, editp, NULL);
		}
		bDragFlagWin = FALSE;
		bDragFlagEdit = FALSE;
		break;

	case WM_LBUTTONDBLCLK:
		// �E�C���h�E��ō��_�u���N���b�N
		if (nnn < MAG_MAX)
		{
			// �g��
			nnn++;
			sprintf_s(title, LP_PLUGIN_NAME " - %d�{ %s", nnn, fixed);
			SetWindowText(hwnd, (LPCTSTR)title);
			if (bufDC) disp(fp, editp, NULL);
		}
		bDragFlagWin = FALSE;
		bDragFlagEdit = FALSE;
		break;

	case WM_FILTER_COMMAND:
	case WM_COMMAND:
		// �X�e�[�^�X�o�[�̃R�}���h
		switch (LOWORD(wparam))
		{
		case IDM_REVERSE: // �z�C�[�����]
		{
			HMENU hmenu = GetMenu(hwnd);
			if (hmenu)
			{
				MENUITEMINFO menuInfo;
				menuInfo.cbSize = sizeof(MENUITEMINFO);
				menuInfo.fMask = MIIM_STATE;
				bReverse = !bReverse;
				menuInfo.fState = bReverse ? MFS_CHECKED : MFS_UNCHECKED;
				SetMenuItemInfo(hmenu, IDM_REVERSE, FALSE, &menuInfo);
			}
		}
		break;

		case IDM_RULER: // ���[���[
		{
			HMENU hmenu = GetMenu(hwnd);
			if (hmenu)
			{
				MENUITEMINFO menuInfo;
				menuInfo.cbSize = sizeof(MENUITEMINFO);
				menuInfo.fMask = MIIM_STATE;
				bRuler = !bRuler;
				menuInfo.fState = bRuler ? MFS_CHECKED : MFS_UNCHECKED;
				SetMenuItemInfo(hmenu, IDM_RULER, FALSE, &menuInfo);
			}
			disp(fp, editp, NULL);
		}
		break;

		case IDM_GRID_NONE: // �O���b�h�i�Ȃ��j
		{
			HMENU hmenu = GetMenu(hwnd);
			if (hmenu)
			{
				MENUITEMINFO menuInfo;
				menuInfo.cbSize = sizeof(MENUITEMINFO);
				menuInfo.fMask = MIIM_STATE;
				gGrid = 0;
				menuInfo.fState = MFS_CHECKED;
				SetMenuItemInfo(hmenu, IDM_GRID_NONE, FALSE, &menuInfo);
				menuInfo.fState = MFS_UNCHECKED;
				SetMenuItemInfo(hmenu, IDM_GRID_BLACK, FALSE, &menuInfo);
				SetMenuItemInfo(hmenu, IDM_GRID_WHITE, FALSE, &menuInfo);
			}
			disp(fp, editp, NULL);
		}
		break;

		case IDM_GRID_BLACK: // �O���b�h�i���j
		{
			HMENU hmenu = GetMenu(hwnd);
			if (hmenu)
			{
				MENUITEMINFO menuInfo;
				menuInfo.cbSize = sizeof(MENUITEMINFO);
				menuInfo.fMask = MIIM_STATE;
				gGrid = 1;
				menuInfo.fState = MFS_CHECKED;
				SetMenuItemInfo(hmenu, IDM_GRID_BLACK, FALSE, &menuInfo);
				menuInfo.fState = MFS_UNCHECKED;
				SetMenuItemInfo(hmenu, IDM_GRID_NONE, FALSE, &menuInfo);
				SetMenuItemInfo(hmenu, IDM_GRID_WHITE, FALSE, &menuInfo);
			}
			disp(fp, editp, NULL);
		}
		break;

		case IDM_GRID_WHITE: // �O���b�h�i���j
		{
			HMENU hmenu = GetMenu(hwnd);
			if (hmenu)
			{
				MENUITEMINFO menuInfo;
				menuInfo.cbSize = sizeof(MENUITEMINFO);
				menuInfo.fMask = MIIM_STATE;
				gGrid = 2;
				menuInfo.fState = MFS_CHECKED;
				SetMenuItemInfo(hmenu, IDM_GRID_WHITE, FALSE, &menuInfo);
				menuInfo.fState = MFS_UNCHECKED;
				SetMenuItemInfo(hmenu, IDM_GRID_NONE, FALSE, &menuInfo);
				SetMenuItemInfo(hmenu, IDM_GRID_BLACK, FALSE, &menuInfo);
			}
			disp(fp, editp, NULL);
		}
		break;
		}
		break;

	}
	return FALSE;
}
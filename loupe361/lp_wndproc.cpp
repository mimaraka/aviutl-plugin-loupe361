//----------------------------------------------------------------------------------
//		loupe361
//		ソースファイル (ウィンドウプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "lp_header.hpp"



//---------------------------------------------------------------------
//		ウィンドウプロシージャ
//---------------------------------------------------------------------
BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	char title[1024] = LP_PLUGIN_NAME;
	static char fixed[128];

	switch (message)
	{
	case WM_FILTER_INIT:
		// INIから設定を読み込む
		bReverse = !!fp->exfunc->ini_load_int(fp, (LPSTR)"reverse", 0); // リバースモード
		nnn = fp->exfunc->ini_load_int(fp, (LPSTR)"nnn", nnn);   // 前回の倍率
		bRuler = !!fp->exfunc->ini_load_int(fp, (LPSTR)"ruler", 1);   // ルーラー表示の有無
		bMenu = !!fp->exfunc->ini_load_int(fp, (LPSTR)"menu", 1);   // メニュー表示の有無
		gGrid = fp->exfunc->ini_load_int(fp, (LPSTR)"grid", 0);    // グリッド線
		if (nnn < 1)       nnn = 1;
		if (nnn > MAG_MAX) nnn = MAG_MAX;

		// メニュー作成
		if (bMenu)
		{
			HMENU hmenu = CreateMenu();
			HMENU hsubm = CreateMenu();
			MENUITEMINFO menuInfo;
			menuInfo.cbSize = sizeof(MENUITEMINFO);

			menuInfo.fMask = MIIM_TYPE | MIIM_SUBMENU;;
			menuInfo.fType = MFT_STRING;
			menuInfo.hSubMenu = hsubm;
			menuInfo.dwTypeData = TEXT("設定");
			InsertMenuItem(hmenu, 0, TRUE, &menuInfo);

			menuInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;

			menuInfo.dwTypeData = TEXT("ホイール反転");
			menuInfo.wID = IDM_REVERSE;
			menuInfo.fState = bReverse ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 0, TRUE, &menuInfo);

			menuInfo.dwTypeData = TEXT("ルーラー");
			menuInfo.wID = IDM_RULER;
			menuInfo.fState = bRuler ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 1, TRUE, &menuInfo);

			menuInfo.dwTypeData = TEXT("グリッド線(なし)");
			menuInfo.wID = IDM_GRID_NONE;
			menuInfo.fState = (gGrid == 0) ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 2, TRUE, &menuInfo);

			menuInfo.dwTypeData = TEXT("グリッド線(黒)");
			menuInfo.wID = IDM_GRID_BLACK;
			menuInfo.fState = (gGrid == 1) ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 3, TRUE, &menuInfo);

			menuInfo.dwTypeData = TEXT("グリッド線(白)");
			menuInfo.wID = IDM_GRID_WHITE;
			menuInfo.fState = (gGrid == 2) ? MFS_CHECKED : MFS_UNCHECKED;
			InsertMenuItem(hsubm, 4, TRUE, &menuInfo);

			SetMenu(hwnd, hmenu);
		}

		// ウインドウのタイトルを変更
		sprintf_s(title, LP_PLUGIN_NAME " - %d倍 %s", nnn, fixed);
		SetWindowText(hwnd, (LPCTSTR)title);
		break;

	case WM_FILTER_EXIT:
		mem_free();
		// INIに設定を保存
		fp->exfunc->ini_save_int(fp, (LPSTR)"nnn", nnn);          // 倍率
		fp->exfunc->ini_save_int(fp, (LPSTR)"reverse", bReverse ? 1 : 0); // ホイール反転
		fp->exfunc->ini_save_int(fp, (LPSTR)"ruler", bRuler ? 1 : 0);   // ルーラー表示の有無
		fp->exfunc->ini_save_int(fp, (LPSTR)"grid", gGrid ? 1 : 0);    // グリッド線
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
		// フィルタの 有効<-->無効 でメモリを解放
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
		// 編集画面でマウスボタンを押した
		if (!bFixFlag)
		{
			// 座標を変更
			POINT p;
			p.x = (int)((short)LOWORD(lparam));
			p.y = (int)((short)HIWORD(lparam));
			sub_pos.x = 0;
			sub_pos.y = 0;
			// 座標が画像内なら、ウインドウ表示更新
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
		// 編集画面でのドラッグ終了
		bDragFlagEdit = FALSE;
		bDragFlagWin = FALSE;
		break;

	case WM_FILTER_MAIN_MOUSE_MOVE:
		// 編集画面でマウスを移動
		if (!bFixFlag && bDragFlagEdit && !bDragFlagWin)
		{
			// ドラッグ処理、座標を変更
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
		// ウインドウ上でマウスボタンを押した
		if (!bFixFlag)
		{
			// ドラッグ開始
			bDragFlagWin = TRUE;
			bDragFlagEdit = FALSE;
			drag_pos.x = (int)((short)LOWORD(lparam));
			drag_pos.y = (int)((short)HIWORD(lparam));
			SetCursor(LoadCursor(NULL, IDC_SIZEALL)); // カーソル変更
			::SetCapture(hwnd);
		}
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
		// ウインドウ上でのドラッグ終了
		bDragFlagWin = FALSE;
		bDragFlagEdit = FALSE;
		::ReleaseCapture();
		break;

	case WM_ACTIVATE:
		// ウインドウの非アクティブ時にも念のためドラッグフラグをクリア
		if (LOWORD(wparam) == 0)
		{
			bDragFlagWin = FALSE;
		}
		break;

	case WM_MOUSEMOVE:
		// ウインドウ上でマウスが移動
		if (!bFixFlag && bDragFlagWin)
		{
			// ドラッグ処理
			POINT p;
			p.x = (int)((short)LOWORD(lparam));
			p.y = (int)((short)HIWORD(lparam));
			sub_pos.x += p.x - drag_pos.x;
			sub_pos.y += p.y - drag_pos.y;
			drag_pos.x = p.x;
			drag_pos.y = p.y;
			if (bufDC) disp(fp, editp, NULL);
			SetCursor(LoadCursor(NULL, IDC_SIZEALL)); // カーソル変更
		}
		else
		{
			bDragFlagWin = FALSE;
			bDragFlagEdit = FALSE;
		}
		break;

	//case WM_LBUTTONUP:
	//	// 中クリックされたので座標固定
	//	bFixFlag = !bFixFlag;
	//	if (bFixFlag)
	//	{
	//		strcpy_s(fixed, "[座標固定]");
	//		bDragFlagWin = FALSE;
	//		bDragFlagEdit = FALSE;
	//	}
	//	else
	//	{
	//		strcpy_s(fixed, " ");
	//	}
	//	sprintf_s(title, LP_PLUGIN_NAME " - %d倍 %s", nnn, fixed);
	//	SetWindowText(hwnd, (LPCTSTR)title);
	//	break;

	case WM_MOUSEWHEEL:
	{
		// マウスホイールの回転
		int d = (short)HIWORD(wparam);
		if ((!bReverse && d < 0) || (bReverse && d > 0))
		{
			if (nnn > 1)
			{
				// 縮小
				nnn--;
				sprintf_s(title, LP_PLUGIN_NAME " - %d倍 %s", nnn, fixed);
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
				// 拡大
				nnn++;
				sprintf_s(title, LP_PLUGIN_NAME " - %d倍 %s", nnn, fixed);
				SetWindowText(hwnd, (LPCTSTR)title);
				if (bufDC) disp(fp, editp, NULL);
			}
			bDragFlagWin = FALSE;
			bDragFlagEdit = FALSE;
		}
	}
	break;

	case WM_RBUTTONUP:
		// ウインドウ上で右クリック
		if (nnn > 1)
		{
			// 縮小
			nnn--;
			sprintf_s(title, LP_PLUGIN_NAME " - %d倍 %s", nnn, fixed);
			SetWindowText(hwnd, (LPCTSTR)title);
			if (bufDC) disp(fp, editp, NULL);
		}
		bDragFlagWin = FALSE;
		bDragFlagEdit = FALSE;
		break;

	case WM_LBUTTONDBLCLK:
		// ウインドウ上で左ダブルクリック
		if (nnn < MAG_MAX)
		{
			// 拡大
			nnn++;
			sprintf_s(title, LP_PLUGIN_NAME " - %d倍 %s", nnn, fixed);
			SetWindowText(hwnd, (LPCTSTR)title);
			if (bufDC) disp(fp, editp, NULL);
		}
		bDragFlagWin = FALSE;
		bDragFlagEdit = FALSE;
		break;

	case WM_FILTER_COMMAND:
	case WM_COMMAND:
		// ステータスバーのコマンド
		switch (LOWORD(wparam))
		{
		case IDM_REVERSE: // ホイール反転
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

		case IDM_RULER: // ルーラー
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

		case IDM_GRID_NONE: // グリッド（なし）
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

		case IDM_GRID_BLACK: // グリッド（黒）
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

		case IDM_GRID_WHITE: // グリッド（白）
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
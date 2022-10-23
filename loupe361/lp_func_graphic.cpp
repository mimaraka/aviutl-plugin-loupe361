//----------------------------------------------------------------------------------
//		loupe361
//		�\�[�X�t�@�C�� (�`��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "lp_header.hpp"



//---------------------------------------------------------------------
//      �f�o�C�X�R���e�L�X�g�Ǘ�
//---------------------------------------------------------------------
void mem_alloc(FILTER* fp)
{
	if (bufDC == NULL && bufBMP == NULL)
	{
		SYS_INFO si;
		fp->exfunc->get_sys_info(NULL, &si);
		HDC hdc = GetDC(fp->hwnd);
		bufDC = CreateCompatibleDC(hdc); // ��:��ʂ̐F���Ɠ����ɂȂ�
		bufBMP = CreateCompatibleBitmap(hdc, si.max_w, si.max_h); // VRAM�ɋ󂫂�����΂����Ɋm�ۂ����
		SelectObject(bufDC, bufBMP);
		DeleteDC(hdc);
	}
}



void mem_free(void)
{
	if (bufDC)
	{
		DeleteDC(bufDC);
		bufDC = NULL;
	}
	if (bufBMP)
	{
		DeleteObject((HGDIOBJ)bufBMP);
		bufBMP = NULL;
	}
}



//---------------------------------------------------------------------
//      �\��
//---------------------------------------------------------------------
void disp(FILTER* fp, void* editp, POINT* pp)
{
	HDC hdc;
	RECT rc, blank_rc;
	HRGN hRgn = NULL;
	BOOL is_editing = FALSE;

	if (fp) is_editing = fp->exfunc->is_editing(editp);


	if (bufDC && is_editing)
	{
		hdc = GetDC(fp->hwnd);
		GetClientRect(fp->hwnd, &rc);

		if (pp)
		{
			pos.x = pp->x;
			pos.y = pp->y;
			sub_pos.x = sub_pos.y = 0;
		}

		// �T�u���W�i�덷�j�����C�����W�ɔ��f
		pos.x -= sub_pos.x / nnn;
		sub_pos.x %= nnn;
		pos.y -= sub_pos.y / nnn;
		sub_pos.y %= nnn;

		int center_x = (rc.right - rc.left) / 2;
		int center_y = (rc.bottom - rc.top) / 2;
		int dst_x = rc.left - pos.x * nnn + center_x + sub_pos.x;
		int dst_y = rc.top - pos.y * nnn + center_y + sub_pos.y;
		int src_width = bmp_width;
		int src_height = bmp_height;
		int dst_width = src_width * nnn;
		int dst_height = src_height * nnn;
		int src_x = 0;
		int src_y = 0;

		// �E�C���h�E�O�̔�\���������G�c�ȃN���b�s���O
		// �i���Ȃ��Ă��ǂ�������������j
		if (dst_x < rc.left)
		{
			int d = rc.left - dst_x;
			int d_dst = d - d % nnn;
			int d_src = d / nnn;
			dst_x += d_dst;
			dst_width -= d_dst;
			src_x += d_src;
			src_width -= d_src;
		}
		if (dst_x + dst_width > rc.right)
		{
			int d = (dst_x + dst_width) - rc.right;
			int d_dst = d - d % nnn;
			int d_src = d / nnn;
			dst_width -= d_dst;
			src_width -= d_src;
		}
		if (dst_y < rc.top)
		{
			int d = rc.top - dst_y;
			int d_dst = d - d % nnn;
			int d_src = d / nnn;
			dst_y += d_dst;
			dst_height -= d_dst;
			src_y += d_src;
			src_height -= d_src;
		}
		if (dst_y + dst_height > rc.bottom)
		{
			int d = (dst_y + dst_height) - rc.bottom;
			int d_dst = d - d % nnn;
			int d_src = d / nnn;
			dst_height -= d_dst;
			src_height -= d_src;
		}

		// ���[���[�i�ڐ���j
		if (bRuler && nnn >= 2)
		{
			HPEN hPen = (HPEN)GetStockObject(BLACK_PEN); // �y��
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
			// �܂����[���[�w�i�𔒂œh��Ԃ�
			HBRUSH hBG = (HBRUSH)GetStockObject(WHITE_BRUSH);
			blank_rc.left = rc.left;
			blank_rc.right = rc.right;
			blank_rc.top = rc.top;
			blank_rc.bottom = rc.top + RULER_SIZE;
			FillRect(hdc, &blank_rc, hBG);
			blank_rc.left = rc.left;
			blank_rc.right = rc.left + RULER_SIZE;
			blank_rc.top = rc.top;
			blank_rc.bottom = rc.bottom;
			FillRect(hdc, &blank_rc, hBG);
			// ��������Ɖ��ɕ\��
			for (int sx = 0; sx <= src_width; sx++)
			{
				int dx = dst_x + sx * nnn;
				if (dx >= RULER_SIZE)
				{
					int h = ((src_x + sx) % 10 == 0) ? RULER_SIZE : (RULER_SIZE + 2) / 3;
					MoveToEx(hdc, dx, 0, NULL);
					LineTo(hdc, dx, h);
				}
			}
			// �c�������ƉE�ɕ\��
			for (int sy = 0; sy <= src_height; sy++)
			{
				int dy = dst_y + sy * nnn;
				if (dy >= RULER_SIZE)
				{
					int h = ((src_y + sy) % 10 == 0) ? RULER_SIZE : (RULER_SIZE + 2) / 3;
					MoveToEx(hdc, 0, dy, NULL);
					LineTo(hdc, h, dy);
				}
			}
			SelectObject(hdc, hOldPen); // �y����߂�
		}

		// ���[���[�������Ȃ��悤�ɃN���b�s���O�ݒ�
		if (bRuler && nnn >= 2)
		{
			hRgn = CreateRectRgn(rc.left + RULER_SIZE, rc.top + RULER_SIZE, rc.right, rc.bottom);
			SelectClipRgn(hdc, hRgn);
		}

		// ���ɗ]��������Ȃ�h��Ԃ�
		if (dst_x > rc.left)
		{
			blank_rc.left = rc.left;
			blank_rc.right = dst_x;
			blank_rc.top = rc.top;
			blank_rc.bottom = rc.bottom;
			FillRect(hdc, &blank_rc, (HBRUSH)(COLOR_ACTIVEBORDER + 1));
		}
		// ��ɗ]��������Ȃ�h��Ԃ�
		if (dst_y > rc.top)
		{
			blank_rc.left = rc.left;
			blank_rc.right = rc.right;
			blank_rc.top = rc.top;
			blank_rc.bottom = dst_y;
			FillRect(hdc, &blank_rc, (HBRUSH)(COLOR_ACTIVEBORDER + 1));
		}
		// �E�ɗ]��������Ȃ�h��Ԃ�
		if (dst_x + dst_width < rc.right)
		{
			blank_rc.left = dst_x + dst_width;
			blank_rc.right = rc.right;
			blank_rc.top = rc.top;
			blank_rc.bottom = rc.bottom;
			FillRect(hdc, &blank_rc, (HBRUSH)(COLOR_ACTIVEBORDER + 1));
		}
		// ���ɗ]��������Ȃ�h��Ԃ�
		if (dst_y + dst_height < rc.bottom)
		{
			blank_rc.left = rc.left;
			blank_rc.right = rc.right;
			blank_rc.top = dst_y + dst_height;
			blank_rc.bottom = rc.bottom;
			FillRect(hdc, &blank_rc, (HBRUSH)(COLOR_ACTIVEBORDER + 1));
		}

		// [StretchDIBits�Ŋg��]
		// StretchDIBits�͋𒼂Ɋg�又�������Ă���]�����Ă���炵���B
		// ��\���������g�又�����邽�߁A���̕����Ԃ��|�����Ă��܂��B
		// �����őO�i�K�ő�G�c�ɃN���b�s���O���邱�Ƃő��x�A�b�v���Ă�B
		if (src_width > 0 && src_height > 0)
		{
			SetStretchBltMode(hdc, COLORONCOLOR);
			StretchBlt(
				hdc, dst_x, dst_y, dst_width, dst_height,
				bufDC, src_x, src_y, src_width, src_height,
				SRCCOPY);
		}

		// �O���b�h����`��
		if (gGrid >= 1 && nnn >= 2)
		{
			HPEN hPen = (HPEN)GetStockObject((gGrid == 1) ? BLACK_PEN : WHITE_PEN); // �y��
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
			// �c��
			for (int sx = 0; sx < src_width; sx++)
			{
				int dx = dst_x + sx * nnn;
				MoveToEx(hdc, dx, 0, NULL);
				LineTo(hdc, dx, rc.bottom);
			}
			// ����
			for (int sy = 0; sy < src_height; sy++)
			{
				int dy = dst_y + sy * nnn;
				MoveToEx(hdc, 0, dy, NULL);
				LineTo(hdc, rc.right, dy);
			}
			SelectObject(hdc, hOldPen); // �y����߂�
		}

		ReleaseDC(fp->hwnd, hdc);
		if (bRuler && nnn >= 2) DeleteObject(hRgn);
	}
	else
	{
		hdc = GetDC(fp->hwnd);
		GetClientRect(fp->hwnd, &rc);
		FillRect(hdc, &rc, (HBRUSH)(COLOR_ACTIVEBORDER + 1));
		ReleaseDC(fp->hwnd, hdc);
	}
}
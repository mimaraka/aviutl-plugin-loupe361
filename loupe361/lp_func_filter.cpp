//----------------------------------------------------------------------------------
//		loupe361
//		�\�[�X�t�@�C�� (�t�B���^�֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "lp_header.hpp"



//---------------------------------------------------------------------
//      �t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
    return &filter;
}



//---------------------------------------------------------------------
//      YUY2�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTableYUY2(void)
{
    return &filter;
}



//---------------------------------------------------------------------
// �t�B���^�����֐�
//---------------------------------------------------------------------
BOOL filter_proc(FILTER* fp, FILTER_PROC_INFO* fpip)
{
    void* editp = fpip->editp;
    BOOL is_editing = fp->exfunc->is_editing(editp);

    if (bufDC && is_editing)
    {
        int w = fpip->w;
        int h = fpip->h;

        bmp_width = w;
        bmp_height = h;

        // �r�b�g�}�b�v�̃w�b�_���쐬
        BITMAPINFO bmi;
        ZeroMemory(&bmi, sizeof(bmi));
        bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 24;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biPlanes = 1;

        // �r�b�g�}�b�v��bufDC�ɃR�s�[
        void* pixelp = fp->exfunc->get_disp_pixelp(editp, NULL);
        SetDIBitsToDevice(
            bufDC,
            0, 0, w, h,
            0, 0, 0, h,
            pixelp, &bmi, DIB_RGB_COLORS);

        disp(fp, editp, NULL); // �\��
    }

    return TRUE;
}
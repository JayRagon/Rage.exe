#pragma once
#pragma warning (disable : 6031)

#include <Windows.h>
#include <iostream>
#include <ole2.h>
#include <olectl.h>

using namespace std;

bool SaveBitmap(LPCSTR filename, HBITMAP bmp, HPALETTE pal)
{
    bool result = false;
    PICTDESC pd;

    pd.cbSizeofstruct = sizeof(PICTDESC);
    pd.picType = PICTYPE_BITMAP;
    pd.bmp.hbitmap = bmp;
    pd.bmp.hpal = pal;

    LPPICTURE picture;
    HRESULT res = OleCreatePictureIndirect(&pd, IID_IPicture, false,
        reinterpret_cast<void**>(&picture));

    if (!SUCCEEDED(res))
        return false;

    LPSTREAM stream;
    res = CreateStreamOnHGlobal(0, true, &stream);

    if (!SUCCEEDED(res))
    {
        picture->Release();
        return false;
    }

    LONG bytes_streamed;
    res = picture->SaveAsFile(stream, true, &bytes_streamed);

    HANDLE file = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ, 0,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (!SUCCEEDED(res) || !file)
    {
        stream->Release();
        picture->Release();
        return false;
    }

    HGLOBAL mem = 0;
    GetHGlobalFromStream(stream, &mem);
    LPVOID data = GlobalLock(mem);

    DWORD bytes_written;

    result = !!WriteFile(file, data, bytes_streamed, &bytes_written, 0);
    result &= (bytes_written == static_cast<DWORD>(bytes_streamed));

    GlobalUnlock(mem);
    CloseHandle(file);

    stream->Release();
    picture->Release();

    return result;
}

bool ScreenCapture(int x, int y, int w, int h, LPCSTR fname, HBITMAP& outbit) 
{
    HDC hdcSource = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);

    HPALETTE hpal = NULL;

    outbit = hBitmap;

    if (SaveBitmap(fname, hBitmap, hpal)) return true;

    return false;
}


bool ScreenCaptureNBM(int x, int y, int w, int h, HBITMAP& outbit)
{
    HDC hdcSource = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);

    HPALETTE hpal = NULL;

    outbit = hBitmap;

    return true;
}
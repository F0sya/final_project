#include "lib.h"

HBITMAP CaptureWindow(HWND hwnd) {
    HDC hdcWindow = GetDC(hwnd);              
    HDC hdcMemDC = CreateCompatibleDC(hdcWindow); 

    RECT rect;
    GetClientRect(hwnd, &rect);              
    int width = rect.right;
    int height = rect.bottom;

    // Create a compatible bitmap
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
    SelectObject(hdcMemDC, hBitmap);

    // Copy the window content to the memory DC
    BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

    // Clean up
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcWindow);

    return hBitmap;
}

bool SaveBitmapAsBMP(HBITMAP hBitmap, LPCSTR filename) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader = {};
    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = -bmp.bmHeight; // Negative height to flip the bitmap (top-down)
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = bmp.bmWidth * bmp.bmHeight * 4;

    DWORD dwBmpSize = bi.biSizeImage;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char* lpBitmapData = (char*)GlobalLock(hDIB);

    HDC hDC = CreateCompatibleDC(NULL);
    GetDIBits(hDC, hBitmap, 0, bmp.bmHeight, lpBitmapData, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Create a file to save the bitmap
    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Fill the file header
    DWORD dwSizeofDIB = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    bmfHeader.bfType = 0x4D42; // "BM"
    bmfHeader.bfSize = dwSizeofDIB;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Write the file header
    DWORD dwBytesWritten;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpBitmapData, dwBmpSize, &dwBytesWritten, NULL);

    // Cleanup
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    CloseHandle(hFile);
    DeleteDC(hDC);

    return true;
}
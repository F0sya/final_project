#include "jpgSaver.h"

bool JpgSaver::SaveWindowAsJpg(HWND hwnd, const std::wstring& filename, int quality) {
    
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HDC hdcWindow = GetDC(hwnd);
    HDC hdcMemDC = CreateCompatibleDC(hdcWindow);

    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemDC, hBitmap);
    BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

   
    std::unique_ptr<Gdiplus::Bitmap> pBitmap(Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL));


    CLSID clsidEncoder;
    GetEncoderClsid(L"image/jpeg", &clsidEncoder);

    Gdiplus::EncoderParameters encoderParameters;
    encoderParameters.Count = 1;
    encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
    encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues = 1;
    ULONG qualityValue = quality;
    encoderParameters.Parameter[0].Value = &qualityValue;

 
    Gdiplus::Status status = pBitmap->Save(filename.c_str(), &clsidEncoder, &encoderParameters);


    SelectObject(hdcMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcWindow);
    Gdiplus::GdiplusShutdown(gdiplusToken);

    return status == Gdiplus::Ok;
}


int JpgSaver::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    std::unique_ptr<Gdiplus::ImageCodecInfo> pImageCodecInfo((Gdiplus::ImageCodecInfo*)malloc(size));
    if (!pImageCodecInfo) return -1;

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo.get());

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo.get()[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo.get()[j].Clsid;
            return j;
        }
    }
    return -1;
}
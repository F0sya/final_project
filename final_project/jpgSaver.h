#pragma once
#include "lib.h"


class JpgSaver {
public:
    static bool SaveWindowAsJpg(HWND hwnd, const std::wstring& filename, int quality = 90);
private:
    static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};

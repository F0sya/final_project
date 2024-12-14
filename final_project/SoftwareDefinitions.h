#pragma once



#define OnClearField 1
#define OnSetField 2
#define OnCalculate 3
#define OnExitSoftware 4
#define OnMenuAction1 5

#define TextBufferSize 6

char Buffer[TextBufferSize];

HWND hParameterControl_x0;
HWND hParameterControl_y0;
HWND hParameterControl_v0;
HWND static_entry_angle;
HWND static_entry_y0;
HWND static_entry_v0;
HWND static_result_Hmax;
HWND static_result_Xmax;



LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);

void MainWndAddMenus(HWND hWnd);
void MainWndAddWidgets(HWND hWnd);
#include "domain.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION),
		L"MainWndClass", SoftwareMainProcedure);

	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }
	MSG SoftwareMainMessage = { 0 };

	CreateWindow(L"MainWndClass", L"First c++ window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 650, 550, NULL, NULL, NULL, NULL);
	while (GetMessage(&SoftwareMainMessage, NULL, NULL, NULL)) {
		TranslateMessage(&SoftwareMainMessage);
		DispatchMessage(&SoftwareMainMessage);
	}
}

WNDCLASS NewWindowClass(HBRUSH BGCOLOR, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure) {

	WNDCLASS NWC = { 0 };

	NWC.hIcon = Icon;
	NWC.hCursor = Cursor;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGCOLOR;
	NWC.lpfnWndProc = Procedure;
	
	return NWC;
}


LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {

	case WM_COMMAND:
		switch (wp) {
		case OnMenuAction1:
			MessageBoxA(hWnd, "Menu 1 was clicked!", "Menu 1 worked", MB_OK);
			break;
		case OnSetField:
			if (GetWindowTextLengthA(hParameterControl_x0) == 0 || GetWindowTextLengthA(hParameterControl_y0) == 0 || GetWindowTextLengthA(hParameterControl_v0) == 0) { MessageBoxA(hWnd,"Fill all entries!", "Error!", MB_ICONWARNING); }
			else {
				GetWindowTextA(hParameterControl_x0, Buffer, TextBufferSize);
				SetWindowTextA(static_entry_angle, Buffer);
				SetWindowTextA(static_result_Hmax, Buffer);
				SetWindowTextA(static_result_Xmax, Buffer);
				GetWindowTextA(hParameterControl_y0, Buffer, TextBufferSize);
				SetWindowTextA(static_entry_y0, Buffer);
				GetWindowTextA(hParameterControl_v0, Buffer, TextBufferSize);
				SetWindowTextA(static_entry_v0, Buffer);
			}
			break;
		case OnCalculate: {
			bool keepgoing = true;
			if (GetWindowTextLengthA(static_entry_angle) > 0) {
				GetWindowTextA(static_entry_angle, Buffer, TextBufferSize);
				angle = atof(Buffer);
			}

			if (GetWindowTextLengthA(static_entry_y0) > 0) {
				GetWindowTextA(static_entry_y0, Buffer, TextBufferSize);
				initialHeight = atof(Buffer);
			}

			if (GetWindowTextLengthA(static_entry_v0) > 0) {
				GetWindowTextA(static_entry_v0, Buffer, TextBufferSize);
				initialVelocity = atof(Buffer);
			}

			try {
				if (angle < -90 || initialHeight < 0 || initialVelocity < 0 || initialHeight > 500 || angle > 90) {
					throw "Invalid input. Don't use negative or too big numbers";
				}
			}
			catch (const char* exception) {
				SetWindowTextA(static_entry_angle, "");
				SetWindowTextA(static_entry_y0, "");
				SetWindowTextA(static_entry_v0, "");
				SetWindowTextA(hParameterControl_x0, "");
				SetWindowTextA(hParameterControl_y0, "");
				SetWindowTextA(hParameterControl_v0, "");
				MessageBoxA(hWnd, "Invalid input.Don't use negative or too big numbers", "Input Error", MB_ICONERROR);
				keepgoing = false;
			}

			if (keepgoing) {
				Simulation::SimulationWindow simWindow((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), L"ProjectileSimulation", L"Projectile Motion Simulation");
				if (!simWindow.create()) {
					MessageBoxA(hWnd, "Failed to create the simulation window.", "Error", MB_ICONERROR);
					break;
				}

				simWindow.show(SW_SHOW);
			}
			break;
		}
		case OnClearField:
			SetWindowTextA(static_entry_angle, "");
			SetWindowTextA(static_entry_y0, "");
			SetWindowTextA(static_entry_v0, "");
			SetWindowTextA(hParameterControl_x0, "");
			SetWindowTextA(hParameterControl_y0, "");
			SetWindowTextA(hParameterControl_v0, "");
			break;
		case OnExitSoftware:
			PostQuitMessage(0);
			break;
		default: break;
		}
		break;

	case WM_CREATE:
		MainWndAddMenus(hWnd);
		MainWndAddWidgets(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void MainWndAddMenus(HWND hWnd) {
	HMENU RootMenu = CreateMenu();
	HMENU SubMenu = CreateMenu();

	AppendMenu(SubMenu, MF_STRING, OnMenuAction1, L"Menu 1");
	AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(SubMenu, MF_STRING, OnExitSoftware, L"Close");

	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");
	AppendMenu(RootMenu, MF_STRING, (UINT_PTR)SubMenu, L"Help");

	SetMenu(hWnd, RootMenu);
}

void MainWndAddWidgets(HWND hWnd) {
	//non-entries statics
	CreateWindowA("static", "Movement of a thrown body", WS_VISIBLE | WS_CHILD, 20, 5, 350, 30, hWnd, NULL, NULL, NULL);
	CreateWindowA("static", "Entries:", WS_VISIBLE | WS_CHILD, 250, 20, 350, 30, hWnd, NULL, NULL, NULL);

	CreateWindowA("static", "a = ", WS_VISIBLE | WS_CHILD | ES_CENTER, 300, 20, 50, 30, hWnd, NULL, NULL, NULL);
	static_entry_angle = CreateWindowA("static", "", WS_VISIBLE | WS_CHILD | ES_CENTER, 340, 20, 50, 30, hWnd, NULL, NULL, NULL);

	CreateWindowA("static", "H = ", WS_VISIBLE | WS_CHILD | ES_CENTER, 410, 20, 50, 30, hWnd, NULL, NULL, NULL);
	static_entry_y0 = CreateWindowA("static", "", WS_VISIBLE | WS_CHILD | ES_CENTER, 450, 20, 50, 30, hWnd, NULL, NULL, NULL);

	CreateWindowA("static", "V0 = ", WS_VISIBLE | WS_CHILD | ES_CENTER, 510, 20, 50, 30, hWnd, NULL, NULL, NULL);
	static_entry_v0 = CreateWindowA("static", "", WS_VISIBLE | WS_CHILD | ES_CENTER, 550, 20, 50, 30, hWnd, NULL, NULL, NULL);


	//entry a - angle
	CreateWindowA("static", "a:", WS_VISIBLE | WS_CHILD, 5, 40, 100, 30, hWnd, NULL, NULL, NULL);
	hParameterControl_x0 = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 30, 39, 50, 20, hWnd, NULL, NULL, NULL);

	//entry y0 - start y pos
	CreateWindowA("static", "H:", WS_VISIBLE | WS_CHILD, 5, 90, 100, 30, hWnd, NULL, NULL, NULL);
	hParameterControl_y0 = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER , 30, 89, 50, 20, hWnd, NULL, NULL, NULL);

	//entry v0 - starting velocity
	CreateWindowA("static", "V0:", WS_VISIBLE | WS_CHILD, 5, 140, 100, 30, hWnd, NULL, NULL, NULL);
	hParameterControl_v0 = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER , 30, 139, 50, 20, hWnd, NULL, NULL, NULL);

	//buttons
	CreateWindowA("button", "Set", WS_VISIBLE | WS_CHILD | ES_CENTER, 100, 45, 135, 30, hWnd, (HMENU)OnSetField, NULL, NULL);
	CreateWindowA("button", "Calculate and Build", WS_VISIBLE | WS_CHILD | ES_CENTER, 100, 80, 135, 30, hWnd, (HMENU)OnCalculate, NULL, NULL);
	CreateWindowA("button", "Clear", WS_VISIBLE | WS_CHILD | ES_CENTER, 100, 115, 135, 30, hWnd, (HMENU)OnClearField, NULL, NULL);


}

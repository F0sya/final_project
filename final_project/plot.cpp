// plot.cpp
#define _CRT_SECURE_NO_WARNINGS
#define GRAVITY 9.81
#include "plot.h"

double initialVelocity = 0.0, angle = 0.0, initialHeight = 0.0;

namespace Simulation {

    SimulationWindow::SimulationWindow(HINSTANCE hInst, LPCWSTR className, LPCWSTR title) 
        : hInstance(hInst), className(className), windowTitle(title), hwnd(nullptr), scaleX(0.0), scaleY(0.0) {}

    bool SimulationWindow::create() {
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = className;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        if (!RegisterClass(&wc)) return false;

        hwnd = CreateWindow(className, windowTitle, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

        if (!hwnd) return false;


        createControls(hwnd);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        return true;
    }

    void SimulationWindow::show(int nCmdShow) {
        ShowWindow(hwnd, nCmdShow);
    }

    void SimulationWindow::saveToJpg() {
        // Создаем имя файла с текущей датой и временем
        SYSTEMTIME st;
        GetLocalTime(&st);
        wchar_t filename[MAX_PATH];
        swprintf(filename, MAX_PATH, L"trajectory_%04d%02d%02d_%02d%02d%02d.jpg",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        if (JpgSaver::SaveWindowAsJpg(hwnd, filename)) {
            MessageBoxW(hwnd, (L"Graph saved as " + std::wstring(filename)).c_str(),
                L"Success", MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBoxW(hwnd, L"Failed to save graph", L"Error", MB_OK | MB_ICONERROR);
        }
    }
    void SimulationWindow::createControls(HWND hWnd) {
        hCheckboxFixAngle = CreateWindowA("button", "Fix Angle",
            WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
            10, 10, 100, 25, hWnd, NULL, hInstance, NULL);
        SendMessage(hCheckboxFixAngle, BM_SETCHECK, angleFixed ? BST_CHECKED : BST_UNCHECKED, 0);

        hCheckboxFixHeight = CreateWindowA("button", "Fix Height",
            WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
            150, 10, 100, 25, hWnd, NULL, hInstance, NULL);
        SendMessage(hCheckboxFixHeight, BM_SETCHECK, heightFixed ? BST_CHECKED : BST_UNCHECKED, 0);

        hCheckboxFixVelocity = CreateWindowA("button", "Fix Velocity",
            WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
            290, 10, 100, 25, hWnd, NULL, hInstance, NULL);
        SendMessage(hCheckboxFixVelocity, BM_SETCHECK, velocityFixed ? BST_CHECKED : BST_UNCHECKED, 0);

        // Отключаем получение фокуса чекбоксами
        SetWindowLong(hCheckboxFixAngle, GWL_STYLE,
            GetWindowLong(hCheckboxFixAngle, GWL_STYLE) & ~WS_TABSTOP);
        SetWindowLong(hCheckboxFixHeight, GWL_STYLE,
            GetWindowLong(hCheckboxFixHeight, GWL_STYLE) & ~WS_TABSTOP);
        SetWindowLong(hCheckboxFixVelocity, GWL_STYLE,
            GetWindowLong(hCheckboxFixVelocity, GWL_STYLE) & ~WS_TABSTOP);
    }
    void SimulationWindow::addTrajectory(double velocity, double angle, double height, COLORREF color) {

        if (!hwnd) return;


        if (color == 0) {
            color = availableColors[trajectories.size() % availableColors.size()];
        }


        double finalAngle = angleFixed ? fixedAngleValue : angle;
        double finalHeight = heightFixed ? fixedHeightValue : height;
        double finalVelocity = velocityFixed ? fixedVelocityValue : velocity;


        trajectories.push_back({ finalVelocity, finalAngle, finalHeight, color });


        if (angleFixed) fixedAngleValue = finalAngle;
        if (heightFixed) fixedHeightValue = finalHeight;
        if (velocityFixed) fixedVelocityValue = finalVelocity;

        InvalidateRect(hwnd, nullptr, TRUE);


        SetFocus(hwnd);
    }

    void SimulationWindow::setScale(double x, double y) {
        scaleX = x;
        scaleY = y;
        InvalidateRect(hwnd, nullptr, TRUE);
    }
    void SimulationWindow::drawTrajectory(HDC hdc) {
        const int screenWidth = 800;
        const int screenHeight = 600;
        const int xOffset = 70;
        const int yOffset = 70;
        const int axisLabelOffset = 10;

   
        double maxX = 0.0, maxY = 0.0;
        for (const auto& traj : trajectories) {
            double angleRad = traj.angle * M_PI / 180.0;
            double vx = traj.velocity * cos(angleRad);
            double vy = traj.velocity * sin(angleRad);
            double t_flight = (vy + sqrt(vy * vy + 2 * GRAVITY * traj.height)) / GRAVITY;
            double x = vx * t_flight;
            double y = traj.height + (vy * vy) / (2 * GRAVITY);
            if (x > maxX) maxX = x;
            if (y > maxY) maxY = y;
        }

        
        double usedScaleX = (scaleX > 0) ? scaleX : (screenWidth - 2 * xOffset) / maxX;
        double usedScaleY = (scaleY > 0) ? scaleY : (screenHeight - 2 * yOffset) / maxY;

        
        HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
        RECT rect = { 0, 0, screenWidth, screenHeight };
        FillRect(hdc, &rect, bgBrush);
        DeleteObject(bgBrush);

        
        HPEN gridPen = CreatePen(PS_DOT, 1, RGB(200, 200, 200));
        HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

        
        for (double x = 0; x <= maxX; x += maxX / 10) {
            int screenX = xOffset + static_cast<int>(x * usedScaleX);
            MoveToEx(hdc, screenX, yOffset, NULL);
            LineTo(hdc, screenX, screenHeight - yOffset);
        }

        
        for (double y = 0; y <= maxY; y += maxY / 10) {
            int screenY = screenHeight - yOffset - static_cast<int>(y * usedScaleY);
            MoveToEx(hdc, xOffset, screenY, NULL);
            LineTo(hdc, screenWidth - xOffset, screenY);
        }

     
        HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        SelectObject(hdc, axisPen);

        
        MoveToEx(hdc, xOffset, screenHeight - yOffset, NULL);
        LineTo(hdc, screenWidth - xOffset, screenHeight - yOffset);

       
        MoveToEx(hdc, xOffset, screenHeight - yOffset, NULL);
        LineTo(hdc, xOffset, yOffset);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));


        for (double x = 0; x <= maxX; x += maxX / 10) {
            int screenX = xOffset + static_cast<int>(x * usedScaleX);
            std::string label = std::to_string(static_cast<int>(x));
            TextOutA(hdc, screenX - 10, screenHeight - yOffset + axisLabelOffset, label.c_str(), label.length());
        }

      
        for (double y = 0; y <= maxY; y += maxY / 10) {
            int screenY = screenHeight - yOffset - static_cast<int>(y * usedScaleY);
            std::string label = std::to_string(static_cast<int>(y));
            TextOutA(hdc, xOffset - 30, screenY - 8, label.c_str(), label.length());
        }

      
        TextOutA(hdc, screenWidth - xOffset + 10, screenHeight - yOffset - 10, "X (m)", 5);
        TextOutA(hdc, xOffset - 20, yOffset - 20, "Y (m)", 5);

        for (const auto& traj : trajectories) {
            HPEN pen = CreatePen(PS_SOLID, 2, traj.color);
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);

            double angleRad = traj.angle * M_PI / 180.0;
            double vx = traj.velocity * cos(angleRad);
            double vy = traj.velocity * sin(angleRad);
            double t = 0.0;
            bool first = true;

            while (true) {
                double x = vx * t;
                double y = traj.height + vy * t - 0.5 * GRAVITY * t * t;
                if (y < 0) break;

                int screenX = static_cast<int>(xOffset + x * usedScaleX);
                int screenY = static_cast<int>(screenHeight - yOffset - y * usedScaleY);

                if (first) {
                    MoveToEx(hdc, screenX, screenY, NULL);
                    first = false;
                }
                else {
                    LineTo(hdc, screenX, screenY);
                }

                t += 0.01;
            }

            SelectObject(hdc, oldPen);
            DeleteObject(pen);
        }
     
        SelectObject(hdc, oldPen);
        DeleteObject(gridPen);
        DeleteObject(axisPen);
        char info[256];
        sprintf(info, "Current: V=%.1f m/s, a=%.1f°, H=%.1f m",
            trajectories.empty() ? 0 : trajectories.back().velocity,
            trajectories.empty() ? 0 : trajectories.back().angle,
            trajectories.empty() ? 0 : trajectories.back().height);
        TextOutA(hdc, 500, 10, info, strlen(info));
    }

    LRESULT CALLBACK SimulationWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        SimulationWindow* window = reinterpret_cast<SimulationWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (uMsg) {
        case WM_SETFOCUS:
            
            SetFocus(hwnd);
            return 0;

        case WM_COMMAND: {
            if (HIWORD(wParam) == BN_CLICKED) {
                
                if ((HWND)lParam == window->hCheckboxFixAngle ||
                    (HWND)lParam == window->hCheckboxFixHeight ||
                    (HWND)lParam == window->hCheckboxFixVelocity) {


                    window->angleFixed = SendMessage(window->hCheckboxFixAngle, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    window->heightFixed = SendMessage(window->hCheckboxFixHeight, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    window->velocityFixed = SendMessage(window->hCheckboxFixVelocity, BM_GETCHECK, 0, 0) == BST_CHECKED;


                    if (!window->trajectories.empty()) {
                        const auto& last = window->trajectories.back();
                        if (window->angleFixed) window->fixedAngleValue = last.angle;
                        if (window->heightFixed) window->fixedHeightValue = last.height;
                        if (window->velocityFixed) window->fixedVelocityValue = last.velocity;
                    }


                    SetFocus(hwnd);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (window) window->drawTrajectory(hdc);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_KEYDOWN:
            if (window) {
                switch (wParam) {
                case 'P':
                case 'p':
                    window->setScale(window->scaleX * 1.2, window->scaleY * 1.2);
                    break;
                case 'M':
                case 'm':
                    window->setScale(window->scaleX / 1.2, window->scaleY / 1.2);
                    break;
                case 'R': 
                    window->setScale(0, 0);
                    break;
                case 'A':
                case 'a':
 
                    if (!window->trajectories.empty()) {
                        const auto& last = window->trajectories.back();
                        window->addTrajectory(
                            last.velocity * 1.1,  
                            last.angle + 5,      
                            last.height + 2,     
                            0                    
                        );
                    }
                    break;
                case 'S':
                case 's':
                    window->saveToJpg();
                    return 0;
                }
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }
}

#define _CRT_SECURE_NO_WARNINGS
#define GRAVITY 9.81
#include "plot.h"

double initialVelocity = 0.0, angle = 0.0, initialHeight = 0.0;

namespace Simulation {

    SimulationWindow::SimulationWindow(HINSTANCE hInst, LPCWSTR className, LPCWSTR title)
        : hInstance(hInst), className(className), windowTitle(title), hwnd(nullptr), scaleX(0.0), scaleY(0.0) {
    }

    bool SimulationWindow::create() {
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = className;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        if (!RegisterClass(&wc)) return false;

       
        hwnd = CreateWindow(className, windowTitle, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 1000, 800, NULL, NULL, hInstance, NULL);

        if (!hwnd) return false;

        createControls(hwnd);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        return true;
    }

    void SimulationWindow::show(int nCmdShow) {
        ShowWindow(hwnd, nCmdShow);
    }

    void SimulationWindow::calculateTrajectoryParams(TrajectoryParams& traj) {
        double angleRad = traj.angle * M_PI / 180.0;
        double vx = traj.velocity * cos(angleRad);
        double vy = traj.velocity * sin(angleRad);

        
        double t_flight = (vy + sqrt(vy * vy + 2 * GRAVITY * traj.height)) / GRAVITY;

        
        traj.maxDistance = vx * t_flight;

        
        if (vy > 0) {
            double t_max = vy / GRAVITY;
            traj.maxHeight = traj.height + vy * t_max - 0.5 * GRAVITY * t_max * t_max;
        }
        else {
            traj.maxHeight = traj.height;
        }
    }

    void SimulationWindow::saveToJpg() {
       
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

        
        CreateWindowA("static", "Angle (deg):", WS_VISIBLE | WS_CHILD, 10, 10, 80, 20, hWnd, NULL, hInstance, NULL);
        hEditAngle = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            100, 10, 50, 20, hWnd, NULL, hInstance, NULL);

        CreateWindowA("static", "Height (m):", WS_VISIBLE | WS_CHILD, 160, 10, 80, 20, hWnd, NULL, hInstance, NULL);
        hEditHeight = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            250, 10, 50, 20, hWnd, NULL, hInstance, NULL);

        CreateWindowA("static", "Velocity (m/s):", WS_VISIBLE | WS_CHILD, 310, 10, 90, 20, hWnd, NULL, hInstance, NULL);
        hEditVelocity = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            410, 10, 50, 20, hWnd, NULL, hInstance, NULL);

        hBtnAddTrajectory = CreateWindowA("button", "Add Trajectory",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            470, 10, 120, 20, hWnd, (HMENU)1, hInstance, NULL);

        hInfoListBox = CreateWindowW(
            L"LISTBOX",
            L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_HASSTRINGS,
            50, 600, 900, 150,
            hWnd,
            NULL,
            hInstance,
            NULL
        );

        
        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Courier New");
        SendMessage(hInfoListBox, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
    void SimulationWindow::addTrajectory(double velocity, double angle, double height, COLORREF color) {
        if (!hwnd) return;

        if (color == 0) {
            color = availableColors[trajectories.size() % availableColors.size()];
        }

        double finalAngle = angleFixed ? fixedAngleValue : angle;
        double finalHeight = heightFixed ? fixedHeightValue : height;
        double finalVelocity = velocityFixed ? fixedVelocityValue : velocity;

        TrajectoryParams newTraj{ finalVelocity, finalAngle, finalHeight, color, 0.0, 0.0 };
        calculateTrajectoryParams(newTraj); 

        trajectories.push_back(newTraj);

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
        const int screenWidth = 1000;
        const int screenHeight = 800;
        const int xOffset = 100;
        const int yOffset = 100;
        const int infoAreaHeight = 150;

        double maxX = 0.0, maxY = 0.0;
        for (const auto& traj : trajectories) {
            if (traj.maxDistance > maxX) maxX = traj.maxDistance;
            if (traj.maxHeight > maxY) maxY = traj.maxHeight;
        }

        maxX = maxX * 1.1;
        maxY = maxY * 1.1;

        double usedScaleX = (scaleX > 0) ? scaleX : (screenWidth - 2 * xOffset) / maxX;
        double usedScaleY = (scaleY > 0) ? scaleY : (screenHeight - 2 * yOffset - infoAreaHeight) / maxY;

        HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
        RECT rect = { 0, 0, screenWidth, screenHeight };
        FillRect(hdc, &rect, bgBrush);
        DeleteObject(bgBrush);

        HPEN gridPen = CreatePen(PS_DOT, 1, RGB(200, 200, 200));
        HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

        for (double x = 0; x <= maxX; x += maxX / 10) {
            int screenX = xOffset + static_cast<int>(x * usedScaleX);
            MoveToEx(hdc, screenX, yOffset, NULL);
            LineTo(hdc, screenX, screenHeight - yOffset - infoAreaHeight);
        }

        for (double y = 0; y <= maxY; y += maxY / 10) {
            int screenY = screenHeight - yOffset - infoAreaHeight - static_cast<int>(y * usedScaleY);
            MoveToEx(hdc, xOffset, screenY, NULL);
            LineTo(hdc, screenWidth - xOffset, screenY);
        }

        HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        SelectObject(hdc, axisPen);

        MoveToEx(hdc, xOffset, screenHeight - yOffset - infoAreaHeight, NULL);
        LineTo(hdc, screenWidth - xOffset, screenHeight - yOffset - infoAreaHeight);

        MoveToEx(hdc, xOffset, screenHeight - yOffset - infoAreaHeight, NULL);
        LineTo(hdc, xOffset, yOffset);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));

        for (double x = 0; x <= maxX; x += maxX / 10) {
            int screenX = xOffset + static_cast<int>(x * usedScaleX);
            std::string label = std::to_string(static_cast<int>(x));
            TextOutA(hdc, screenX - 10, screenHeight - yOffset - infoAreaHeight + 10, label.c_str(), label.length());
        }

        for (double y = 0; y <= maxY; y += maxY / 10) {
            int screenY = screenHeight - yOffset - infoAreaHeight - static_cast<int>(y * usedScaleY);
            std::string label = std::to_string(static_cast<int>(y));
            TextOutA(hdc, xOffset - 30, screenY - 8, label.c_str(), label.length());
        }

        TextOutA(hdc, screenWidth - xOffset + 10, screenHeight - yOffset - infoAreaHeight - 10, "X (m)", 5);
        TextOutA(hdc, xOffset - 20, yOffset - 20, "Y (m)", 5);

        for (size_t i = 0; i < trajectories.size(); ++i) {
            const auto& traj = trajectories[i];
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
                int screenY = static_cast<int>(screenHeight - yOffset - infoAreaHeight - y * usedScaleY);

                if (first) {
                    MoveToEx(hdc, screenX, screenY, NULL);
                    first = false;
                }
                else {
                    LineTo(hdc, screenX, screenY);
                }

                t += 0.01;
            }

            int screenLandingX = static_cast<int>(xOffset + traj.maxDistance * usedScaleX);
            int screenLandingY = screenHeight - yOffset - infoAreaHeight;

            char numText[10];
            sprintf(numText, "%d", (int)i + 1);
            TextOutA(hdc, screenLandingX + 5, screenLandingY - 20, numText, strlen(numText));

            SelectObject(hdc, oldPen);
            DeleteObject(pen);
        }

        SendMessage(hInfoListBox, LB_RESETCONTENT, 0, 0);
        for (size_t i = 0; i < trajectories.size(); ++i) {
            const auto& traj = trajectories[i];
            wchar_t infoText[256];
            swprintf(infoText, 256,
                L"%d: Distance = %.2f m, Max height = %.2f m (V0=%.1f m/s, angle=%.1f°, H0=%.1f m)",
                (int)i + 1, traj.maxDistance, traj.maxHeight,
                traj.velocity, traj.angle, traj.height);
            SendMessage(hInfoListBox, LB_ADDSTRING, 0, (LPARAM)infoText);
        }

        SelectObject(hdc, oldPen);
        DeleteObject(gridPen);
        DeleteObject(axisPen);
    }

    LRESULT CALLBACK SimulationWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        SimulationWindow* window = reinterpret_cast<SimulationWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (uMsg) {
        case WM_SETFOCUS:
            
            SetFocus(hwnd);
            return 0;

        case WM_COMMAND: {
            if (HIWORD(wParam) == BN_CLICKED) {
                
                if ((HWND)lParam == window->hBtnAddTrajectory) {
                    char angleText[32], heightText[32], velocityText[32];
                    GetWindowTextA(window->hEditAngle, angleText, 32);
                    GetWindowTextA(window->hEditHeight, heightText, 32);
                    GetWindowTextA(window->hEditVelocity, velocityText, 32);

                    double angle = atof(angleText);
                    double height = atof(heightText);
                    double velocity = atof(velocityText);

                    if (angle != 0 || height != 0 || velocity != 0) {
                        window->addTrajectory(velocity, angle, height, 0);
                    }
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
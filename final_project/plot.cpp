#define _CRT_SECURE_NO_WARNINGS
#include "plot.h"

double initialVelocity = 0.0, angle = 0.0, initialHeight = 0.0;

namespace Simulation {

    LRESULT CALLBACK SimulationWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        SimulationWindow* window = reinterpret_cast<SimulationWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        switch (uMsg) {

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (window) {
                window->drawTrajectory(hdc, initialVelocity, angle, initialHeight);
            }
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    SimulationWindow::SimulationWindow(HINSTANCE hInst, LPCWSTR className, LPCWSTR title)
        : hInstance(hInst), className(className), windowTitle(title), hwnd(nullptr) {
    }

    bool SimulationWindow::create() {
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = className;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        if (!RegisterClass(&wc)) {
            return false;
        }

        hwnd = CreateWindow(className, windowTitle,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
            NULL, NULL, hInstance, NULL);

        if (!hwnd) {
            return false;
        }



        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));


        return true;
    }

    void SimulationWindow::show(int nCmdShow) {
        ShowWindow(hwnd, nCmdShow);

    }

    void SimulationWindow::drawTrajectory(HDC hdc, double initialVelocity, double angle, double initialHeight) {
        const int screenWidth = 800;
        const int screenHeight = 600;
        const int xOffset = 70;
        const int yOffset = 70;


        double angleRad = angle * M_PI / 180.0;
        double vx = initialVelocity * cos(angleRad);
        double vy = initialVelocity * sin(angleRad);

        double t_flight;
        if (angle == 0 && initialHeight > 0)
            t_flight = sqrt(2 * initialHeight / GRAVITY);
        else
            t_flight = (vy + sqrt(vy * vy + 2 * GRAVITY * initialHeight)) / GRAVITY;

        double maxX = vx * t_flight;
        double maxY = initialHeight + (vy * vy) / (2 * GRAVITY);

        double scaleX = (screenWidth - 2 * xOffset) / maxX;
        double scaleY = (screenHeight - 2 * yOffset) / maxY;


        HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        HPEN oldPen = (HPEN)SelectObject(hdc, axisPen);

        MoveToEx(hdc, xOffset, screenHeight - yOffset, NULL);
        LineTo(hdc, screenWidth - xOffset, screenHeight - yOffset); 

        MoveToEx(hdc, xOffset, screenHeight - yOffset, NULL);
        LineTo(hdc, xOffset, yOffset); 


        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);

        HFONT font = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, TEXT("Arial"));
        SelectObject(hdc, font);


        for (int i = 0; i <= 10; ++i) {
            double value = maxX * i / 10.0;
            int x = static_cast<int>(xOffset + value * scaleX);
            MoveToEx(hdc, x, screenHeight - yOffset - 5, NULL);
            LineTo(hdc, x, screenHeight - yOffset + 5);

            char label[32];
            sprintf(label, "%.0f", value);
            TextOutA(hdc, x - 10, screenHeight - yOffset + 8, label, strlen(label));
        }


        for (int i = 0; i <= 10; ++i) {
            double value = maxY * i / 10.0;
            int y = static_cast<int>(screenHeight - yOffset - value * scaleY);
            MoveToEx(hdc, xOffset - 5, y, NULL);
            LineTo(hdc, xOffset + 5, y);

            char label[32];
            sprintf(label, "%.0f", value);
            TextOutA(hdc, xOffset - 35, y - 7, label, strlen(label));
        }

        DeleteObject(font);
        SelectObject(hdc, oldPen);
        DeleteObject(axisPen);


        HPEN trajectoryPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        SelectObject(hdc, trajectoryPen);

        double t = 0.0;
        bool firstPoint = true;
        POINT prevPoint = {};

        while (true) {
            double x = vx * t;
            double y = initialHeight + vy * t - 0.5 * GRAVITY * t * t;
            if (y < 0) break;

            int screenX = static_cast<int>(xOffset + x * scaleX);
            int screenY = static_cast<int>(screenHeight - yOffset - y * scaleY);

            if (firstPoint) {
                MoveToEx(hdc, screenX, screenY, NULL);
                firstPoint = false;
            }
            else {
                LineTo(hdc, screenX, screenY);
            }

            t += 0.01;
        }

        SelectObject(hdc, oldPen);
        DeleteObject(trajectoryPen);


        char CalcMsg[200];
        sprintf(CalcMsg, "FlightLength = %.2f\nFlightHeight = %.2f", maxX, maxY);
        MessageBoxA(NULL, CalcMsg, "Calculations", MB_ICONINFORMATION);
    }

} 
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

    void SimulationWindow::drawTrajectory(HDC hdc, double initialVelocity, double angle, double inititalHeight) {

     
        const int screenWidth = 800;  
        const int screenHeight = 600; 
        const int xOffset = 50;      
        const int yOffset = 50;      


        const int axisLengthX = screenWidth - 2 * xOffset;
        const int axisLengthY = screenHeight - 2 * yOffset;



        
        HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        HPEN oldPen = (HPEN)SelectObject(hdc, axisPen);

        
        MoveToEx(hdc, xOffset, screenHeight - yOffset, NULL);
        LineTo(hdc, screenWidth - xOffset, screenHeight - yOffset);

       
        MoveToEx(hdc, xOffset, screenHeight - yOffset, NULL);
        LineTo(hdc, xOffset, yOffset);

        SelectObject(hdc, oldPen);
        DeleteObject(axisPen);

        
        HPEN trajectoryPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        SelectObject(hdc, trajectoryPen);

        double t = 0.0, x = 0.0, y = 0.0;
        while (true) {
            t += 0.01;
            x = initialVelocity * t * cos(angle * M_PI / 180.0);
            y = initialHeight + (initialVelocity * t * sin(angle * M_PI / 180.0) - 0.5 * GRAVITY * t * t);


            if (y < 0) break;


            int screenX = static_cast<int>(xOffset + x);
            int screenY = static_cast<int>(screenHeight - yOffset - y);

            if (screenX > screenWidth - xOffset || screenY < yOffset) break;


            SetPixel(hdc, screenX, screenY, RGB(255, 0, 0));
        }

        double FlightLength = ((initialVelocity * initialVelocity) * sin(2 * angle * M_PI / 180)) / GRAVITY;
        double FlightHeight = initialHeight + ((initialVelocity * initialVelocity) * sin(angle * M_PI / 180) * sin(angle * M_PI / 180)) / (2 * GRAVITY);

        char CalcMsg[200];
        sprintf(CalcMsg, "FlightLength=%.2f,\nFlightHeight=%.2f", FlightLength, FlightHeight);
        MessageBoxA(NULL, CalcMsg, "Calculations", MB_ICONINFORMATION);

        SelectObject(hdc, oldPen);
        DeleteObject(trajectoryPen);
    }
} 
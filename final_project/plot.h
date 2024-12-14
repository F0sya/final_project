#ifndef SIMULATION_WINDOW_H
#define SIMULATION_WINDOW_H

#include "lib.h"

extern double initialVelocity;
extern double angle;
extern double initialHeight;

namespace Simulation {

    class SimulationWindow {
    private:
        HINSTANCE hInstance;
        HWND hwnd;
        LPCWSTR className;
        LPCWSTR windowTitle;
        static constexpr double GRAVITY = 9.81;

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void drawTrajectory(HDC hdc, double initialVelocity, double angle, double initialHeight);

    public:

        SimulationWindow(HINSTANCE hInst, LPCWSTR className, LPCWSTR title);
        bool create();
        void show(int nCmdShow);


    };

} 

#endif // SIMULATION_WINDOW_H
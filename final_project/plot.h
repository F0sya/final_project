#ifndef SIMULATION_WINDOW_H
#define SIMULATION_WINDOW_H

#include "lib.h"
#include "jpgSaver.h"

extern double initialVelocity;
extern double angle;
extern double initialHeight;

namespace Simulation {

    struct TrajectoryParams {
        double velocity;
        double angle;
        double height;
        COLORREF color;
    };

    class SimulationWindow {
    private:
        HINSTANCE hInstance;
        HWND hwnd;
        LPCWSTR className;
        LPCWSTR windowTitle;
        std::vector<TrajectoryParams> trajectories;
        double scaleX;
        double scaleY;
        HWND hCheckboxFixAngle;
        HWND hCheckboxFixHeight;
        HWND hCheckboxFixVelocity;
        bool angleFixed = false;
        bool heightFixed = false;
        bool velocityFixed = false;
        double fixedAngleValue = 0;
        double fixedHeightValue = 0;
        double fixedVelocityValue = 0;
        std::vector<COLORREF> availableColors = {
            RGB(255, 0, 0),   
            RGB(0, 150, 0),  
            RGB(0, 0, 255),   
            RGB(255, 0, 255), 
            RGB(255, 165, 0)  
        };

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void drawTrajectory(HDC hdc);


    public:
        SimulationWindow(HINSTANCE hInst, LPCWSTR className, LPCWSTR title);
        bool create();
        void show(int nCmdShow);
        void addTrajectory(double velocity, double angle, double height, COLORREF color);
        void setScale(double xScale, double yScale);
        void createControls(HWND hWnd);
        void saveToJpg();
    };

}

#endif // SIMULATION_WINDOW_H
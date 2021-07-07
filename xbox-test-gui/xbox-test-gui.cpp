// xbox-test-gui.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "xbox-test-gui.h"

#include "image-loading.hpp"
#include "BitmapDrawer.h"
#include "controller.hpp"

#include <thread>
#include <chrono>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#define MAX_LOADSTRING 100

// Global Variables:
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

std::unique_ptr<BitmapDrawer> drawer;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE);
void ControlApp(std::unique_ptr<BitmapDrawer> drawer);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // TODO: Place code here.

    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
        return FALSE;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_XBOXTESTGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance))
        return FALSE;

    std::thread thread(ControlApp, std::move(drawer));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = DefWindowProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XBOXTESTGUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_XBOXTESTGUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance)
{
   HWND hWnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT, szWindowClass, szTitle, WS_POPUP,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
       return FALSE;

   HBITMAP hBitmap = LoadSplashImage(IDB_PNG2);
   drawer = std::make_unique<BitmapDrawer>(hWnd, hBitmap);
   drawer->Update();
   drawer->Hide();

   return TRUE;
}


void ControlApp(std::unique_ptr<BitmapDrawer> drawer)
{
    drawer->Show();

    const float durationSeconds = 10;
    const int fps = 144;

    const int numFrames = durationSeconds * fps;
    const auto frameDuration = std::chrono::microseconds(1000 * 1000 / fps);
    const int alphaPerFrame = 256 / numFrames;

    for (int frame = 1; frame <= numFrames; ++frame)
    {
        float alpha = 256 - frame * 256.f / numFrames;
        BYTE alphaByte = std::min(256, std::max(0, int(alpha)));

        drawer->Update(alphaByte);
        DwmFlush();
    }

    drawer->Hide();

    while (true)
    {
        if (GetBatteryLevel(0) != BatteryLevel::UNKNOWN)
        {
            drawer->Show();
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            drawer->Hide();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}
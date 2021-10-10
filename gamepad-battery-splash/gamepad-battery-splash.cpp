// gamepad-battery-splash.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "gamepad-battery-splash.h"

#include "image-loading.hpp"
#include "BitmapDrawer.h"
#include "controller.hpp"
#include "Animator.h"

#define MAX_LOADSTRING 100

// Global Variables:
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

const WCHAR mutexName[] = L"gamepad-battery-gui-mutex1337";

std::unique_ptr<BitmapDrawer> drawer;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE);
bool IsAppAlreadyRunning();
void ControlApp(std::unique_ptr<BitmapDrawer> drawer);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    if (IsAppAlreadyRunning())
        return 0;

    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
        return 0;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_XBOXTESTGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance))
        return 0;

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

   HBITMAP hBitmap = LoadSplashImage(IDB_BATTERY);
   drawer = std::make_unique<BitmapDrawer>(hWnd, hBitmap);
   drawer->Update();
   drawer->Hide();

   return TRUE;
}


bool IsAppAlreadyRunning()
{
    return CreateMutex(NULL, TRUE, mutexName) == NULL || GetLastError() == ERROR_ALREADY_EXISTS;
}


void ControlApp(std::unique_ptr<BitmapDrawer> drawer)
{
    FadeAnimation fade;

    while (true)
    {
        for (int playerID = 0; playerID < 4; ++playerID)
        {
            auto batteryLevel = GetBatteryLevel(playerID);
            if (batteryLevel == BatteryLevel::LOW || batteryLevel == BatteryLevel::EMPTY)
            {
                drawer->Show();
                std::this_thread::sleep_for(std::chrono::seconds(5));
                Animator(1500).Animate(*drawer, fade);
                drawer->Hide();
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::minutes(5));
    }
}
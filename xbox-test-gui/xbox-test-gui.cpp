// xbox-test-gui.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "xbox-test-gui.h"

#include "image-loading.hpp"
#include "controller.hpp"

#include <thread>
#include <chrono>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND my_hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE);
void ControlApp(HWND hWnd);


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

    std::thread thread(ControlApp, my_hWnd);

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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT, szWindowClass, szTitle, WS_POPUP,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
       return FALSE;

   HBITMAP bitmap = LoadSplashImage(IDB_PNG2);
   SetSplashImage(hWnd, bitmap);
   ShowWindow(hWnd, SW_HIDE);

   my_hWnd = hWnd;

   return TRUE;
}


void ControlApp(HWND hWnd)
{
    while (true)
    {
        if (GetBatteryLevel(0) != BatteryLevel::UNKNOWN)
        {
            ShowWindow(hWnd, SW_SHOW);
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            ShowWindow(hWnd, SW_HIDE);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}
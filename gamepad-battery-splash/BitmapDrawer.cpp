#include "BitmapDrawer.h"

BitmapDrawer::BitmapDrawer(HWND window, HBITMAP hBitmap)
    : window(window)
    , hBitmap(hBitmap)
{
    hdcScreen = GetDC(nullptr);
    hdcMem = CreateCompatibleDC(hdcScreen);

    SetBitmapParams();
    SetBlendStruct();
}

BitmapDrawer::~BitmapDrawer()
{
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

void BitmapDrawer::SetBitmapParams()
{
    // get bitmap size
    BITMAP bm;
    GetObject(hBitmap, sizeof(bm), &bm);
    bitmapSize = { bm.bmWidth, bm.bmHeight };

    // get primary monitor's info
    HMONITOR hmonPrimary = MonitorFromPoint(posZero, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorinfo = { 0 };
    monitorinfo.cbSize = sizeof(monitorinfo);
    GetMonitorInfo(hmonPrimary, &monitorinfo);

    // center bitmap at top of primary work area
    const RECT& rcWork = monitorinfo.rcWork;
    bitmapPos.x = rcWork.left + (rcWork.right - rcWork.left - bitmapSize.cx) / 2;
    bitmapPos.y = rcWork.top + 10;
}

void BitmapDrawer::SetBlendStruct()
{
    blendStruct = { 0 };
    blendStruct.BlendOp = AC_SRC_OVER;
    blendStruct.SourceConstantAlpha = 255;
    blendStruct.AlphaFormat = AC_SRC_ALPHA;
}

void BitmapDrawer::Show()
{
    ShowWindow(window, SW_SHOW);
    Update();
}

void BitmapDrawer::Hide()
{
    ShowWindow(window, SW_HIDE);
}

void BitmapDrawer::Update(BYTE alpha)
{
    blendStruct.SourceConstantAlpha = alpha;

    HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
    UpdateLayeredWindow(window, hdcScreen, &bitmapPos, &bitmapSize,
        hdcMem, &posZero, RGB(0, 0, 0), &blendStruct, ULW_ALPHA);
    SelectObject(hdcMem, hbmpOld);
}
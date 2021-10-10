#pragma once

#include "framework.h"

class BitmapDrawer
{
    HWND window;
	HBITMAP hBitmap;
	HDC hdcScreen;
	HDC hdcMem;
	BLENDFUNCTION blendStruct;
    SIZE bitmapSize;

	POINT posZero = { 0 };
    POINT bitmapPos;

    void SetBitmapParams();
    void SetBlendStruct();

public:
    BitmapDrawer(HWND window, HBITMAP hBitmap);
    ~BitmapDrawer();

    void Show();
    void Hide();
    void Update(BYTE alpha = 255);
};


#pragma once

#include <Windows.h>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

// Creates a stream object initialized with the data from an executable resource
IStream* CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
    IStream* ipStream = nullptr;

    // Find resource
    HRSRC hrsrc = FindResource(nullptr, lpName, lpType);
    if (hrsrc == nullptr)
        return ipStream;

    // Load resource
    DWORD dwResourceSize = SizeofResource(nullptr, hrsrc);
    HGLOBAL hglbImage = LoadResource(nullptr, hrsrc);
    if (hglbImage == nullptr)
        return ipStream;

    // Lock resource
    LPVOID pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == nullptr)
        return ipStream;

    // Allocate global memory to hold resource data
    HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == nullptr)
        return ipStream;

    // Lock global memory
    LPVOID pvResourceData = GlobalLock(hgblResourceData);
    if (pvResourceData == nullptr)
    {
        GlobalFree(hgblResourceData);
        return ipStream;
    }

    // Copy resource to global memory
    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
    GlobalUnlock(hgblResourceData);

    // Create stream from global memory
    if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
        return ipStream;

    // Clean up
    GlobalFree(hgblResourceData);
    return ipStream;
}

// Loads PNG image from specified stream via WIC
IWICBitmapSource* LoadBitmapFromStream(IStream* ipImageStream)
{
    IWICBitmapSource* ipBitmap = nullptr;

    // Load WIC PNG decoder
    IWICBitmapDecoder* ipDecoder = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICPngDecoder, nullptr, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder))))
        return ipBitmap;

    // Load PNG
    if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad)))
    {
        ipDecoder->Release();
        return ipBitmap;
    }

    // Check number of image frames
    UINT nFrameCount = 0;
    if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount != 1)
    {
        ipDecoder->Release();
        return ipBitmap;
    }

    // Load image
    IWICBitmapFrameDecode* ipFrame = nullptr;
    if (FAILED(ipDecoder->GetFrame(0, &ipFrame)))
    {
        ipDecoder->Release();
        return ipBitmap;
    }

    // Convert image to BGRA32 with pre-multiplied alpha
    WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
    ipFrame->Release();

    ipDecoder->Release();
    return ipBitmap;
}

// Creates a 32-bit DIB from the specified WIC bitmap
HBITMAP CreateHBITMAP(IWICBitmapSource* ipBitmap)
{
    HBITMAP hbmp = nullptr;

    UINT width = 0;
    UINT height = 0;
    if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)
        return hbmp;

    // Prepare bitmap info
    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -((LONG)height); // negative -> top down DIB
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;

    // Create HBITMAP for image
    void* pvImageBits = nullptr;
    HDC hdcScreen = GetDC(nullptr);
    hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, nullptr, 0);
    ReleaseDC(nullptr, hdcScreen);
    if (hbmp == nullptr)
        return hbmp;

    // Copy into HBITMAP
    const UINT cbStride = width * 4;
    const UINT cbImage = cbStride * height;
    if (FAILED(ipBitmap->CopyPixels(nullptr, cbStride, cbImage, static_cast<BYTE*>(pvImageBits))))
    {
        DeleteObject(hbmp);
        hbmp = nullptr;
    }

    return hbmp;
}

// Loads PNG into HBITMAP
HBITMAP LoadSplashImage(int pngID)
{
    HBITMAP hbmpSplash = nullptr;

    // Load image data into stream
    IStream* ipImageStream = CreateStreamOnResource(MAKEINTRESOURCE(pngID), _T("PNG"));
    if (ipImageStream == nullptr)
        return hbmpSplash;

    // Load bitmap with WIC
    IWICBitmapSource* ipBitmap = LoadBitmapFromStream(ipImageStream);
    if (ipBitmap == nullptr)
    {
        ipImageStream->Release();
        return hbmpSplash;
    }

    hbmpSplash = CreateHBITMAP(ipBitmap);

    // Clean up
    ipBitmap->Release();
    ipImageStream->Release();

    return hbmpSplash;
}

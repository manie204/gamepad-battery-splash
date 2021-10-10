#include "Animator.h"

#include "framework.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

Animator::Animator(int durationMilliSecs)
{
    DEVMODEA deviceInfo;
    EnumDisplaySettingsA(nullptr, ENUM_CURRENT_SETTINGS, &deviceInfo);
    numFrames = durationMilliSecs * deviceInfo.dmDisplayFrequency / 1000;
}

void Animator::Animate(BitmapDrawer & drawer, IAnimation & animation)
{
    for (int frame = 0; frame < numFrames; ++frame)
    {
        animation(drawer, frame, numFrames);
        DwmFlush();
    }
}

void FadeAnimation::operator()(BitmapDrawer& drawer, int frame, int numFrames) const
{
	float alpha = 256 - (frame + 1) * 256.f / numFrames;
	BYTE alphaByte = std::min(256, std::max(0, int(alpha)));

	drawer.Update(alphaByte);
}

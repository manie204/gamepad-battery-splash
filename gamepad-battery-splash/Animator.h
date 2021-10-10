#pragma once

#include "BitmapDrawer.h"

struct IAnimation
{
	virtual void operator()(BitmapDrawer& drawer, int frame, int numFrames) const = 0;
};

class Animator
{
	int numFrames = 0;

public:
	Animator(int durationMilliSecs);
	void Animate(BitmapDrawer& drawer, IAnimation & animation);
};

struct FadeAnimation : IAnimation
{
	virtual void operator()(BitmapDrawer& drawer, int frame, int numFrames) const;
};
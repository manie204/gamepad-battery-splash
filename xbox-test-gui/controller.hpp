#pragma once

#include <Xinput.h>
#pragma comment(lib, "xinput1_4.dll")

enum class BatteryLevel
{
	EMPTY, LOW, MEDIUM, FULL, UNKNOWN
};

BatteryLevel TranslateBatteryLevel(BYTE level)
{
	switch (level)
	{
	case BATTERY_LEVEL_EMPTY:
		return BatteryLevel::EMPTY;
	case BATTERY_LEVEL_LOW:
		return BatteryLevel::LOW;
	case BATTERY_LEVEL_MEDIUM:
		return BatteryLevel::MEDIUM;
	case BATTERY_LEVEL_FULL:
		return BatteryLevel::FULL;
	default:
		return BatteryLevel::UNKNOWN;
	}
}

BatteryLevel GetBatteryLevel(int playerNum)
{
	XINPUT_BATTERY_INFORMATION batteryInfo;
	auto success = XInputGetBatteryInformation(DWORD(playerNum), BATTERY_DEVTYPE_GAMEPAD, &batteryInfo);
	if (success == 0)
		return TranslateBatteryLevel(batteryInfo.BatteryLevel);
	else
		return BatteryLevel::UNKNOWN;
}
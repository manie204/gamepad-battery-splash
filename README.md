# gamepad-battery-splash
Displays a little splash-screen overlay on the top of the screen when a wireless gamepad reaches low battery levels.

![splash-animation](https://github.com/manie204/gamepad-battery-splash/blob/master/splash-animation.gif)

## How to Use

There is no installation/configuration. Just run the program.
It will be sitting idly in the background and checks battery levels once every few minutes.
There can only be one instance running at the same time.

I suggest adding the app to autostart to always have it running.

## Current Limitations

I would like to work on these in the future. However, I don't have much time currently.

* Splash overlay does not show gamepad ID (player number)
* There are no options regarding the splash overlay. When the battery level becomes low, it displays once every 5 minutes for about 5 seconds.
* Only displays correctly on top of apps that do *not* run in exclusive full-screen mode
* No multi-monitor support
* No option for notifications (toast noficiations, sounds, etc.)
* Only works on Windows

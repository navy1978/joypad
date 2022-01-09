# joypad
Reads joypad events and displays them.

This code has been taken from  https://gist.github.com/jasonwhite/c5b2048c15993d285130 (Author: Jason White).
This version fixes the DPAD tracking (pressed and released) missed in the original project
There is also an attempt to listen to the events in background using pthread .

To compile:

gcc -pthread joypad.c -o joypad

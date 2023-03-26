#include "functions.hpp"
#include <iostream>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <string>

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <config-path>" << std::endl;
		return 1;
	}

	std::string config_path(argv[1]);

	WinWM wm(config_path);
	wm.run();

	Display* display = XOpenDisplay(nullptr);
	if (display == nullptr) {
		std::cerr << "Failed to open X display." << std::endl;
		return 1;
	}

	Window root_window = XDefaultRootWindow(display);

	// Grab the Ctrl+Enter key combination
	KeyCode ctrl_keycode = XKeysymToKeycode(display, XK_Control_L);
	KeyCode enter_keycode = XKeysymToKeycode(display, XK_Return);

	XGrabKey(display, ctrl_keycode, ControlMask, root_window, true, GrabModeAsync, GrabModeAsync);

	XEvent event;
	while (true) {
		XNextEvent(display, &event);

		if (event.type == KeyPress) {
			if (event.xkey.keycode == enter_keycode && event.xkey.state & ControlMask) {
				system("x-terminal-emulator");
			}
		}
	}

	XCloseDisplay(display);

	return 0;
}

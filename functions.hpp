#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
using json = nlohmann::json;

class WindowAttributes {
public:
	int x, y, width, height, border_width, depth;
};

class WinWM {
public:
	WinWM(const std::string& config_path);
	void run();
	void set_key_binding(const std::string& key, unsigned int modifiers);
	void set_mouse_binding(unsigned int button, unsigned int modifiers);
	void set_move_step(unsigned int step);
	void set_resize_step(unsigned int step);
private:
	void read_config(const std::string& config_path);
	Display* display_;
	Window root_;
	std::unordered_map<std::string, KeyCode> key_binds_;
	unsigned int mouse_button_;
	unsigned int mouse_modifiers_;
	unsigned int move_step_;
	unsigned int resize_step_;
	WindowAttributes get_window_attributes(Window window);
};


void WinWM::read_config(const std::string& config_path) {
	std::ifstream config_file(config_path);
	if (!config_file) {
		std::cerr << "Failed to open config file: " << config_path << std::endl;
		std::exit(1);
	}

	json config;
	config_file >> config;

	if (config.contains("mouse_button")) {
		mouse_button_ = config["mouse_button"];
	}
	if (config.contains("mouse_modifiers")) {
		mouse_modifiers_ = config["mouse_modifiers"];
	}
	if (config.contains("move_step")) {
		move_step_ = config["move_step"];
	}
	if (config.contains("resize_step")) {
		resize_step_ = config["resize_step"];
	}

	if (config.contains("key_binds")) {
		for (const auto& [key, value] : config["key_binds"].items()) {
			set_key_binding(key, value);
		}
	}
}

WinWM::WinWM(const std::string& config_path) : display_{XOpenDisplay(0)}, root_{DefaultRootWindow(display_)},
	mouse_button_{1}, mouse_modifiers_{Mod1Mask}, move_step_{10}, resize_step_{10}
{
	if (!display_) {
		std::cerr << "Failed to open display." << std::endl;
		std::exit(1);
	}

	read_config(config_path);
}

void WinWM::run()
{
	XGrabButton(display_, mouse_button_, mouse_modifiers_, root_, True,
			ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);

	for (auto& pair : key_binds_) {
		XGrabKey(display_, pair.second, mouse_modifiers_, root_,
				True, GrabModeAsync, GrabModeAsync);
	}

	for (;;) {
		XEvent event;
		XNextEvent(display_, &event);

		switch (event.type) {
			case KeyPress: {
				auto keycode = event.xkey.keycode;
				auto key = XKeysymToString(XkbKeycodeToKeysym(display_, keycode, 0, 0));
				auto subwindow = event.xkey.subwindow;
				if (key_binds_.find(key) != key_binds_.end() && subwindow != None) {
					XRaiseWindow(display_, subwindow);
				}
				break;
			}

			case ButtonPress: {
				auto button = event.xbutton.button;
				if (button == mouse_button_) {
					auto subwindow = event.xbutton.subwindow;
					if (subwindow != None) {
						XGrabPointer(display_, subwindow, True,
								PointerMotionMask | ButtonReleaseMask,
								GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
						auto attributes = get_window_attributes(subwindow);
						event.xbutton.x -= attributes.x;
						event.xbutton.y -= attributes.y;
						XWindowAttributes win_attr;
						XGetWindowAttributes(display_, subwindow, &win_attr);
						attributes.x = win_attr.x;
						attributes.y = win_attr.y;
					}
				}
				break;
			}

			case MotionNotify: {
				XEvent next_event;
				int xdiff, ydiff;
				while (XCheckTypedEvent(display_, MotionNotify, &next_event));
				auto subwindow = event.xmotion.window;
				auto attributes = get_window_attributes(subwindow);
				xdiff = event.xbutton.x_root - event.xmotion.x;
				ydiff = event.xbutton.y_root - event.xmotion.y;
				if (mouse_button_ == 1) {
					XMoveWindow(display_, subwindow, attributes.x + (xdiff / move_step_) * move_step_, attributes.y + (ydiff / move_step_) * move_step_);
				}
			break;
			}
			case ButtonRelease: {
				XUngrabPointer(display_, CurrentTime);
				break;
			}
			default:
			break;
		}
	}
}

void WinWM::set_key_binding(const std::string& key, unsigned int modifiers) {
    KeyCode keycode = XKeysymToKeycode(display_, XStringToKeysym(key.c_str()));
    if (keycode == NoSymbol) {
        std::cerr << "Failed to get keycode for key " << key << std::endl;
        return;
    }

    XGrabKey(display_, keycode, modifiers, root_, True, GrabModeAsync, GrabModeAsync);
    
    if (key == "Return" && modifiers == ControlMask) {
        // Launch terminal when CTRL+Enter is pressed
        std::system("xterm &");
    }

    key_binds_[key] = keycode;
}

void WinWM::set_mouse_binding(unsigned int button, unsigned int modifiers) {
	mouse_button_ = button;
	mouse_modifiers_ = modifiers;
}

void WinWM::set_move_step(unsigned int step) {
		move_step_ = step;
}

void WinWM::set_resize_step(unsigned int step) {
	resize_step_ = step;
}

WindowAttributes WinWM::get_window_attributes(Window window) {
	Window root;
	int x, y;
	unsigned int width, height, border_width, depth;
	XGetGeometry(display_, window, &root, &x, &y, &width, &height, &border_width, &depth);
	return {x, y, static_cast<int>(width), static_cast<int>(height), static_cast<int>(border_width), static_cast<int>(depth)};
}

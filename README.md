# WinWM

WinWM is a window manager based on TinyWM written in C++ that is inspired by the i3 window manager. It is a more feature-rich version of TinyWM and allows for moving, resizing, focusing, and raising windows. Additionally, WinWM can be configured via configuration files. I'm not planning on mantaining this, as this has extremely bad code that throws a segmentation fault when compiled and executed.

## Files

- winwm.cpp: the main code;
- functions.hpp: the header containing functions and classes;
- LICENSE: License file containing the license of this project;
- Makefile: build file:
  - Compile files with ```make all```
  - Build with ```make install```
  - Clean the build with ```make clean```

## Usage

WinWM provides several keybindings that allow you to interact with your windows. These include:

- Focus follows pointer.
- Alt+Button1, drag: interactive window move.
- Alt+Button3, drag: interactive window resize.
- Alt+F1: raise focused window.

To use WinWM, simply compile and run the program. By default, the configuration file is expected to be located at ~/.config/winwm/config.json. However, you can specify a custom configuration file path by providing it as an argument to the program.

## Configuration

WinWM can be configured via a JSON configuration file. The default configuration file is located at ~/.config/winwm/config.json. The configuration file contains the following options:

- `modkey`: The modifier key used in keybindings (e.g. "Alt").
- `move_resize_step`: The step size used when moving or resizing windows.
- `terminal`: The terminal command used to launch a new terminal window.
- `workspaces`: A list of workspaces and their associated keybindings.

Example configuration file:

```json
{
    "modkey": "Alt",
    "move_resize_step": 20,
    "terminal": "xterm",
    "workspaces": [
        {
        "name": "1",
        "key": "1"
        },
        {
        "name": "2",
        "key": "2"
        },
        {
        "name": "3",
        "key": "3"
        }
    ]
}
```

In this example, the modifier key is set to "Alt", the move/resize step is set to 20 pixels, the terminal command is set to "xterm", and there are three workspaces defined with their associated keybindings (Alt+1, Alt+2, and Alt+3).

## Misc

Enjoy using WinWM as a simple, lightweight window manager for your Linux desktop!

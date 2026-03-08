# Clickermann(MVP)

A macro recorder and playback tool for Windows, written in C++.  
Records keyboard and mouse input, saves it to a file, and replays it in a loop.

## How it works

The program installs low-level Windows hooks (`WH_KEYBOARD_LL`, `WH_MOUSE_LL`) to capture input events globally.  
Each event is timestamped and written to a binary buffer, which can be saved to disk and replayed with accurate timing.

## Hotkeys

| Hotkey   | Action                              |
|----------|-------------------------------------|
| Ctrl + R | Start recording                     |
| Ctrl + S | Stop recording and save             |
| Ctrl + A | Load last save and play in loop     |

> Ctrl + S also works during playback to stop the loop.

## Build

Requires **Visual Studio** with the MSVC toolchain (Windows only).

1. Open `Clickermann.slnx` in Visual Studio
2. Select `Release` configuration
3. Build → Run

No external dependencies.

## Project structure
```
main.cpp             — entry point, hooks, hotkey handling
RecordAction.cpp     — recording logic and file I/O
ActivateScenario.cpp — playback logic
Actions.h            — shared types, structs, globals, buffer helper
```

## Known limitations

- Playback loops indefinitely until stopped with Ctrl + S
- Scenario is saved to `autosave.scenario` in the working directory (hardcoded)
- Coordinates are recorded in absolute screen pixels — playback may behave differently at other screen resolutions
- The playback thread is detached (`std::thread::detach`), so abrupt exit during playback may cause issues

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <windows.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include <string>
#include "Actions.h"

std::vector<char> scenarioBuffer;
std::atomic<ProgramState> currentState{ ProgramState::Idle };

void startRecording();
void recordKeyboard(WORD vKey, bool isDown);
void recordMouse(long x, long y, MouseBtn btn, int action);
void playScenario();
void saveScenario(const std::string& filename);
void loadScenario(const std::string& filename);

constexpr UINT VK_R = 0x52;
constexpr UINT VK_S = 0x53;
constexpr UINT VK_A = 0x41;

constexpr int ACTION_MOVE = 2;

LRESULT CALLBACK handleLowLevelKeyboardEvent(int code, WPARAM wparam, LPARAM lparam) {
    if (code != HC_ACTION) return CallNextHookEx(NULL, code, wparam, lparam);

    KBDLLHOOKSTRUCT* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);
    bool ctrlPressed = GetAsyncKeyState(VK_CONTROL) & 0x8000;
    bool isDown      = (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN);

    if (ctrlPressed && (kbd->vkCode == VK_R || kbd->vkCode == VK_S || kbd->vkCode == VK_A)) {
        if (isDown) {
            if (kbd->vkCode == VK_R && currentState == ProgramState::Idle) {
                startRecording();
            }
            else if (kbd->vkCode == VK_S && currentState != ProgramState::Idle) {
                currentState = ProgramState::Idle;
                saveScenario("autosave.scenario");
                std::cout << "[SYSTEM] Stopped and Saved.\n";
            }
            else if (kbd->vkCode == VK_A && currentState == ProgramState::Idle) {
                loadScenario("autosave.scenario");
                if (!scenarioBuffer.empty()) {
                    std::thread(playScenario).detach(); 
                }
            }
        }
        return 1; 
    }

    if (currentState == ProgramState::Recording) {
        if (kbd->vkCode != VK_CONTROL) {
            recordKeyboard((WORD)kbd->vkCode, isDown);
        }
    }

    return CallNextHookEx(NULL, code, wparam, lparam);
}

LRESULT CALLBACK handleLowLevelMouseEvent(int code, WPARAM wparam, LPARAM lparam) {
    if (code == HC_ACTION && currentState == ProgramState::Recording) {
        MSLLHOOKSTRUCT* m = reinterpret_cast<MSLLHOOKSTRUCT*>(lparam);

        MouseBtn btn = MouseBtn::NONE;
        int action   = ACTION_MOVE;

        if      (wparam == WM_LBUTTONDOWN) { btn = MouseBtn::LEFT;   action = 0; }
        else if (wparam == WM_LBUTTONUP)   { btn = MouseBtn::LEFT;   action = 1; }
        else if (wparam == WM_RBUTTONDOWN) { btn = MouseBtn::RIGHT;  action = 0; }
        else if (wparam == WM_RBUTTONUP)   { btn = MouseBtn::RIGHT;  action = 1; }
        else if (wparam == WM_MBUTTONDOWN) { btn = MouseBtn::MIDDLE; action = 0; }
        else if (wparam == WM_MBUTTONUP)   { btn = MouseBtn::MIDDLE; action = 1; }

        if (btn != MouseBtn::NONE || action == ACTION_MOVE) {
            recordMouse(m->pt.x, m->pt.y, btn, action);
        }
    }

    return CallNextHookEx(NULL, code, wparam, lparam);
}

int main() {
    HHOOK kHook = SetWindowsHookExW(WH_KEYBOARD_LL, handleLowLevelKeyboardEvent, GetModuleHandle(NULL), 0);
    HHOOK mHook = SetWindowsHookExW(WH_MOUSE_LL,    handleLowLevelMouseEvent,    GetModuleHandle(NULL), 0);

    if (!kHook || !mHook) {
        std::cerr << "[ERROR] Failed to install hooks\n";
        return 1;
    }

    std::cout << "========================================\n";
    std::cout << "   CLICKERMANN CLONE (C++ Edition)      \n";
    std::cout << "========================================\n";
    std::cout << "Ctrl + R : Start Recording\n";
    std::cout << "Ctrl + S : Stop & Save\n";
    std::cout << "Ctrl + A : Play Scenario (Loop)\n";
    std::cout << "----------------------------------------\n";

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    UnhookWindowsHookEx(kHook);
    UnhookWindowsHookEx(mHook);
    return 0;
}
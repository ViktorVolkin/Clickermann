#include "Actions.h"
#include <chrono>
#include <iostream>

void playScenario() {
    if (scenarioBuffer.empty()) return;

    bool loop = true;

    do {
        currentState = ProgramState::Playing;
        size_t offset = 0;
        double lastTime = 0;

        while (offset < scenarioBuffer.size() && currentState == ProgramState::Playing) {
            double currentTime = *reinterpret_cast<double*>(&scenarioBuffer[offset]);
            offset += sizeof(double);

            double delay = currentTime - lastTime;
            if (delay > 0) {
                Sleep(static_cast<DWORD>(delay * 1000));
            }
            lastTime = currentTime;

            EventType type = static_cast<EventType>(scenarioBuffer[offset]);
            offset += sizeof(char);

            if (type == EventType::Keyboard) {
                RawKey rk = *reinterpret_cast<RawKey*>(&scenarioBuffer[offset]);
                offset += sizeof(RawKey);

                INPUT in = { 0 };
                in.type = INPUT_KEYBOARD;

                in.ki.wScan = (WORD)MapVirtualKey(rk.vKey, MAPVK_VK_TO_VSC);
                in.ki.wVk = 0;

                in.ki.dwFlags = KEYEVENTF_SCANCODE;
                if (!rk.down) in.ki.dwFlags |= KEYEVENTF_KEYUP;

                if (rk.vKey >= 0x21 && rk.vKey <= 0x2E) in.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;

                SendInput(1, &in, sizeof(INPUT));
            }

            else if (type == EventType::Mouse) {
                RawMouse rm = *reinterpret_cast<RawMouse*>(&scenarioBuffer[offset]);
                offset += sizeof(RawMouse);

                INPUT in = { 0 };
                in.type = INPUT_MOUSE;

                in.mi.dx = (rm.x * 65536) / GetSystemMetrics(SM_CXSCREEN);
                in.mi.dy = (rm.y * 65536) / GetSystemMetrics(SM_CYSCREEN);

                in.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

                if (rm.action != 2) { 
                    if (rm.btn == MouseBtn::LEFT) {
                        in.mi.dwFlags |= (rm.action == 0) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
                    }
                    else if (rm.btn == MouseBtn::RIGHT) {
                        in.mi.dwFlags |= (rm.action == 0) ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
                    }
                    else if (rm.btn == MouseBtn::MIDDLE) {
                        in.mi.dwFlags |= (rm.action == 0) ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
                    }
                }
                SendInput(1, &in, sizeof(INPUT));
            }
        }

        if (loop) Sleep(100); 

    } while (loop && currentState == ProgramState::Playing);


    WORD modifiers[] = { VK_CONTROL, VK_SHIFT, VK_MENU };
    for (WORD vk : modifiers) {
        INPUT release = { 0 };
        release.type = INPUT_KEYBOARD;
        release.ki.wScan = (WORD)MapVirtualKey(vk, MAPVK_VK_TO_VSC);
        release.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        SendInput(1, &release, sizeof(INPUT));
    }

    currentState = ProgramState::Idle;
    std::cout << "[PLAY] Finished and Cleanup done." << std::endl;
}

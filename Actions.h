#pragma once

#include <Windows.h>
#include <vector>
#include <atomic>
#include <string>


enum class EventType : char { Mouse = 1, Keyboard = 2 };
enum class MouseBtn  : int  { NONE = 0, LEFT = 1, RIGHT = 2, MIDDLE = 3 };
enum class ProgramState     { Idle, Recording, Playing };


struct RawMouse { long x, y; MouseBtn btn; int action; };
struct RawKey   { WORD vKey; bool down; };


extern std::vector<char>          scenarioBuffer;
extern std::atomic<ProgramState>  currentState;


template<typename T>
void pushToBuffer(const T& data) {
    const char* p = reinterpret_cast<const char*>(&data);
    scenarioBuffer.insert(scenarioBuffer.end(), p, p + sizeof(T));
}

void startRecording();
void recordKeyboard(WORD vKey, bool isDown);
void recordMouse(long x, long y, MouseBtn btn, int action);

void playScenario();

void saveScenario(const std::string& filename);
void loadScenario(const std::string& filename);
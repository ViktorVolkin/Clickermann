#include "Actions.h"
#include <chrono>
#include <iostream>
#include <fstream>

static std::chrono::steady_clock::time_point recordingStartTime;


void startRecording() {
    scenarioBuffer.clear();
    recordingStartTime = std::chrono::steady_clock::now();
    currentState = ProgramState::Recording;
    std::cout << "[REC] Started\n";
}

void recordKeyboard(WORD vKey, bool isDown) {
    auto now = std::chrono::steady_clock::now();
    double ts = std::chrono::duration<double>(now - recordingStartTime).count();

    pushToBuffer(ts);
    pushToBuffer(static_cast<char>(EventType::Keyboard));
    pushToBuffer(RawKey{ vKey, isDown });
}

void recordMouse(long x, long y, MouseBtn btn, int action) {
    auto now = std::chrono::steady_clock::now();
    double ts = std::chrono::duration<double>(now - recordingStartTime).count();

    pushToBuffer(ts);
    pushToBuffer(static_cast<char>(EventType::Mouse));
    pushToBuffer(RawMouse{ x, y, btn, action });
}


void saveScenario(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "[ERROR] Cannot save to " << filename << "\n";
        return;
    }
    outFile.write(scenarioBuffer.data(), scenarioBuffer.size());
    std::cout << "[FILE] Saved to " << filename << " (" << scenarioBuffer.size() << " bytes)\n";
}

void loadScenario(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary | std::ios::ate);
    if (!inFile.is_open()) {
        std::cerr << "[ERROR] Cannot open " << filename << "\n";
        return;
    }
    std::streamsize size = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    scenarioBuffer.resize(size);
    if (inFile.read(scenarioBuffer.data(), size)) {
        std::cout << "[FILE] Loaded " << size << " bytes from " << filename << "\n";
    }
    else {
        std::cerr << "[ERROR] Failed to read " << filename << "\n";
        scenarioBuffer.clear();
    }
}
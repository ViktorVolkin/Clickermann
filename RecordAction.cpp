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
    pushToBuffer((char)EventType::Keyboard); 
    RawKey rk = { vKey, isDown };
    pushToBuffer(rk);
}




void saveScenario(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);

    if (outFile.is_open()) {
        outFile.write(scenarioBuffer.data(), scenarioBuffer.size());
        outFile.close();
        std::cout << "[FILE] Saved to " << filename << " (" << scenarioBuffer.size() << " bytes)\n";
    }
}



void loadScenario(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary | std::ios::ate); 

    if (inFile.is_open()) {
        std::streamsize size = inFile.tellg(); 
        inFile.seekg(0, std::ios::beg);
        scenarioBuffer.resize(size);
        if (inFile.read(scenarioBuffer.data(), size)) {
            std::cout << "[FILE] Loaded " << size << " bytes from " << filename << "\n";
        }
        inFile.close();
    }
}



void recordMouse(long x, long y, MouseBtn btn, int action) {
    auto now = std::chrono::steady_clock::now();
    double ts = std::chrono::duration<double>(now - recordingStartTime).count();

    pushToBuffer(ts);
    pushToBuffer((char)EventType::Mouse);
    RawMouse rm = { x, y, btn, action };
    pushToBuffer(rm);
}

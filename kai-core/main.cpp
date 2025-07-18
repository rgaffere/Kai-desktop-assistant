/*
Project: Kai
Date: June 28th, 2025
Description: 

Kai is an all-in-one AI assistant for your desktop or laptop.
Created by Rissu Jacana to mimic J.A.R.V.I.S.

Let's get started...


#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include <cstdio>
#include <future>
#include <fstream>
#include <sstream>
#include "capture/screen.hpp"
#include "capture/audio.hpp"
#include "ui/overlay.hpp"
#include "third_party/json/json.hpp"

const char* path = "npm --prefix ../../kai-overlay run tauri dev &";

std::atomic<bool> running(true);
std::atomic<bool> isSpeaking(false);

void speakResponse(const std::string& text) {
    if (isSpeaking.exchange(true)) {
        std::cout << "[Kai] Skipping TTS — already speaking.\n";
        return;
    }

    std::string command = "node ../../kai-core/tts/speak-stream.js \"" + text + "\"";
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "[Kai] TTS playback error: " << result << std::endl;
    }

    isSpeaking = false;
}

void launchOverlay() {
    int result = system(path);
    if (result != 0) {
        std::cerr << "[Kai] Failed to launch overlay window." << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void runtimeLoop() {
    while (running) {
        std::string screenData = Capture::getScreenText();
        std::string audioData  = Capture::getAudioText();
        std::string prompt = screenData + " " + audioData;

        std::string escapedPrompt = prompt;
        std::replace(escapedPrompt.begin(), escapedPrompt.end(), '"', '\'');

        std::string command = "node ../../kai-core/model/stream-gpt.js \"" + escapedPrompt + "\"";
        std::system(command.c_str());

        std::ifstream infile("../../kai-overlay/src/overlay.json");
        std::string result;
        if (infile) {
            std::stringstream buffer;
            buffer << infile.rdbuf();

            try {
                nlohmann::json j = nlohmann::json::parse(buffer.str());
                result = j["text"].get<std::string>();
            } catch (...) {
                std::cerr << "[Kai] Failed to parse overlay.json\n";
            }
        }

        UI::displayOverlay(result);
        (void) std::async(std::launch::async, speakResponse, result);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    std::cout << "[Kai] Core runtime starting...\n";
    launchOverlay();
    UI::displayOverlay("Hello! I'm Kai!");

    std::thread coreThread(runtimeLoop);

    std::cout << "Press enter to exit.\n";
    std::cin.get();
    running = false;

    coreThread.join();
    std::cout << "[Kai] Core runtime exited.\n";
    return 0;
}
*/

/*
Project: Kai
Date: June 28th, 2025
Description: 

Kai is an all-in-one AI assistant for your desktop or laptop.
Created by Rissu Jacana to mimic J.A.R.V.I.S.

Let's get started...
*/

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include <cstdio>
#include <future>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "capture/screen.hpp"
#include "capture/audio.hpp"
#include "ui/overlay.hpp"
#include "third_party/json/json.hpp"

const char* path = "npm --prefix ../../kai-overlay run tauri dev &";

std::atomic<bool> running(true);
std::atomic<bool> isSpeaking(false);

// Speak the given text using TTS (Node.js script)
void speakResponse(const std::string& text) {
    if (isSpeaking.exchange(true)) {
        std::cout << "[Kai] Skipping TTS — already speaking.\n";
        return;
    }

    std::string command = "node ../../kai-core/tts/speak-stream.js \"" + text + "\"";
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "[Kai] TTS playback error: " << result << std::endl;
    }

    isSpeaking = false;
}

// Launch the Tauri overlay window
void launchOverlay() {
    int result = system(path);
    if (result != 0) {
        std::cerr << "[Kai] Failed to launch overlay window." << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// Call the Python intent classifier and return result as "intent:score"
std::string classifyIntent(const std::string& prompt) {
    std::string command = "python3 ../../kai-core/model/kai_intent_classifier.py \"" + prompt + "\"";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "unknown:0.0";

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

void runtimeLoop() {
    while (running) {
        std::string screenData = Capture::getScreenText();
        std::string audioData  = Capture::getAudioText();
        std::string prompt = screenData + " " + audioData;

        std::cout << "[DEBUG] Screen: " << screenData << '\n';
        std::cout << "[DEBUG] Audio: " << audioData << '\n';
        std::cout << "[DEBUG] Prompt: " << prompt << '\n';

        // Skip if no input
        if (prompt.empty()) {
            std::cout << "[Kai] No input detected. Waiting...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        std::string escapedPrompt = prompt;
        std::replace(escapedPrompt.begin(), escapedPrompt.end(), '"', '\'');

        std::string intentResult = classifyIntent(escapedPrompt);
        std::cout << "[Kai] Classifier output: " << intentResult << '\n';

        // Parse intent and score
        std::string intent = "unknown";
        float score = 0.0f;
        std::istringstream iss(intentResult);
        getline(iss, intent, ':');
        iss >> score;

        std::string overlayText;

        // Handle based on intent
        if (intent == "greeting") {
            overlayText = "Hey! I'm here.";
        } else if (intent == "open_app") {
            overlayText = "Opening app..."; // TODO: shell exec
        } else if (intent == "do_nothing") {
            std::cout << "[Kai] Intent was do_nothing — skipping output.\n";
            overlayText = "";
        } else if (intent == "ask_gpt" || intent == "unknown" || score < 0.6f) {
            std::cout << "[Kai] Triggering GPT due to intent fallback or low confidence.\n";
            std::string gptCommand = "node ../../kai-core/model/stream-gpt.js \"" + escapedPrompt + "\"";
            std::system(gptCommand.c_str());

            std::ifstream infile("../../kai-overlay/src/overlay.json");
            if (infile) {
                std::stringstream buffer;
                buffer << infile.rdbuf();
                try {
                    nlohmann::json j = nlohmann::json::parse(buffer.str());
                    overlayText = j["text"].get<std::string>();
                } catch (...) {
                    std::cerr << "[Kai] Failed to parse overlay.json\n";
                }
            }
        }

        if (!overlayText.empty()) {
            UI::displayOverlay(overlayText);
            (void) std::async(std::launch::async, speakResponse, overlayText);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}


int main() {
    std::cout << "[Kai] Core runtime starting...\n";
    launchOverlay();
    UI::displayOverlay("Hello! I'm Kai!");

    std::thread coreThread(runtimeLoop);

    std::cout << "Press enter to exit.\n";
    std::cin.get();
    running = false;

    coreThread.join();
    std::cout << "[Kai] Core runtime exited.\n";
    return 0;
}

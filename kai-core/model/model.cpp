#include "model.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include "json.hpp"
#include <cstdio>
#include <memory>
#include "../ui/overlay.hpp"

using json = nlohmann::json;

namespace {
    std::string apiKey;

    size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append((char*)contents, totalSize);
        return totalSize;
    }

    void loadApiKey() {
        const char* key = std::getenv("OPENAI_API_KEY");
        if (key) apiKey = key;
        else std::cerr << "OPENAI_API_KEY not found\n";
    }
}

/*std::string Model::queryModel(const std::string& input) {
    if (apiKey.empty()) loadApiKey();

    if (apiKey.empty()) return "[Missing API Key]";

    CURL* curl = curl_easy_init();
    std::string responseStr;

    if (curl) {
        std::string endpoint = "https://api.openai.com/v1/chat/completions";
        json body = {
            {"model", "gpt-4o"},
            {"messages", {
                {{"role", "system"}, {"content", "You are Kai, a helpful AI assistant. Respond in **1-2 concise sentences**, only with helpful, actionable info. Prioritize speed, clarity, and brevity."}},
                {{"role", "user"}, {"content", input}}
            }},
            {"temperature", 0.7},
            {"max_tokens", 100}
        };

        std::string bodyStr = body.dump();
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    

    // Parse OpenAI response
    try {
        auto parsed = json::parse(responseStr);
        return parsed["choices"][0]["message"]["content"];
    } catch (const std::exception& e) {
        std::cerr << "[JSON Parse Error] " << e.what() << std::endl;
        std::cerr << "[Raw Response] " << responseStr << std::endl;
        return "[Failed to parse model response]";
    }
}*/

std::string Model::queryModel(const std::string& input) {
    std::string command = "node ../../kai-core/model/stream-gpt.js \"" + input + "\"";
    std::string result;
    std::string bufferLine;
    bool capture = false;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "[Kai] Failed to run stream-gpt.js" << std::endl;
        return "[Error launching Node script]";
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        bufferLine = buffer;

        // Detect special marker
        if (bufferLine.find("[[KAI_RESPONSE_DONE]]") != std::string::npos) {
            capture = true;
            result.clear(); // Start fresh after marker
            continue;
        }

        if (capture) {
            result += bufferLine;
        }
    }

    pclose(pipe);

    // Now update the overlay (optional: also call TTS)
    UI::displayOverlay(result);

    return result;
}



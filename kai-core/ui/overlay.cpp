#include "overlay.hpp"
#include <fstream>
#include "json.hpp"

void UI::displayOverlay(const std::string& message) {
    nlohmann::json j = { {"text", message} };

    std::ofstream("../../kai-overlay/src/overlay.json") << j.dump(2);
}


/*
Date: June 28th, 2025
Description: Screen Capture stuff
*/
#pragma once
#include <string>

namespace Capture {
    std::string getScreenText();
    bool captureScreenshot(const std::string& filename);
}


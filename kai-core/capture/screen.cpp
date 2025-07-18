#include "screen.hpp"
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

std::string Capture::getScreenText() {
    // Step 1: Capture the screen to capture.png
    bool ok = captureScreenshot("capture.png");
    if (!ok) return "[screen capture failed]";

    // Step 2: Run Tesseract OCR
    int code = std::system("tesseract capture.png output --oem 1 --psm 3");
    if (code != 0) {
        std::cerr << "[Tesseract] OCR command failed.\n";
        return "[OCR failed]";
    }

    // Step 3: Read OCR output
    std::ifstream file("output.txt");
    if (!file.is_open()) {
        std::cerr << "[OCR] output.txt not found.\n";
        return "[OCR output missing]";
    }

    std::string ocrText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return ocrText.empty() ? "[OCR returned empty text]" : ocrText;
}

bool Capture::captureScreenshot(const std::string& filename) {
    // Get list of all on-screen windows
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
    if (!windowList) return false;

    // Loop through windows and find Kai overlay
    uint32_t overlayWindowID = 0;
    CFIndex count = CFArrayGetCount(windowList);
    for (CFIndex i = 0; i < count; i++) {
        CFDictionaryRef window = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
        CFStringRef ownerName = (CFStringRef)CFDictionaryGetValue(window, kCGWindowOwnerName);
        CFStringRef windowName = (CFStringRef)CFDictionaryGetValue(window, kCGWindowName);

        char ownerBuf[256];
        char titleBuf[256];

        if (ownerName) CFStringGetCString(ownerName, ownerBuf, sizeof(ownerBuf), kCFStringEncodingUTF8);
        if (windowName) CFStringGetCString(windowName, titleBuf, sizeof(titleBuf), kCFStringEncodingUTF8);

        if (strstr(ownerBuf, "Kai") || strstr(titleBuf, "kai-overlay")) {
            CFNumberRef windowIDRef = (CFNumberRef)CFDictionaryGetValue(window, kCGWindowNumber);
            if (windowIDRef) CFNumberGetValue(windowIDRef, kCFNumberSInt32Type, &overlayWindowID);
            break;
        }
    }

    CFRelease(windowList);

    // Capture screen excluding Kai overlay
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    CGImageRef image = CGWindowListCreateImage(
        CGRectInfinite,
        kCGWindowListOptionOnScreenBelowWindow,
        overlayWindowID,
        kCGWindowImageDefault
    );
    #pragma clang diagnostic pop


    if (!image) {
        std::cerr << "[Capture] Failed to capture screen image without overlay.\n";
        return false;
    }

    // Save image
    CFURLRef url = CFURLCreateWithFileSystemPath(
        nullptr,
        CFStringCreateWithCString(nullptr, filename.c_str(), kCFStringEncodingUTF8),
        kCFURLPOSIXPathStyle,
        false
    );

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, nullptr);
    #pragma clang diagnostic pop
    if (!dest) {
        CFRelease(url);
        CGImageRelease(image);
        return false;
    }

    CGImageDestinationAddImage(dest, image, nullptr);
    bool success = CGImageDestinationFinalize(dest);

    CFRelease(dest);
    CFRelease(url);
    CGImageRelease(image);

    return success;
}

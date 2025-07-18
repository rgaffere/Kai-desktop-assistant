cmake_minimum_required(VERSION 3.15)
project(kai-core)

set(CMAKE_CXX_STANDARD 17)

# Source files
file(GLOB_RECURSE SRC
    main.cpp
    capture/*.cpp
    model/*.cpp
    ui/*.cpp
)

# Executable target
add_executable(kai-core ${SRC})

# macOS-specific frameworks for screen capture
if(APPLE)
    target_link_libraries(kai-core
        "-framework ApplicationServices"
        "-framework CoreGraphics"
        "-framework ImageIO"
    )
endif()

# libcurl for OpenAI API HTTP requests
find_package(CURL REQUIRED)
target_link_libraries(kai-core CURL::libcurl)

# Include path for nlohmann/json (assumes it's in third_party/json/json.hpp)
target_include_directories(kai-core PRIVATE third_party/json)

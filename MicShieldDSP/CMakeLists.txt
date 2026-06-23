cmake_minimum_required(VERSION 3.21)
project(MicShieldDSP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1)
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3)
FetchContent_Declare(miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.22)

FetchContent_MakeAvailable(spdlog nlohmann_json miniaudio)

file(GLOB_RECURSE MICSHIELD_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM MICSHIELD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(micshield_core ${MICSHIELD_SOURCES})
target_include_directories(micshield_core
    PUBLIC include
    PRIVATE ${miniaudio_SOURCE_DIR})
target_link_libraries(micshield_core
    PUBLIC spdlog::spdlog nlohmann_json::nlohmann_json)
if(WIN32)
    target_link_libraries(micshield_core PUBLIC ole32 uuid)
endif()

if(MSVC)
    target_compile_options(micshield_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(micshield_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(MicShieldDSP src/main.cpp)
target_link_libraries(MicShieldDSP PRIVATE micshield_core)

enable_testing()
add_subdirectory(tests)

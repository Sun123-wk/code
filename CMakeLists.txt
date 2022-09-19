cmake_minimum_required(VERSION 3.0)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.1/json.tar.xz)
FetchContent_MakeAvailable(json)

project(IMSYNTHESIS)

include_directories(include)

add_executable(integrate_cmake_exe main.cpp src/imSynthesis.cpp)

target_link_libraries(integrate_cmake_exe PRIVATE nlohmann_json::nlohmann_json)
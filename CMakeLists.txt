cmake_minimum_required(VERSION 3.0)

project(IMSYNTHESIS)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.1/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(OpenCV REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} include)

add_executable(integrate_cmake_exe main.cpp src/imSynthesis.cpp)

target_link_libraries(integrate_cmake_exe PRIVATE nlohmann_json::nlohmann_json PRIVATE ${OpenCV_LIBS})
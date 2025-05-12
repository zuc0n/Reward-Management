cmake_minimum_required(VERSION 3.10)
project(RewardManagement)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(include)
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(RewardManagement ${SOURCES})

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)

target_link_libraries(RewardManagement PRIVATE nlohmann_json::nlohmann_json OpenSSL::Crypto OpenSSL::SSL) 
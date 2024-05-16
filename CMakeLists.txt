cmake_minimum_required(VERSION 3.25)
project(ipgdrive VERSION 0.1 LANGUAGES CXX)

find_package(Boost 1.85 COMPONENTS context filesystem system thread program_options REQUIRED)
find_package(OpenSSL REQUIRED)

include(./cmake/CPM.cmake)
CPMAddPackage("gh:nlohmann/json@3.11.3")

add_subdirectory(conns)
add_executable(ipgdrive ipgdrive.cc)
target_link_libraries(ipgdrive Boost::program_options conns)

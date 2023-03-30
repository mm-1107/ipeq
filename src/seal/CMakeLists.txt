cmake_minimum_required(VERSION 3.18)
project(SEALDB VERSION 4.1.1 LANGUAGES CXX)
# Import Microsoft SEAL
find_package(SEAL 4.1.1 EXACT REQUIRED)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE SEAL::seal)

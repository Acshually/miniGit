# Set the minimum required version for CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(minigit)

# Set the C++ standard to C++17 (required for <filesystem>)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
# This will compile main.cpp and minigit.cpp together
add_executable(minigit main.cpp minigit.cpp)

# Note: No external libraries are needed as <filesystem>
# is part of the standard library in C++17.

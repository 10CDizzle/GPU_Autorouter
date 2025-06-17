cmake_minimum_required(VERSION 3.18)
project(PCBAutorouter LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Enable CMAKE_AUTOMOC, CMAKE_AUTORCC, CMAKE_AUTOUIC for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Add subdirectories
# We'll add autorouter_core later
add_subdirectory(src/gui)
# add_subdirectory(src/autorouter_core) # For later
# add_subdirectory(tests) # For later
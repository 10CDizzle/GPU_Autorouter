cmake_minimum_required(VERSION 3.18)
project(PCBAutorouter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find and enable CUDA if available ---
find_package(CUDAToolkit)
if(CUDAToolkit_FOUND)
    message(STATUS "Found CUDA Toolkit ${CUDAToolkit_VERSION}. Enabling CUDA language support.")
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
else()
    message(WARNING "CUDA Toolkit not found. Building without CUDA acceleration.")
endif()

# --- Configure and include wxWidgets from source ---
# Build wxWidgets as a static library to simplify deployment
set(wxBUILD_SHARED OFF)
# We don't need to build the samples or tests
set(wxBUILD_SAMPLES OFF CACHE INTERNAL "wxWidgets Samples")
set(wxBUILD_TESTS OFF CACHE INTERNAL "wxWidgets Tests")

add_subdirectory(lib/wxWidgets-3.3.0)
# Add subdirectories
# We'll add autorouter_core later
add_subdirectory(src/gui)
# add_subdirectory(src/autorouter_core) # For later
# add_subdirectory(tests) # For later
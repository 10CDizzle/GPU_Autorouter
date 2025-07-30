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


# We need to build the static library version of Catch2 to get the
# Catch2::Catch2WithMain target that includes the main() function.
set(CATCH_BUILD_STATIC_LIBRARY ON)

# This builds wxWidgets and its dependencies (including Catch2).
add_subdirectory(lib/wxWidgets-3.3.0)

# We must manually include the Catch2 testing framework, which is bundled with
# wxWidgets, so that its targets are available for our own test executable.
add_subdirectory(lib/wxWidgets-3.3.0/3rdparty/catch)

# Add our KiCad parser library
add_subdirectory(src/kicad)

# Define our core logic as a library that other targets can link to.
add_subdirectory(src/core)

# Add subdirectories
add_subdirectory(src/gui)

# Add our tests directory, which will create the test executable.
add_subdirectory(tests)
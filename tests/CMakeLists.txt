cmake_minimum_required(VERSION 3.18)

project(PCBAutorouterTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing for this project
enable_testing()
include(CTest)

# We need to find wxWidgets to link our test executable against it.
# The parent CMakeLists.txt has already configured it via add_subdirectory,
# so the link targets (wx::core, wx::base) are already available.
add_executable(AutorouterTests
    main.cpp
)

# Pass the absolute path of the test file directory to the executable.
# This is more robust than relying on relative paths from the build directory.
target_compile_definitions(AutorouterTests PRIVATE
    PCB_FILES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/pcb_files"
)

# Link our test against the core logic, wxWidgets, and Catch2
# We use Catch2::Catch2 because we provide our own main() in main.cpp
# to initialize wxWidgets.
target_link_libraries(AutorouterTests PRIVATE
    AutorouterCore
    KiCadParser
    wx::core
    wx::base
    Catch2::Catch2
)

# Add the test to CTest
add_test(NAME AutorouterTests COMMAND AutorouterTests)

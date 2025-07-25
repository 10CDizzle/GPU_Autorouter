cmake_minimum_required(VERSION 3.18)

project(PCBAutorouterTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We need to find wxWidgets to link our test executable against it.
# The parent CMakeLists.txt has already configured it via add_subdirectory,
# so the link targets (wx::core, wx::base) are already available.
add_executable(AutorouterTests
    main.cpp
)

# Link our test against the core logic, wxWidgets, and Catch2
target_link_libraries(AutorouterTests PRIVATE
    AutorouterCore
    wx::core
    wx::base
    Catch2::Catch2WithMain
)
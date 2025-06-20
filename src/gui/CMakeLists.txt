cmake_minimum_required(VERSION 3.18)

project(PCBAutorouterGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PCBAutorouterApp
    main.cpp
)

set_target_properties(PCBAutorouterApp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Link against the modern wxWidgets targets. This automatically handles include paths.
target_link_libraries(PCBAutorouterApp PRIVATE wx::core wx::base)
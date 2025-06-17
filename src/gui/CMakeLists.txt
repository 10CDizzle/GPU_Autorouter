cmake_minimum_required(VERSION 3.18)

project(PCBAutorouterGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PCBAutorouterApp
    main.cpp
)

target_link_libraries(PCBAutorouterApp PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)
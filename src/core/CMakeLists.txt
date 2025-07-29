cmake_minimum_required(VERSION 3.18)

project(AutorouterCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the core logic as a library
add_library(AutorouterCore
    PcbData.cpp
    PcbParser.cpp
    RoutingGrid.cpp
    AutorouterCore.cpp
)

# Link against wxWidgets (for wxString, etc.)
target_link_libraries(AutorouterCore PUBLIC kicad_parser wx::core wx::base)
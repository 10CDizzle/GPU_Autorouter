cmake_minimum_required(VERSION 3.18)

# The project is defined in the top-level CMakeLists.txt

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the core logic as a library
add_library(AutorouterCore
    PcbData.cpp
    PcbParser.cpp
    RoutingGrid.cpp
    AutorouterCore.cpp
)

# Add the parent directory (src) to the include path.
# This is necessary for includes like #include "core/PcbData.h" to work.
target_include_directories(AutorouterCore PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
)

# Manually add the wxWidgets include directories. This is a workaround, as
# target_link_libraries should handle this, but appears not to be propagating them.
target_include_directories(AutorouterCore PUBLIC
    "${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.3.0/include"
    "${CMAKE_BINARY_DIR}/lib/wxWidgets-3.3.0/include"
)

# Link against wxWidgets (for wxString, etc.)
# The PUBLIC keyword ensures that any target linking against AutorouterCore
# (like our GUI app) also gets the necessary wxWidgets include paths.
# The KiCadParser is an internal implementation detail.
target_link_libraries(AutorouterCore
    PUBLIC wx::core wx::base wx::adv
    PRIVATE KiCadParser
)
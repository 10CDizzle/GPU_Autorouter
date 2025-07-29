add_library(kicad_parser
    KicadPcb.cpp
    SexpParser.cpp
)

# Add the parent directory (src) to the include path so that
# headers can be found via "kicad/Sexp.h" and other code
# can find our headers via "kicad/KicadPcb.h".
target_include_directories(kicad_parser
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
)
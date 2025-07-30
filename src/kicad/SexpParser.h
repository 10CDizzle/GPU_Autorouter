#ifndef KICAD_SEXP_PARSER_H
#define KICAD_SEXP_PARSER_H

#include "kicad/Sexp.h"
#include <string>
#include <stdexcept>

class SexpParser {
public:
    /**
     * @brief Parses a string containing an S-expression into a node tree.
     * @param input The string to parse.
     * @return The root SexpNode of the parsed tree.
     * @throws std::runtime_error on parsing errors.
     */
    static SexpNode parse(const std::string& input);

private:
    SexpParser(const std::string& input);
    SexpNode parseNode();
    SexpList parseList();
    SexpAtom parseAtom();
    void skipWhitespace();
    char peek();
    char get();
    bool eof();

    const std::string& m_input;
    size_t m_pos;
};

#endif // KICAD_SEXP_PARSER_H
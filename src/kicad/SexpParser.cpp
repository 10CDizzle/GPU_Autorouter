#include "kicad/SexpParser.h"

// Public static method
SexpNode SexpParser::parse(const std::string& input) {
    SexpParser parser(input);
    return parser.parseNode();
}

// Private constructor
SexpParser::SexpParser(const std::string& input) : m_input(input), m_pos(0) {}

// Main parsing routine for a single node
SexpNode SexpParser::parseNode() {
    skipWhitespace();
    if (eof()) {
        throw std::runtime_error("Unexpected end of input while parsing node.");
    }

    if (peek() == '(') {
        return SexpNode{parseList()};
    } else {
        return SexpNode{parseAtom()};
    }
}

// Parses a list starting with '('
SexpList SexpParser::parseList() {
    get(); // Consume '('
    SexpList list;
    while (true) {
        skipWhitespace();
        if (eof()) {
            throw std::runtime_error("Unmatched opening parenthesis.");
        }
        if (peek() == ')') {
            get(); // Consume ')'
            break;
        }
        list.push_back(parseNode());
    }
    return list;
}

// Parses an atom (quoted or unquoted string)
SexpAtom SexpParser::parseAtom() {
    skipWhitespace();
    if (peek() == '"') {
        // Quoted string
        get(); // Consume opening '"'
        std::string atom;
        while (!eof() && peek() != '"') {
            // KiCad format doesn't seem to use escape sequences like \",
            // so we'll keep it simple for now.
            atom += get();
        }
        if (eof()) {
            throw std::runtime_error("Unmatched quote in string literal.");
        }
        get(); // Consume closing '"'
        return atom;
    } else {
        // Unquoted atom
        std::string atom;
        while (!eof() && !isspace(peek()) && peek() != '(' && peek() != ')') {
            atom += get();
        }
        if (atom.empty()) {
            throw std::runtime_error("Expected an atom but found none.");
        }
        return atom;
    }
}

void SexpParser::skipWhitespace() {
    while (!eof() && isspace(peek())) {
        m_pos++;
    }
}

char SexpParser::peek() {
    if (eof()) return '\0';
    return m_input[m_pos];
}

char SexpParser::get() {
    if (eof()) return '\0';
    return m_input[m_pos++];
}

bool SexpParser::eof() {
    return m_pos >= m_input.length();
}
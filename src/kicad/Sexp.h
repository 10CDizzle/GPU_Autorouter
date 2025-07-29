#ifndef KICAD_SEXP_H
#define KICAD_SEXP_H

#include <string>
#include <vector>
#include <variant>

// An S-expression can be either an atom (a string) or a list of other S-expressions.

class SexpNode; // Forward declaration

using SexpList = std::vector<SexpNode>;
using SexpAtom = std::string;

class SexpNode {
public:
    // A node can hold either an atom or a list of child nodes.
    std::variant<SexpAtom, SexpList> value;

    // Convenience functions to check the type
    bool isAtom() const { return std::holds_alternative<SexpAtom>(value); }
    bool isList() const { return std::holds_alternative<SexpList>(value); }

    // Convenience functions to get the value (with checks)
    const SexpAtom& getAtom() const { return std::get<SexpAtom>(value); }
    const SexpList& getList() const { return std::get<SexpList>(value); }
};

#endif // KICAD_SEXP_H
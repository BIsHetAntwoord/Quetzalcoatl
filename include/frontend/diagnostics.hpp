#ifndef _QUETZALCOATL_FRONTEND_DIAGNOSTICS_HPP
#define _QUETZALCOATL_FRONTEND_DIAGNOSTICS_HPP

#include "source_location.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <span>

struct Diagnostic {
    enum Type {
        ERROR,
        WARNING,
        NOTE
    };

    Type type;
    SourceLocation loc;
    std::string msg;

    Diagnostic(Type type, SourceLocation loc, std::string_view msg):
        type(type), loc(loc), msg(msg) {
    }
};

class Diagnostics {
    std::vector<Diagnostic> msgs;

public:
    void error(SourceLocation loc, std::string_view msg);
    void warning(SourceLocation loc, std::string_view msg);
    void note(SourceLocation loc, std::string_view msg);

    std::span<const Diagnostic> messages() const;
};

#endif

#ifndef _QUETZALCOATL_COMPILE_ERROR_HPP
#define _QUETZALCOATL_COMPILE_ERROR_HPP

#include <cstddef>
#include <string>
#include <string_view>

struct CompileError {
    SourceLocation loc;
    std::string msg;

    CompileError(SourceLocation loc, std::string_view msg):
        loc(loc), msg(msg) {
    }
};

#endif

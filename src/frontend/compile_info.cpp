#include "frontend/compile_info.hpp"

#include <iostream>

namespace {
    constexpr const char* diagnostic_colors[] = {
        "\x1B[31m", // Red
        "\x1B[33m", // Yellow
        "\x1B[90m" // Gray
    };

    constexpr const char* diagnostic_tags[] = {
        "error",
        "warning",
        "note"
    };

    constexpr const char* reset_color = "\x1B[m";
    constexpr const char* msg_style = "\x1B[1m";
}

void CompileInfo::printDiagnostics(std::ostream& out, bool want_color) const {
    for (const auto& [type, loc, msg] : this->diagnostics.messages()) {
        if (want_color)
            out << msg_style;
        auto filename = this->files.getFile(loc.file_id);
        out << filename << ':' << loc.line << ':' << loc.column << ": ";
        if(want_color)
            out << diagnostic_colors[type];
        out << diagnostic_tags[type] << ": ";
        if(want_color)
           out << reset_color << msg_style;
        out << msg << '\n';
    }
}


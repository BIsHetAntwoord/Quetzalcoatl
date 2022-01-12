#include "frontend/diagnostics.hpp"

void Diagnostics::error(SourceLocation loc, std::string_view msg) {
    this->msgs.emplace_back(Diagnostic::ERROR, loc, msg);
}

void Diagnostics::warning(SourceLocation loc, std::string_view msg) {
    this->msgs.emplace_back(Diagnostic::WARNING, loc, msg);
}

void Diagnostics::note(SourceLocation loc, std::string_view msg) {
    this->msgs.emplace_back(Diagnostic::NOTE, loc, msg);
}

std::span<const Diagnostic> Diagnostics::messages() const {
    return this->msgs;
}

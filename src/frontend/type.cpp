#include "frontend/type.hpp"

#include <fmt/ostream.h>

#include <cstddef>
#include <cassert>

namespace {
    constexpr static const char* const PRIMITIVE_NAMES[] = {
        "void",
        "bool",
        "char",
        "wchar t",
        "unsigned char",
        "signed char",
        "short",
        "unsigned short",
        "int",
        "unsigned int",
        "long",
        "unsigned long",
        "long long",
        "unsigned long long",
        "float",
        "double",
        "long double",
    };
}

void PrimitiveType::print(std::ostream& os) const {
    fmt::print(os, "{}", PRIMITIVE_NAMES[this->kind]);
}

void PointerType::print(std::ostream& os) const {
    this->child->print(os);
    fmt::print(os, "*");
}

TypeTable::TypeTable() {
    for (size_t i = size_t{PrimitiveType::VOID}; i < size_t{PrimitiveType::LONG_DOUBLE}; ++i) {
        assert(this->types.size() == i);
        this->types.push_back(std::make_unique<PrimitiveType>(static_cast<PrimitiveType::Kind>(i)));
    }
}

const Type* TypeTable::getPrimitiveType(PrimitiveType::Kind kind) const {
    return this->types[static_cast<size_t>(kind)].get();
}

std::ostream& operator<<(std::ostream& os, const Type& type) {
    type.print(os);
    return os;
}

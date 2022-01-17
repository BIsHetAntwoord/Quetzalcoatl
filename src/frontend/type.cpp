#include "frontend/type.hpp"

#include <cstddef>
#include <cassert>

TypeTable::TypeTable() {
    for (size_t i = size_t{PrimitiveType::VOID}; i < size_t{PrimitiveType::LONG_DOUBLE}; ++i) {
        assert(this->types.size() == i);
        this->types.push_back(std::make_unique<PrimitiveType>(static_cast<PrimitiveType::Kind>(i)));
    }
}

const Type* TypeTable::getPrimitiveType(PrimitiveType::Kind kind) const {
    return this->types[static_cast<size_t>(kind)].get();
}

#include "lexer/stringtable.hpp"

size_t StringTable::add(View str) {
    size_t offset = this->string_bytes.size();
    this->string_bytes.insert(this->string_bytes.end(), str.begin(), str.end());
    size_t id = this->strings.size();
    this->strings.push_back({offset, str.size()});
    return id;
}

void StringTable::pushToMostRecent(uint8_t c) {
    this->string_bytes.push_back(c);
    auto& str = this->strings.back();
    ++str.length;
}

StringTable::View StringTable::get(size_t id) const {
    auto [offset, length] = this->strings[id];
    return View(&this->string_bytes[offset], length);
}

#include "unicode.hpp"

#include <ostream>
#include <cassert>

uint8_t CodePoint::Utf8Iterator::operator*() const {
    constexpr uint8_t masks[] = {0, 0, 0b11000000, 0b011100000, 0b11110000};
    auto length = ToUtf8{this->cp}.length();
    auto offset = this->byte_offset - 1;

    uint8_t mask = length == this->byte_offset ? masks[length] : 0b10000000;
    return ((this->cp >> (offset * 6)) & 0b111111) | mask;
}

auto CodePoint::Utf8Iterator::operator++() -> Utf8Iterator& {
    --this->byte_offset;
    return *this;
}

bool CodePoint::Utf8Iterator::operator==(const Utf8Iterator& other) const {
    return (this->byte_offset == 0 && other.byte_offset == 0) ||
        (this->cp == other.cp && this->byte_offset == other.byte_offset);
}

bool CodePoint::Utf8Iterator::operator!=(const Utf8Iterator& other) const {
    return !(*this == other);
}

uint8_t CodePoint::ToUtf8::length() const {
    if(this->cp < 0x80)
        return 1;
    if(this->cp < 0x800)
        return 2;
    if(this->cp < 0x10000)
        return 3;
    if(this->cp < 0x110000)
        return 4;
    assert(false);
}

auto CodePoint::ToUtf8::begin() const -> Utf8Iterator {
    return Utf8Iterator{this->cp, this->length()};
}

auto CodePoint::ToUtf8::end() const -> Utf8Iterator {
    return Utf8Iterator{this->cp, 0};
}

auto CodePoint::toUtf8() const -> ToUtf8 {
    assert(this->isValidUtf8());
    return ToUtf8{this->value};
}

bool CodePoint::isValidUtf8() const {
    return !((this->value >= 0xD800 && this->value <= 0xDFFF) || this->value >= 0x110000); 
}

std::ostream& operator<<(std::ostream& os, CodePoint::ToUtf8 cp) {
    for(auto c : cp) {
        os << c;
    }
    return os;
}

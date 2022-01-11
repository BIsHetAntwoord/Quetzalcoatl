#ifndef _QUETZALCOATL_UNICODE_HPP
#define _QUETZALCOATL_UNICODE_HPP

#include <iosfwd>
#include <cstddef>
#include <cstdint>

struct CodePoint {
    uint32_t value;

    struct Utf8Iterator {
        uint32_t cp;
        uint8_t byte_offset;

        uint8_t operator*() const;
        Utf8Iterator& operator++();
        bool operator==(const Utf8Iterator& other) const;
        bool operator!=(const Utf8Iterator& other) const;
    };

    struct ToUtf8 {
        uint32_t cp;

        uint8_t length() const;
        Utf8Iterator begin() const;
        Utf8Iterator end() const;
    };

    ToUtf8 toUtf8() const;
    bool isValidUtf8() const;
};

std::ostream& operator<<(std::ostream& os, CodePoint::ToUtf8 cp);

#endif

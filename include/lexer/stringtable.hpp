#ifndef _QUETZALCOATL_LEXER_STRINGTABLE_HPP
#define _QUETZALCOATL_LEXER_STRINGTABLE_HPP

#include <vector>
#include <string_view>
#include <cstdint>
#include <cstddef>

class StringTable {
    struct String {
        size_t offset;
        size_t length;
    };

    std::vector<uint8_t> string_bytes;
    std::vector<String> strings;
public:
    using View = std::basic_string_view<uint8_t>;

    size_t add(View str);
    void pushToMostRecent(uint8_t c);
    View get(size_t id) const;
};

#endif

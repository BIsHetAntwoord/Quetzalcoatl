#ifndef _QUETZALCOATL_LEXER_FILETABLE_HPP
#define _QUETZALCOATL_LEXER_FILETABLE_HPP

#include <string>
#include <string_view>
#include <cstddef>
#include <vector>
#include <unordered_map>

class FileTable {
private:
    std::vector<std::string> files;
    std::unordered_map<std::string_view, size_t> index_lookup;
public:
    size_t addFile(std::string_view);
    std::string_view getFile(size_t) const;
};

#endif

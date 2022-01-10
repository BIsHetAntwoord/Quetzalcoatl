#ifndef _QUETZALCOATL_LEXER_FILETABLE_HPP
#define _QUETZALCOATL_LEXER_FILETABLE_HPP

#include <string>
#include <cstddef>
#include <vector>
#include <unordered_map>

class FileTable {
private:
    std::vector<std::string> files;
    std::unordered_map<std::string, size_t> index_lookup;
public:
    size_t addFile(const std::string&);
    std::string getFile(size_t) const;
};

#endif

#include "lexer/filetable.hpp"

size_t FileTable::addFile(std::string_view filename) {
    if(this->index_lookup.count(filename) > 0)
        return this->index_lookup.at(filename);

    size_t new_id = this->files.size();
    this->files.emplace_back(filename);
    return this->index_lookup[this->files.back()] = new_id;
}

std::string_view FileTable::getFile(size_t id) const {
    return this->files[id];
}

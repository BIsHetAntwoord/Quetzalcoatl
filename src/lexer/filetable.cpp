#include "lexer/filetable.hpp"

size_t FileTable::addFile(const std::string& filename) {
    if(this->index_lookup.count(filename) > 0)
        return this->index_lookup.at(filename);

    size_t new_id = this->files.size();
    this->files.push_back(filename);
    return this->index_lookup[filename] = new_id;
}

std::string FileTable::getFile(size_t id) const {
    return this->files[id];
}
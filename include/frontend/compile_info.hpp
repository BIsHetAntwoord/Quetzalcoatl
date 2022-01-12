#ifndef _QUETZALCOATL_FRONTEND_COMPILE_INFO_HPP
#define _QUETZALCOATL_FRONTEND_COMPILE_INFO_HPP

#include <vector>

#include "frontend/datatype.hpp"
#include "frontend/filetable.hpp"
#include "frontend/stringtable.hpp"
#include "compile_error.hpp"

struct CompileInfo {
    FileTable files;
    DataTypeTable types;
    StringTable strings;

    std::vector<CompileError> errors;
};

#endif

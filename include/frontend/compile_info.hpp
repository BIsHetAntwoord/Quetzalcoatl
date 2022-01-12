#ifndef _QUETZALCOATL_FRONTEND_COMPILE_INFO_HPP
#define _QUETZALCOATL_FRONTEND_COMPILE_INFO_HPP

#include "frontend/datatype.hpp"
#include "frontend/filetable.hpp"
#include "frontend/stringtable.hpp"

struct CompileInfo {
    FileTable files;
    DataTypeTable types;
    StringTable strings;
};

#endif

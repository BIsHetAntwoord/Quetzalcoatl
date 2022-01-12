#ifndef _QUETZALCOATL_FRONTEND_COMPILE_INFO_HPP
#define _QUETZALCOATL_FRONTEND_COMPILE_INFO_HPP

#include <vector>
#include <iosfwd>

#include "frontend/datatype.hpp"
#include "frontend/filetable.hpp"
#include "frontend/stringtable.hpp"
#include "frontend/diagnostics.hpp"

struct CompileInfo {
    FileTable files;
    DataTypeTable types;
    StringTable strings;
    Diagnostics diagnostics;

    void printDiagnostics(std::ostream& out, bool want_color) const; 
};

#endif

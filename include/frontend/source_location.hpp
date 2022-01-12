#ifndef _QUETZALCOATL_FRONTEND_SOURCE_LOCATION_HPP
#define _QUETZALCOATL_FRONTEND_SOURCE_LOCATION_HPP

#include <cstddef>

struct SourceLocation {
    size_t line;
    size_t column;
    size_t file_id;
};

#endif

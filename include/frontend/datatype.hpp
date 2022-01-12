#ifndef _QUETZALCOATL_FRONTEND_DATATYPE_HPP
#define _QUETZALCOATL_FRONTEND_DATATYPE_HPP

#include <vector>
#include <cstddef>

enum class BaseDataType {
    VOID,
    CHAR,
    UNSIGNED_CHAR,
    SIGNED_CHAR,
    SHORT,
    UNSIGNED_SHORT,
    INT,
    UNSIGNED_INT,
    LONG,
    UNSIGNED_LONG,
    FLOAT,
    DOUBLE,

    POINTER,
    FUNC_POINTER,
    STRUCT,
    CLASS
};

struct DataType {
    BaseDataType base_type;
    std::vector<size_t> sub_types;
};

class DataTypeTable {
    private:
        std::vector<DataType> data_types;
    public:
        DataTypeTable();

        size_t getBaseTypeId(BaseDataType);

        DataType& getDataType(size_t);
        const DataType& getDataType(size_t) const;
};

#endif

#include "datatype.hpp"

DataTypeTable::DataTypeTable() {
    for(size_t i = 0; i <= size_t(BaseDataType::DOUBLE); ++i) {
        DataType t;
        t.base_type = BaseDataType(i);
        this->data_types.push_back(t);
    }
}

size_t DataTypeTable::getBaseTypeId(BaseDataType base_type) {
    return size_t(base_type);
}

DataType& DataTypeTable::getDataType(size_t id) {
    return this->data_types[id];
}

const DataType& DataTypeTable::getDataType(size_t id) const {
    return this->data_types[id];
}
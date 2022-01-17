#ifndef _QUETZALCOATL_FRONTEND_TYPE_HPP
#define _QUETZALCOATL_FRONTEND_TYPE_HPP

#include <memory>
#include <vector>
#include <utility>

struct Type {
    Type() = default;

    Type(const Type&) = delete;
    Type(Type&&) = delete;

    Type& operator=(const Type&) = delete;
    Type& operator=(Type&&) = delete;

    virtual ~Type() = default;
};

struct PrimitiveType : public Type {
    enum Kind {
        VOID,
        BOOL,

        CHAR,
        WCHAR_T,
        UNSIGNED_CHAR,
        SIGNED_CHAR,

        SHORT,
        UNSIGNED_SHORT,
        INT,
        UNSIGNED_INT,
        LONG,
        UNSIGNED_LONG,
        LONG_LONG,
        UNSIGNED_LONG_LONG,

        FLOAT,
        DOUBLE,
        LONG_DOUBLE,
    };

    Kind kind;

    explicit PrimitiveType(Kind kind): kind(kind) {}
};

struct PointerType : public Type {
    // TODO: Qualifiers
    Type* child;
};

class TypeTable {
private:
    std::vector<std::unique_ptr<Type>> types;

public:
    TypeTable();
    const Type* getPrimitiveType(PrimitiveType::Kind kind) const;

    template <typename T, typename... Args>
    const Type* addType(Args&&... args) {
        if constexpr (std::is_same_v<T, PrimitiveType>)
            return this->getPrimitiveType(std::forward<Args>()...);

        auto unique = std::make_unique<T>(std::forward<Args>()...);
        auto* ptr = unique.get();
        this->types.push_back(std::move(ptr));
        return ptr;
    }
};

#endif

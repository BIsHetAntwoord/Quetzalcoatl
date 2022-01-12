#ifndef _QUETZALCOATL_FRONTEND_AST_HPP
#define _QUETZALCOATL_FRONTEND_AST_HPP

#include <vector>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>

const size_t INVALID_ASTNODE_ID = std::numeric_limits<size_t>::max();

enum class AstNodeType {
    INVALID,

    INTEGER_CONSTANT
};

struct AstNode {
    AstNodeType type;
    std::vector<size_t> children;
    size_t datatype;

    union {
        uint64_t integer;
    };
};

class AstTable {
private:
    std::vector<AstNode> nodes;
public:
    size_t addNode(AstNodeType);
    size_t addNode(AstNodeType, std::initializer_list<size_t>);
    size_t addNode(AstNodeType, const std::vector<size_t>&);
    size_t addNode(AstNodeType, size_t, uint64_t);

    AstNode& getNode(size_t);
    const AstNode& getNode(size_t) const;
};

#endif

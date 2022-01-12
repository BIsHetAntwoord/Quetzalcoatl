#ifndef _QUETZALCOATL_FRONTEND_AST_HPP
#define _QUETZALCOATL_FRONTEND_AST_HPP

#include <vector>
#include <cstdint>
#include <initializer_list>

enum class AstNodeType {
    INVALID
};

struct AstNode {
    AstNodeType type;
    std::vector<size_t> children;
};

class AstTable {
private:
    std::vector<AstNode> nodes;
public:
    size_t addNode(AstNodeType);
    size_t addNode(AstNodeType, std::initializer_list<size_t>);
    size_t addNode(AstNodeType, const std::vector<size_t>&);

    AstNode& getNode(size_t);
    const AstNode& getNode(size_t) const;
};

#endif

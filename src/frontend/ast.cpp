#include "frontend/ast.hpp"

AstNode::AstNode(AstNodeType type, std::initializer_list<size_t> children, size_t datatype)
    : type(type), children(children), datatype(datatype) {}

AstNode::AstNode(AstNodeType type, const std::vector<size_t>& children, size_t datatype)
    : type(type), children(children), datatype(datatype) {}

IntegerAstNode::IntegerAstNode(AstNodeType type, std::initializer_list<size_t> children,
        size_t datatype, uint64_t integer) : AstNode(type, children, datatype), integer(integer) {}

IntegerAstNode::IntegerAstNode(AstNodeType type, const std::vector<size_t>& children,
        size_t datatype, uint64_t integer) : AstNode(type, children, datatype), integer(integer) {}

SwitchAstNode::SwitchAstNode(AstNodeType type, std::initializer_list<size_t> children, size_t datatype)
    : AstNode(type, children, datatype), default_id(INVALID_ASTNODE_ID) {}

SwitchAstNode::SwitchAstNode(AstNodeType type, const std::vector<size_t>& children, size_t datatype)
    : AstNode(type, children, datatype), default_id(INVALID_ASTNODE_ID) {}

size_t AstTable::addNode(AstNodeType type) {
    size_t id = this->nodes.size();
    this->nodes.emplace_back(new AstNode(type, {}, 0));
    return id;
}

size_t AstTable::addNode(AstNodeType type, std::initializer_list<size_t> children) {
    size_t id = this->nodes.size();
    this->nodes.emplace_back(new AstNode(type, children, 0));
    return id;
}

size_t AstTable::addNode(AstNodeType type, const std::vector<size_t>& children) {
    size_t id = this->nodes.size();
    this->nodes.emplace_back(new AstNode(type, children, 0));
    return id;
}

size_t AstTable::addNode(AstNodeType type, size_t datatype) {
    size_t id = this->nodes.size();
    this->nodes.emplace_back(new AstNode(type, {}, datatype));
    return id;
}

size_t AstTable::addNode(AstNodeType type, size_t datatype, std::initializer_list<size_t> children) {
    size_t id = this->nodes.size();
    this->nodes.emplace_back(new AstNode(type, children, datatype));
    return id;
}

size_t AstTable::addNode(AstNodeType type, size_t datatype, const std::vector<size_t>& children) {
    size_t id = this->nodes.size();
    this->nodes.emplace_back(new AstNode(type, children, datatype));
    return id;
}

size_t AstTable::addIntegerNode(AstNodeType type, size_t datatype, uint64_t integer) {
    size_t id = this->nodes.size();
    this->nodes.emplace_back(new IntegerAstNode(type, {}, datatype, integer));
    return id;
}

size_t AstTable::addSwitchNode(AstNodeType type) {
    size_t id = this->nodes.size();
    this->nodes.emplace_back(new SwitchAstNode(type, {}, 0));
    return id;
}

AstNode& AstTable::getNode(size_t id) {
    return *this->nodes[id];
}

const AstNode& AstTable::getNode(size_t id) const {
    return *this->nodes[id];
}
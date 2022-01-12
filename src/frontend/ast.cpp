#include "frontend/ast.hpp"

size_t AstTable::addNode(AstNodeType type) {
    size_t id = this->nodes.size();
    this->nodes.push_back({type, {}, 0});
    return id;
}

size_t AstTable::addNode(AstNodeType type, std::initializer_list<size_t> children) {
    size_t id = this->nodes.size();
    this->nodes.push_back({type, children, 0});
    return id;
}

size_t AstTable::addNode(AstNodeType type, const std::vector<size_t>& children) {
    size_t id = this->nodes.size();
    this->nodes.push_back({type, children, 0});
    return id;
}

size_t AstTable::addNode(AstNodeType type, size_t data_type, uint64_t integer) {
    size_t id = this->nodes.size();
    AstNode new_node;
    new_node.type = type;
    new_node.datatype = data_type;
    new_node.integer = integer;
    this->nodes.push_back(new_node);
    return id;
}

AstNode& AstTable::getNode(size_t id) {
    return this->nodes[id];
}

const AstNode& AstTable::getNode(size_t id) const {
    return this->nodes[id];
}
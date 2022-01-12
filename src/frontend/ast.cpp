#include "frontend/ast.hpp"

size_t AstTable::addNode(AstNodeType type) {
    size_t id = this->nodes.size();
    this->nodes.push_back({type});
    return id;
}

size_t AstTable::addNode(AstNodeType type, std::initializer_list<size_t> children) {
    size_t id = this->nodes.size();
    this->nodes.push_back({type, children});
    return id;
}

size_t AstTable::addNode(AstNodeType type, const std::vector<size_t>& children) {
    size_t id = this->nodes.size();
    this->nodes.push_back({type, children});
    return id;
}

AstNode& AstTable::getNode(size_t id) {
    return this->nodes[id];
}

const AstNode& AstTable::getNode(size_t id) const {
    return this->nodes[id];
}
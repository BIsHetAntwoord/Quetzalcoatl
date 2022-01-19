#ifndef _QUETZALCOATL_FRONTEND_AST_HPP
#define _QUETZALCOATL_FRONTEND_AST_HPP

#include <vector>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <memory>

#include "frontend/type.hpp"

const size_t INVALID_ASTNODE_ID = std::numeric_limits<size_t>::max();

enum class AstNodeType {
    INVALID,
    STATEMENT_LIST,

    EMPTY_STAT,
    EXPR_STAT,
    IF_STAT,
    IF_ELSE_STAT,
    SWITCH_STAT,
    WHILE_STAT,
    DO_WHILE_STAT,

    DEFAULT_LABEL,
    CASE_LABEL,

    ADD_EXPR,
    SUB_EXPR,
    MUL_EXPR,
    DIV_EXPR,
    MOD_EXPR,
    LSHIFT_EXPR,
    RSHIFT_EXPR,
    BITWISE_AND_EXPR,
    BITWISE_OR_EXPR,
    BITWISE_XOR_EXPR,
    BITWISE_NOT_EXPR,
    DEREF_EXPR,
    ADDRESS_OF_EXPR,
    PREFIX_INCREMENT_EXPR,
    PREFIX_DECREMENT_EXPR,
    POSTFIX_INCREMENT_EXPR,
    POSTFIX_DECREMENT_EXPR,
    UNARY_PLUS_EXPR,
    UNARY_MINUS_EXPR,
    EQUAL_EXPR,
    NOTEQUAL_EXPR,
    LESS_EXPR,
    GREATER_EXPR,
    LESSEQ_EXPR,
    GREATEREQ_EXPR,
    LOGICAL_AND_EXPR,
    LOGICAL_OR_EXPR,
    LOGICAL_NOT_EXPR,
    CALL_EXPR,
    SUBSCRIPT_EXPR,
    ASSIGN_EXPR,
    ADD_ASSIGN_EXPR,
    SUB_ASSIGN_EXPR,
    MUL_ASSIGN_EXPR,
    DIV_ASSIGN_EXPR,
    MOD_ASSIGN_EXPR,
    LSHIFT_ASSIGN_EXPR,
    RSHIFT_ASSIGN_EXPR,
    BITAND_ASSIGN_EXPR,
    BITOR_ASSIGN_EXPR,
    BITXOR_ASSIGN_EXPR,
    COMMA_EXPR,
    SIZEOF_EXPR,
    THROW_EXPR,
    RETHROW_EXPR,
    TERNARY_EXPR,
    POINTER_TO_MEMBER_EXPR,
    INDIRECT_POINTER_TO_MEMBER_EXPR,

    INTEGER_CONSTANT
};

struct AstNode {
    AstNodeType type;
    std::vector<size_t> children;
    TypeId datatype;

    AstNode(AstNodeType, std::initializer_list<size_t>, TypeId);
    AstNode(AstNodeType, const std::vector<size_t>&, TypeId);
    virtual ~AstNode() = default;
};

struct IntegerAstNode : public AstNode {
    uint64_t integer;

    IntegerAstNode(AstNodeType, std::initializer_list<size_t>, TypeId, uint64_t);
    IntegerAstNode(AstNodeType, const std::vector<size_t>&, TypeId, uint64_t);
};

struct SwitchAstNode : public AstNode {
    size_t default_id;
    std::vector<size_t> case_nodes;

    SwitchAstNode(AstNodeType, std::initializer_list<size_t>, TypeId);
    SwitchAstNode(AstNodeType, const std::vector<size_t>&, TypeId);
};

class AstTable {
private:
    std::vector<std::unique_ptr<AstNode>> nodes;
public:
    size_t addNode(AstNodeType);
    size_t addNode(AstNodeType, std::initializer_list<size_t>);
    size_t addNode(AstNodeType, const std::vector<size_t>&);
    size_t addNode(AstNodeType, TypeId);
    size_t addNode(AstNodeType, TypeId, std::initializer_list<size_t>);
    size_t addNode(AstNodeType, TypeId, const std::vector<size_t>&);
    size_t addIntegerNode(AstNodeType, TypeId, uint64_t);
    size_t addSwitchNode(AstNodeType);

    AstNode& getNode(size_t);
    const AstNode& getNode(size_t) const;
};

#endif

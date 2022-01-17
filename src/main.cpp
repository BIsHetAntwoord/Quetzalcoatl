#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "frontend/filetable.hpp"
#include "frontend/stringtable.hpp"
#include "frontend/ast.hpp"
#include "unicode.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>

void print_tree(AstTable& ast, size_t node, size_t indent = 0) {
    auto print_indent = [&]() {
        for(size_t i = 0; i < indent; ++i) {
            std::cout << "  ";
        }
    };

    AstNode& node_info = ast.getNode(node);

    print_indent();
    std::cout << "Node " << node << ":" << std::endl;
    ++indent;
    print_indent();
    std::cout << "type: " << size_t(node_info.type) << std::endl;
    if (node_info.datatype) {
        print_indent();
        std::cout << "datatype: " << *node_info.datatype << std::endl;
    }
    switch(node_info.type) {
        case AstNodeType::INTEGER_CONSTANT: {
            print_indent();

            IntegerAstNode& integer_node_info = (IntegerAstNode&)node_info;
            std::cout << "integer: " << integer_node_info.integer << std::endl;
            break;
        }
        case AstNodeType::SWITCH_STAT: {
            SwitchAstNode& switch_node_info = (SwitchAstNode&)node_info;
            if(switch_node_info.default_id != INVALID_ASTNODE_ID) {
                print_indent();
                std::cout << "default: " << switch_node_info.default_id << std::endl;
            }

            if(switch_node_info.case_nodes.size() > 0) {
                print_indent();
                std::cout << "cases: ";
                bool first = true;
                for(size_t case_id : switch_node_info.case_nodes) {
                    if(first)
                        first = false;
                    else
                        std::cout << ", ";

                    std::cout << case_id;
                }
                std::cout << std::endl;
            }
            break;
        }
        default:
            break;
    }

    if(node_info.children.size() > 0) {
        print_indent();
        std::cout << "children:" << std::endl;

        for(size_t i : node_info.children) {
            print_tree(ast, i, indent+1);
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2)
        return 1;
    std::ifstream input(argv[1]);
    std::stringstream ss;
    ss << input.rdbuf();

    std::string input_str = ss.str();

    CompileInfo compile_info;
    Lexer lexer(input_str, compile_info);

    AstTable ast;
    Parser parser(lexer, compile_info, ast);

    size_t root_node = parser.parse();
    if(root_node != INVALID_ASTNODE_ID)
        print_tree(ast, root_node);

    compile_info.printDiagnostics(std::cout, true);
    return 0;
}

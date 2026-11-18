#include "ast_printer.hpp"
#include <iomanip>

// ============================================================================
// ASTTreePrinter Implementation
// ============================================================================

void ASTTreePrinter::indent() {
    for (int i = 0; i < indent_level; ++i) {
        out << "  ";
    }
}

std::string ASTTreePrinter::print(ASTNode* root) {
    out.str("");
    out.clear();
    indent_level = 0;
    if (root) {
        root->accept(this);
    } else {
        out << "(null AST)\n";
    }
    return out.str();
}

void ASTTreePrinter::visit(ProgramAST* node) {
    indent();
    out << "ProgramAST (" << node->declarations.size() << " declarations)\n";
    indent_level++;
    for (const auto& decl : node->declarations) {
        decl->accept(this);
    }
    indent_level--;
}

void ASTTreePrinter::visit(VarDeclAST* node) {
    indent();
    out << "VarDecl: " << node->type << " " << node->name;
    if (node->is_array) {
        out << "[" << node->array_size << "]";
    }
    if (node->initializer) {
        out << " medalo\n";
        indent_level++;
        node->initializer->accept(this);
        indent_level--;
    } else {
        out << "\n";
    }
}

void ASTTreePrinter::visit(FuncDeclAST* node) {
    indent();
    out << "FuncDecl: " << node->return_type << " " << node->name << "(";
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        if (i > 0) out << ", ";
        out << node->parameters[i].type << " " << node->parameters[i].name;
        if (node->parameters[i].is_array) {
            out << "[" << node->parameters[i].array_size << "]";
        }
    }
    out << ")\n";
    if (node->body) {
        indent_level++;
        node->body->accept(this);
        indent_level--;
    }
}

void ASTTreePrinter::visit(CompoundStmtAST* node) {
    indent();
    out << "Block (shro ... khatam) [" << node->statements.size() << " statements]\n";
    indent_level++;
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    indent_level--;
}

void ASTTreePrinter::visit(AssignStmtAST* node) {
    indent();
    out << "AssignStmt (medalo)\n";
    indent_level++;
    node->target->accept(this);
    node->expression->accept(this);
    indent_level--;
}

void ASTTreePrinter::visit(IfStmtAST* node) {
    indent();
    out << "IfStmt (agar)\n";
    indent_level++;
    indent();
    out << "Condition:\n";
    indent_level++;
    node->condition->accept(this);
    indent_level--;

    indent();
    out << "Then Branch:\n";
    indent_level++;
    for (const auto& s : node->then_branch) {
        s->accept(this);
    }
    indent_level--;

    if (!node->else_branch.empty()) {
        indent();
        out << "Else Branch (magar):\n";
        indent_level++;
        for (const auto& s : node->else_branch) {
            s->accept(this);
        }
        indent_level--;
    }
    indent_level--;
}

void ASTTreePrinter::visit(WhileStmtAST* node) {
    indent();
    out << "WhileStmt (jabtak)\n";
    indent_level++;
    indent();
    out << "Condition:\n";
    indent_level++;
    node->condition->accept(this);
    indent_level--;

    indent();
    out << "Body:\n";
    indent_level++;
    for (const auto& s : node->body) {
        s->accept(this);
    }
    indent_level--;
    indent_level--;
}

void ASTTreePrinter::visit(BreakStmtAST* /*node*/) {
    indent();
    out << "BreakStmt (nikal)\n";
}

void ASTTreePrinter::visit(ReturnStmtAST* node) {
    indent();
    out << "ReturnStmt (wapis)";
    if (node->value) {
        out << "\n";
        indent_level++;
        node->value->accept(this);
        indent_level--;
    } else {
        out << " [void]\n";
    }
}

void ASTTreePrinter::visit(IOStmtAST* node) {
    indent();
    if (node->is_output) {
        out << "IOStmt: dikhao\n";
        if (node->expression) {
            indent_level++;
            node->expression->accept(this);
            indent_level--;
        }
    } else {
        out << "IOStmt: parho\n";
        if (node->variable) {
            indent_level++;
            node->variable->accept(this);
            indent_level--;
        }
    }
}

void ASTTreePrinter::visit(ExprStmtAST* node) {
    indent();
    out << "ExprStmt:\n";
    indent_level++;
    node->expression->accept(this);
    indent_level--;
}

void ASTTreePrinter::visit(BinaryExprAST* node) {
    indent();
    out << "BinaryExpr '" << node->op << "'\n";
    indent_level++;
    node->left->accept(this);
    node->right->accept(this);
    indent_level--;
}

void ASTTreePrinter::visit(UnaryExprAST* node) {
    indent();
    out << "UnaryExpr '" << node->op << "'\n";
    indent_level++;
    node->operand->accept(this);
    indent_level--;
}

void ASTTreePrinter::visit(LiteralAST* node) {
    indent();
    out << "Literal (" << node->literal_type << "): " << node->value << "\n";
}

void ASTTreePrinter::visit(VarRefAST* node) {
    indent();
    out << "VarRef: " << node->name;
    if (node->index) {
        out << " [indexed]\n";
        indent_level++;
        node->index->accept(this);
        indent_level--;
    } else {
        out << "\n";
    }
}

void ASTTreePrinter::visit(FuncCallAST* node) {
    indent();
    out << "FuncCall: " << node->name << " (" << node->arguments.size() << " args)\n";
    indent_level++;
    for (const auto& arg : node->arguments) {
        arg->accept(this);
    }
    indent_level--;
}

// ============================================================================
// ASTJsonPrinter Implementation
// ============================================================================

void ASTJsonPrinter::indent() {
    for (int i = 0; i < indent_level; ++i) {
        out << "  ";
    }
}

std::string ASTJsonPrinter::escape_json(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '"') result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else if (c == '\b') result += "\\b";
        else if (c == '\f') result += "\\f";
        else if (c == '\n') result += "\\n";
        else if (c == '\r') result += "\\r";
        else if (c == '\t') result += "\\t";
        else result += c;
    }
    return result;
}

std::string ASTJsonPrinter::print(ASTNode* root) {
    out.str("");
    out.clear();
    indent_level = 0;
    if (root) {
        root->accept(this);
    } else {
        out << "null";
    }
    return out.str();
}

void ASTJsonPrinter::visit(ProgramAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"ProgramAST\",\n";
    indent();
    out << "\"declarations\": [\n";
    indent_level++;
    for (size_t i = 0; i < node->declarations.size(); ++i) {
        node->declarations[i]->accept(this);
        if (i + 1 < node->declarations.size()) out << ",";
        out << "\n";
    }
    indent_level--;
    indent();
    out << "]\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(VarDeclAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"VarDeclAST\",\n";
    indent();
    out << "\"name\": \"" << escape_json(node->name) << "\",\n";
    indent();
    out << "\"type\": \"" << escape_json(node->type) << "\",\n";
    indent();
    out << "\"is_array\": " << (node->is_array ? "true" : "false") << ",\n";
    indent();
    out << "\"array_size\": " << node->array_size;
    if (node->initializer) {
        out << ",\n";
        indent();
        out << "\"initializer\":\n";
        indent_level++;
        node->initializer->accept(this);
        indent_level--;
        out << "\n";
    } else {
        out << "\n";
    }
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(FuncDeclAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"FuncDeclAST\",\n";
    indent();
    out << "\"name\": \"" << escape_json(node->name) << "\",\n";
    indent();
    out << "\"return_type\": \"" << escape_json(node->return_type) << "\",\n";
    indent();
    out << "\"parameters\": [\n";
    indent_level++;
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        indent();
        out << "{\"name\": \"" << escape_json(node->parameters[i].name)
            << "\", \"type\": \"" << escape_json(node->parameters[i].type) << "\"";
        if (node->parameters[i].is_array) {
            out << ", \"is_array\": true, \"size\": " << node->parameters[i].array_size;
        }
        out << "}";
        if (i + 1 < node->parameters.size()) out << ",";
        out << "\n";
    }
    indent_level--;
    indent();
    out << "],\n";
    indent();
    out << "\"body\":\n";
    indent_level++;
    if (node->body) node->body->accept(this);
    else out << "null";
    indent_level--;
    out << "\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(CompoundStmtAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"CompoundStmtAST\",\n";
    indent();
    out << "\"statements\": [\n";
    indent_level++;
    for (size_t i = 0; i < node->statements.size(); ++i) {
        node->statements[i]->accept(this);
        if (i + 1 < node->statements.size()) out << ",";
        out << "\n";
    }
    indent_level--;
    indent();
    out << "]\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(AssignStmtAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"AssignStmtAST\",\n";
    indent();
    out << "\"target\":\n";
    indent_level++;
    node->target->accept(this);
    indent_level--;
    out << ",\n";
    indent();
    out << "\"expression\":\n";
    indent_level++;
    node->expression->accept(this);
    indent_level--;
    out << "\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(IfStmtAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"IfStmtAST\",\n";
    indent();
    out << "\"condition\":\n";
    indent_level++;
    node->condition->accept(this);
    indent_level--;
    out << ",\n";

    indent();
    out << "\"then_branch\": [\n";
    indent_level++;
    for (size_t i = 0; i < node->then_branch.size(); ++i) {
        node->then_branch[i]->accept(this);
        if (i + 1 < node->then_branch.size()) out << ",";
        out << "\n";
    }
    indent_level--;
    indent();
    out << "],\n";

    indent();
    out << "\"else_branch\": [\n";
    indent_level++;
    for (size_t i = 0; i < node->else_branch.size(); ++i) {
        node->else_branch[i]->accept(this);
        if (i + 1 < node->else_branch.size()) out << ",";
        out << "\n";
    }
    indent_level--;
    indent();
    out << "]\n";

    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(WhileStmtAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"WhileStmtAST\",\n";
    indent();
    out << "\"condition\":\n";
    indent_level++;
    node->condition->accept(this);
    indent_level--;
    out << ",\n";

    indent();
    out << "\"body\": [\n";
    indent_level++;
    for (size_t i = 0; i < node->body.size(); ++i) {
        node->body[i]->accept(this);
        if (i + 1 < node->body.size()) out << ",";
        out << "\n";
    }
    indent_level--;
    indent();
    out << "]\n";

    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(BreakStmtAST* /*node*/) {
    indent();
    out << "{\"node\": \"BreakStmtAST\"}";
}

void ASTJsonPrinter::visit(ReturnStmtAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"ReturnStmtAST\",\n";
    indent();
    out << "\"value\":\n";
    indent_level++;
    if (node->value) node->value->accept(this);
    else out << "null";
    indent_level--;
    out << "\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(IOStmtAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"IOStmtAST\",\n";
    indent();
    out << "\"action\": \"" << (node->is_output ? "dikhao" : "parho") << "\",\n";
    indent();
    out << "\"target\":\n";
    indent_level++;
    if (node->is_output && node->expression) {
        node->expression->accept(this);
    } else if (!node->is_output && node->variable) {
        node->variable->accept(this);
    } else {
        out << "null";
    }
    indent_level--;
    out << "\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(ExprStmtAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"ExprStmtAST\",\n";
    indent();
    out << "\"expression\":\n";
    indent_level++;
    node->expression->accept(this);
    indent_level--;
    out << "\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(BinaryExprAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"BinaryExprAST\",\n";
    indent();
    out << "\"op\": \"" << escape_json(node->op) << "\",\n";
    indent();
    out << "\"left\":\n";
    indent_level++;
    node->left->accept(this);
    indent_level--;
    out << ",\n";
    indent();
    out << "\"right\":\n";
    indent_level++;
    node->right->accept(this);
    indent_level--;
    out << "\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(UnaryExprAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"UnaryExprAST\",\n";
    indent();
    out << "\"op\": \"" << escape_json(node->op) << "\",\n";
    indent();
    out << "\"operand\":\n";
    indent_level++;
    node->operand->accept(this);
    indent_level--;
    out << "\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(LiteralAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"LiteralAST\",\n";
    indent();
    out << "\"type\": \"" << escape_json(node->literal_type) << "\",\n";
    indent();
    out << "\"value\": \"" << escape_json(node->value) << "\"\n";
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(VarRefAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"VarRefAST\",\n";
    indent();
    out << "\"name\": \"" << escape_json(node->name) << "\"";
    if (node->index) {
        out << ",\n";
        indent();
        out << "\"index\":\n";
        indent_level++;
        node->index->accept(this);
        indent_level--;
        out << "\n";
    } else {
        out << "\n";
    }
    indent_level--;
    indent();
    out << "}";
}

void ASTJsonPrinter::visit(FuncCallAST* node) {
    indent();
    out << "{\n";
    indent_level++;
    indent();
    out << "\"node\": \"FuncCallAST\",\n";
    indent();
    out << "\"name\": \"" << escape_json(node->name) << "\",\n";
    indent();
    out << "\"arguments\": [\n";
    indent_level++;
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        node->arguments[i]->accept(this);
        if (i + 1 < node->arguments.size()) out << ",";
        out << "\n";
    }
    indent_level--;
    indent();
    out << "]\n";
    indent_level--;
    indent();
    out << "}";
}

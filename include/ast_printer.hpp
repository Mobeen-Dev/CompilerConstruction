#ifndef CITU_AST_PRINTER_HPP
#define CITU_AST_PRINTER_HPP

#include <string>
#include <sstream>
#include "ast.hpp"

class ASTTreePrinter : public ASTVisitor {
private:
    std::ostringstream out;
    int indent_level = 0;

    void indent();

public:
    std::string print(ASTNode* root);

    void visit(ProgramAST* node) override;
    void visit(VarDeclAST* node) override;
    void visit(FuncDeclAST* node) override;
    void visit(CompoundStmtAST* node) override;
    void visit(AssignStmtAST* node) override;
    void visit(IfStmtAST* node) override;
    void visit(WhileStmtAST* node) override;
    void visit(BreakStmtAST* node) override;
    void visit(ReturnStmtAST* node) override;
    void visit(IOStmtAST* node) override;
    void visit(ExprStmtAST* node) override;
    void visit(BinaryExprAST* node) override;
    void visit(UnaryExprAST* node) override;
    void visit(LiteralAST* node) override;
    void visit(VarRefAST* node) override;
    void visit(FuncCallAST* node) override;
};

class ASTJsonPrinter : public ASTVisitor {
private:
    std::ostringstream out;
    int indent_level = 0;

    void indent();
    std::string escape_json(const std::string& str);

public:
    std::string print(ASTNode* root);

    void visit(ProgramAST* node) override;
    void visit(VarDeclAST* node) override;
    void visit(FuncDeclAST* node) override;
    void visit(CompoundStmtAST* node) override;
    void visit(AssignStmtAST* node) override;
    void visit(IfStmtAST* node) override;
    void visit(WhileStmtAST* node) override;
    void visit(BreakStmtAST* node) override;
    void visit(ReturnStmtAST* node) override;
    void visit(IOStmtAST* node) override;
    void visit(ExprStmtAST* node) override;
    void visit(BinaryExprAST* node) override;
    void visit(UnaryExprAST* node) override;
    void visit(LiteralAST* node) override;
    void visit(VarRefAST* node) override;
    void visit(FuncCallAST* node) override;
};

#endif // CITU_AST_PRINTER_HPP

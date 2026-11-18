#ifndef CITU_AST_HPP
#define CITU_AST_HPP

#include <string>
#include <vector>
#include <memory>
#include "location.hpp"

// Forward declarations of AST nodes
class ASTVisitor;
class ASTNode;
class ExprNode;
class StmtNode;
class ProgramAST;
class VarDeclAST;
class FuncDeclAST;
class CompoundStmtAST;
class AssignStmtAST;
class IfStmtAST;
class WhileStmtAST;
class BreakStmtAST;
class ReturnStmtAST;
class IOStmtAST;
class ExprStmtAST;
class BinaryExprAST;
class UnaryExprAST;
class LiteralAST;
class VarRefAST;
class FuncCallAST;

// AST Visitor interface
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ProgramAST* node) = 0;
    virtual void visit(VarDeclAST* node) = 0;
    virtual void visit(FuncDeclAST* node) = 0;
    virtual void visit(CompoundStmtAST* node) = 0;
    virtual void visit(AssignStmtAST* node) = 0;
    virtual void visit(IfStmtAST* node) = 0;
    virtual void visit(WhileStmtAST* node) = 0;
    virtual void visit(BreakStmtAST* node) = 0;
    virtual void visit(ReturnStmtAST* node) = 0;
    virtual void visit(IOStmtAST* node) = 0;
    virtual void visit(ExprStmtAST* node) = 0;
    virtual void visit(BinaryExprAST* node) = 0;
    virtual void visit(UnaryExprAST* node) = 0;
    virtual void visit(LiteralAST* node) = 0;
    virtual void visit(VarRefAST* node) = 0;
    virtual void visit(FuncCallAST* node) = 0;
};

// Base AST Node
class ASTNode {
public:
    SourceLocation location;
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
};

// Expression Base Node
class ExprNode : public ASTNode {
public:
    std::string deduced_type = "unknown";
};

// Statement Base Node
class StmtNode : public ASTNode {};

// Parameter representation for function declarations
struct Parameter {
    std::string type;
    std::string name;
    bool is_array = false;
    int array_size = 0;
    SourceLocation location;
};

// Program AST (Root)
class ProgramAST : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> declarations;

    ProgramAST() = default;
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Variable Declaration: Type ID [ [size] ] [ medalo expr ] ;
class VarDeclAST : public StmtNode {
public:
    std::string type;
    std::string name;
    bool is_array = false;
    int array_size = 0;
    std::unique_ptr<ExprNode> initializer;

    VarDeclAST(std::string t, std::string n, bool is_arr, int sz, std::unique_ptr<ExprNode> init, SourceLocation loc)
        : type(std::move(t)), name(std::move(n)), is_array(is_arr), array_size(sz), initializer(std::move(init)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Compound Statement (Block): shro ... khatam
class CompoundStmtAST : public StmtNode {
public:
    std::vector<std::unique_ptr<StmtNode>> statements;

    CompoundStmtAST(SourceLocation loc) { location = loc; }
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Function Declaration: Type ID ( params ) CompoundStmt
class FuncDeclAST : public ASTNode {
public:
    std::string return_type;
    std::string name;
    std::vector<Parameter> parameters;
    std::unique_ptr<CompoundStmtAST> body;

    FuncDeclAST(std::string ret_t, std::string n, std::vector<Parameter> params, std::unique_ptr<CompoundStmtAST> b, SourceLocation loc)
        : return_type(std::move(ret_t)), name(std::move(n)), parameters(std::move(params)), body(std::move(b)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Variable Reference: identifier or array element id[expr]
class VarRefAST : public ExprNode {
public:
    std::string name;
    std::unique_ptr<ExprNode> index; // nullptr if not indexed

    VarRefAST(std::string n, std::unique_ptr<ExprNode> idx, SourceLocation loc)
        : name(std::move(n)), index(std::move(idx)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Assignment Statement: variable medalo expression ;
class AssignStmtAST : public StmtNode {
public:
    std::unique_ptr<VarRefAST> target;
    std::unique_ptr<ExprNode> expression;

    AssignStmtAST(std::unique_ptr<VarRefAST> tgt, std::unique_ptr<ExprNode> expr, SourceLocation loc)
        : target(std::move(tgt)), expression(std::move(expr)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Selection Statement: agar (cond) shro ... [magar ...] khatam
class IfStmtAST : public StmtNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::vector<std::unique_ptr<StmtNode>> then_branch;
    std::vector<std::unique_ptr<StmtNode>> else_branch;

    IfStmtAST(std::unique_ptr<ExprNode> cond,
              std::vector<std::unique_ptr<StmtNode>> then_b,
              std::vector<std::unique_ptr<StmtNode>> else_b,
              SourceLocation loc)
        : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Iteration Statement: jabtak (cond) shro ... khatam
class WhileStmtAST : public StmtNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::vector<std::unique_ptr<StmtNode>> body;

    WhileStmtAST(std::unique_ptr<ExprNode> cond, std::vector<std::unique_ptr<StmtNode>> b, SourceLocation loc)
        : condition(std::move(cond)), body(std::move(b)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Break Statement: nikal ;
class BreakStmtAST : public StmtNode {
public:
    BreakStmtAST(SourceLocation loc) { location = loc; }
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Return Statement: wapis [expr] ;
class ReturnStmtAST : public StmtNode {
public:
    std::unique_ptr<ExprNode> value; // nullptr if void return

    ReturnStmtAST(std::unique_ptr<ExprNode> val, SourceLocation loc)
        : value(std::move(val)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// I/O Statement: dikhao(expr) ; or parho(var) ;
class IOStmtAST : public StmtNode {
public:
    bool is_output; // true = dikhao, false = parho
    std::unique_ptr<ExprNode> expression; // For dikhao
    std::unique_ptr<VarRefAST> variable;   // For parho

    IOStmtAST(bool is_out, std::unique_ptr<ExprNode> expr, std::unique_ptr<VarRefAST> var, SourceLocation loc)
        : is_output(is_out), expression(std::move(expr)), variable(std::move(var)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Expression Statement: expr ;
class ExprStmtAST : public StmtNode {
public:
    std::unique_ptr<ExprNode> expression;

    ExprStmtAST(std::unique_ptr<ExprNode> expr, SourceLocation loc)
        : expression(std::move(expr)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Binary Expression: left OP right
class BinaryExprAST : public ExprNode {
public:
    std::string op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;

    BinaryExprAST(std::string o, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r, SourceLocation loc)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Unary Expression: OP operand
class UnaryExprAST : public ExprNode {
public:
    std::string op;
    std::unique_ptr<ExprNode> operand;

    UnaryExprAST(std::string o, std::unique_ptr<ExprNode> opnd, SourceLocation loc)
        : op(std::move(o)), operand(std::move(opnd)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Literal Node: integer, float, char, string
class LiteralAST : public ExprNode {
public:
    std::string literal_type; // "int", "float", "char", "string"
    std::string value;

    LiteralAST(std::string type, std::string val, SourceLocation loc)
        : literal_type(std::move(type)), value(std::move(val)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

// Function Call Node: func(arg1, arg2, ...)
class FuncCallAST : public ExprNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ExprNode>> arguments;

    FuncCallAST(std::string n, std::vector<std::unique_ptr<ExprNode>> args, SourceLocation loc)
        : name(std::move(n)), arguments(std::move(args)) {
        location = loc;
    }

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

#endif // CITU_AST_HPP

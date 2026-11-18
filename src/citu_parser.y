%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include "ast.hpp"
#include "location.hpp"
#include "error_reporter.hpp"

extern int yylex();
extern int line_num;
extern int col_num;
extern char* yytext;
extern FILE* yyin;
extern std::string g_current_filename;

void yyerror(const char* s);

SourceLocation current_loc() {
    SourceLocation loc;
    loc.filename = g_current_filename;
    loc.first_line = line_num;
    loc.first_column = col_num;
    loc.last_line = line_num;
    loc.last_column = col_num;
    return loc;
}

// Global AST root
ProgramAST* g_program_ast = nullptr;
%}

%union {
    int int_val;
    double float_val;
    char char_val;
    char* str_val;
    ASTNode* ast_node;
    ExprNode* expr_node;
    StmtNode* stmt_node;
    VarRefAST* var_ref_node;
    CompoundStmtAST* block_node;
    std::vector<std::unique_ptr<ASTNode>>* decl_list;
    std::vector<std::unique_ptr<StmtNode>>* stmt_list;
    std::vector<std::unique_ptr<ExprNode>>* expr_list;
    std::vector<Parameter>* param_list;
    Parameter* param;
}

/* Tokens */
%token TOKEN_JABTAK TOKEN_SHRO TOKEN_KHATAM
%token TOKEN_AGAR TOKEN_MAGAR TOKEN_NIKAL
%token TOKEN_MEDALO TOKEN_WAPIS TOKEN_DIKHAO TOKEN_PARHO

%token TOKEN_INT16 TOKEN_INT32 TOKEN_UINT16 TOKEN_UINT32
%token TOKEN_REAL TOKEN_FLOAT TOKEN_CHAR TOKEN_VOID

%token TOKEN_PLUS TOKEN_MINUS TOKEN_STAR TOKEN_SLASH TOKEN_PERCENT
%token TOKEN_EQ TOKEN_NEQ TOKEN_LT TOKEN_LTE TOKEN_GT TOKEN_GTE
%token TOKEN_AND TOKEN_OR TOKEN_NOT

%token TOKEN_LPAREN TOKEN_RPAREN TOKEN_LBRACKET TOKEN_RBRACKET
%token TOKEN_SEMICOLON TOKEN_COMMA

%token <int_val> TOKEN_INT_LITERAL
%token <float_val> TOKEN_FLOAT_LITERAL
%token <char_val> TOKEN_CHAR_LITERAL
%token <str_val> TOKEN_STRING_LITERAL TOKEN_IDENTIFIER

/* Non-terminal types */
%type <decl_list> declaration_list
%type <ast_node> declaration func_declaration
%type <stmt_node> var_declaration statement compound_statement selection_statement iteration_statement assignment_statement
%type <stmt_node> jump_statement return_statement io_statement expr_statement
%type <stmt_list> statement_list
%type <str_val> type_specifier
%type <param_list> parameter_list parameter_list_nonempty
%type <param> parameter
%type <var_ref_node> variable
%type <expr_node> expression func_call literal
%type <expr_list> argument_list argument_list_nonempty

/* Precedence and Associativity */
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_EQ TOKEN_NEQ
%left TOKEN_LT TOKEN_LTE TOKEN_GT TOKEN_GTE
%left TOKEN_PLUS TOKEN_MINUS
%left TOKEN_STAR TOKEN_SLASH TOKEN_PERCENT
%right TOKEN_NOT UMINUS UPLUS

%start program

%%

program
    : declaration_list
        {
            auto prog = std::make_unique<ProgramAST>();
            if ($1) {
                prog->declarations = std::move(*$1);
                delete $1;
            }
            g_program_ast = prog.release();
        }
    ;

declaration_list
    : declaration_list declaration
        {
            $$ = $1 ? $1 : new std::vector<std::unique_ptr<ASTNode>>();
            if ($2) {
                $$->push_back(std::unique_ptr<ASTNode>($2));
            }
        }
    | /* empty */
        {
            $$ = new std::vector<std::unique_ptr<ASTNode>>();
        }
    ;

declaration
    : var_declaration
        { $$ = $1; }
    | func_declaration
        { $$ = $1; }
    ;

type_specifier
    : TOKEN_INT16   { $$ = strdup("int16"); }
    | TOKEN_INT32   { $$ = strdup("int32"); }
    | TOKEN_UINT16  { $$ = strdup("uint16"); }
    | TOKEN_UINT32  { $$ = strdup("uint32"); }
    | TOKEN_REAL    { $$ = strdup("real"); }
    | TOKEN_FLOAT   { $$ = strdup("float"); }
    | TOKEN_CHAR    { $$ = strdup("char"); }
    | TOKEN_VOID    { $$ = strdup("void"); }
    ;

var_declaration
    : type_specifier TOKEN_IDENTIFIER TOKEN_SEMICOLON
        {
            SourceLocation loc = current_loc();
            $$ = new VarDeclAST($1, $2, false, 0, nullptr, loc);
            free($1); free($2);
        }
    | type_specifier TOKEN_IDENTIFIER TOKEN_LBRACKET TOKEN_INT_LITERAL TOKEN_RBRACKET TOKEN_SEMICOLON
        {
            SourceLocation loc = current_loc();
            $$ = new VarDeclAST($1, $2, true, $4, nullptr, loc);
            free($1); free($2);
        }
    | type_specifier TOKEN_IDENTIFIER TOKEN_MEDALO expression TOKEN_SEMICOLON
        {
            SourceLocation loc = current_loc();
            $$ = new VarDeclAST($1, $2, false, 0, std::unique_ptr<ExprNode>($4), loc);
            free($1); free($2);
        }
    ;

func_declaration
    : type_specifier TOKEN_IDENTIFIER TOKEN_LPAREN parameter_list TOKEN_RPAREN compound_statement
        {
            SourceLocation loc = current_loc();
            std::vector<Parameter> params;
            if ($4) {
                params = std::move(*$4);
                delete $4;
            }
            CompoundStmtAST* body_block = dynamic_cast<CompoundStmtAST*>($6);
            $$ = new FuncDeclAST($1, $2, std::move(params), std::unique_ptr<CompoundStmtAST>(body_block), loc);
            free($1); free($2);
        }
    ;

parameter_list
    : parameter_list_nonempty
        { $$ = $1; }
    | /* empty */
        { $$ = new std::vector<Parameter>(); }
    ;

parameter_list_nonempty
    : parameter_list_nonempty TOKEN_COMMA parameter
        {
            $$ = $1;
            $$->push_back(*$3);
            delete $3;
        }
    | parameter
        {
            $$ = new std::vector<Parameter>();
            $$->push_back(*$1);
            delete $1;
        }
    ;

parameter
    : type_specifier TOKEN_IDENTIFIER
        {
            $$ = new Parameter{$1, $2, false, 0, current_loc()};
            free($1); free($2);
        }
    | type_specifier TOKEN_IDENTIFIER TOKEN_LBRACKET TOKEN_INT_LITERAL TOKEN_RBRACKET
        {
            $$ = new Parameter{$1, $2, true, $4, current_loc()};
            free($1); free($2);
        }
    | type_specifier TOKEN_IDENTIFIER TOKEN_LBRACKET TOKEN_RBRACKET
        {
            $$ = new Parameter{$1, $2, true, 0, current_loc()};
            free($1); free($2);
        }
    ;

compound_statement
    : TOKEN_SHRO statement_list TOKEN_KHATAM
        {
            SourceLocation loc = current_loc();
            auto block = new CompoundStmtAST(loc);
            if ($2) {
                block->statements = std::move(*$2);
                delete $2;
            }
            $$ = block;
        }
    ;

statement_list
    : statement_list statement
        {
            $$ = $1 ? $1 : new std::vector<std::unique_ptr<StmtNode>>();
            if ($2) {
                $$->push_back(std::unique_ptr<StmtNode>($2));
            }
        }
    | /* empty */
        {
            $$ = new std::vector<std::unique_ptr<StmtNode>>();
        }
    ;

statement
    : var_declaration
        { $$ = $1; }
    | assignment_statement
        { $$ = $1; }
    | selection_statement
        { $$ = $1; }
    | iteration_statement
        { $$ = $1; }
    | jump_statement
        { $$ = $1; }
    | return_statement
        { $$ = $1; }
    | io_statement
        { $$ = $1; }
    | expr_statement
        { $$ = $1; }
    | compound_statement
        { $$ = $1; }
    | error TOKEN_SEMICOLON
        {
            $$ = nullptr; // Error recovered after semicolon
        }
    ;

assignment_statement
    : variable TOKEN_MEDALO expression TOKEN_SEMICOLON
        {
            SourceLocation loc = current_loc();
            $$ = new AssignStmtAST(std::unique_ptr<VarRefAST>($1), std::unique_ptr<ExprNode>($3), loc);
        }
    ;

selection_statement
    : TOKEN_AGAR TOKEN_LPAREN expression TOKEN_RPAREN TOKEN_SHRO statement_list TOKEN_KHATAM
        {
            SourceLocation loc = current_loc();
            std::vector<std::unique_ptr<StmtNode>> then_b;
            if ($6) { then_b = std::move(*$6); delete $6; }
            $$ = new IfStmtAST(std::unique_ptr<ExprNode>($3), std::move(then_b), {}, loc);
        }
    | TOKEN_AGAR TOKEN_LPAREN expression TOKEN_RPAREN TOKEN_SHRO statement_list TOKEN_MAGAR statement_list TOKEN_KHATAM
        {
            SourceLocation loc = current_loc();
            std::vector<std::unique_ptr<StmtNode>> then_b;
            if ($6) { then_b = std::move(*$6); delete $6; }
            std::vector<std::unique_ptr<StmtNode>> else_b;
            if ($8) { else_b = std::move(*$8); delete $8; }
            $$ = new IfStmtAST(std::unique_ptr<ExprNode>($3), std::move(then_b), std::move(else_b), loc);
        }
    ;

iteration_statement
    : TOKEN_JABTAK TOKEN_LPAREN expression TOKEN_RPAREN TOKEN_SHRO statement_list TOKEN_KHATAM
        {
            SourceLocation loc = current_loc();
            std::vector<std::unique_ptr<StmtNode>> body;
            if ($6) { body = std::move(*$6); delete $6; }
            $$ = new WhileStmtAST(std::unique_ptr<ExprNode>($3), std::move(body), loc);
        }
    ;

jump_statement
    : TOKEN_NIKAL TOKEN_SEMICOLON
        {
            $$ = new BreakStmtAST(current_loc());
        }
    ;

return_statement
    : TOKEN_WAPIS expression TOKEN_SEMICOLON
        {
            $$ = new ReturnStmtAST(std::unique_ptr<ExprNode>($2), current_loc());
        }
    | TOKEN_WAPIS TOKEN_SEMICOLON
        {
            $$ = new ReturnStmtAST(nullptr, current_loc());
        }
    ;

io_statement
    : TOKEN_DIKHAO TOKEN_LPAREN expression TOKEN_RPAREN TOKEN_SEMICOLON
        {
            $$ = new IOStmtAST(true, std::unique_ptr<ExprNode>($3), nullptr, current_loc());
        }
    | TOKEN_PARHO TOKEN_LPAREN variable TOKEN_RPAREN TOKEN_SEMICOLON
        {
            $$ = new IOStmtAST(false, nullptr, std::unique_ptr<VarRefAST>($3), current_loc());
        }
    ;

expr_statement
    : expression TOKEN_SEMICOLON
        {
            $$ = new ExprStmtAST(std::unique_ptr<ExprNode>($1), current_loc());
        }
    | TOKEN_SEMICOLON
        {
            $$ = nullptr;
        }
    ;

expression
    : expression TOKEN_OR expression
        { $$ = new BinaryExprAST("||", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_AND expression
        { $$ = new BinaryExprAST("&&", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_EQ expression
        { $$ = new BinaryExprAST("==", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_NEQ expression
        { $$ = new BinaryExprAST("!=", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_LT expression
        { $$ = new BinaryExprAST("<", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_LTE expression
        { $$ = new BinaryExprAST("<=", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_GT expression
        { $$ = new BinaryExprAST(">", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_GTE expression
        { $$ = new BinaryExprAST(">=", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_PLUS expression
        { $$ = new BinaryExprAST("+", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_MINUS expression
        { $$ = new BinaryExprAST("-", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_STAR expression
        { $$ = new BinaryExprAST("*", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_SLASH expression
        { $$ = new BinaryExprAST("/", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | expression TOKEN_PERCENT expression
        { $$ = new BinaryExprAST("%", std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3), current_loc()); }
    | TOKEN_PLUS expression %prec UPLUS
        { $$ = new UnaryExprAST("+", std::unique_ptr<ExprNode>($2), current_loc()); }
    | TOKEN_MINUS expression %prec UMINUS
        { $$ = new UnaryExprAST("-", std::unique_ptr<ExprNode>($2), current_loc()); }
    | TOKEN_NOT expression
        { $$ = new UnaryExprAST("!", std::unique_ptr<ExprNode>($2), current_loc()); }
    | TOKEN_LPAREN expression TOKEN_RPAREN
        { $$ = $2; }
    | variable
        { $$ = $1; }
    | func_call
        { $$ = $1; }
    | literal
        { $$ = $1; }
    ;

variable
    : TOKEN_IDENTIFIER
        {
            $$ = new VarRefAST($1, nullptr, current_loc());
            free($1);
        }
    | TOKEN_IDENTIFIER TOKEN_LBRACKET expression TOKEN_RBRACKET
        {
            $$ = new VarRefAST($1, std::unique_ptr<ExprNode>($3), current_loc());
            free($1);
        }
    ;

func_call
    : TOKEN_IDENTIFIER TOKEN_LPAREN argument_list TOKEN_RPAREN
        {
            std::vector<std::unique_ptr<ExprNode>> args;
            if ($3) { args = std::move(*$3); delete $3; }
            $$ = new FuncCallAST($1, std::move(args), current_loc());
            free($1);
        }
    ;

argument_list
    : argument_list_nonempty
        { $$ = $1; }
    | /* empty */
        { $$ = new std::vector<std::unique_ptr<ExprNode>>(); }
    ;

argument_list_nonempty
    : argument_list_nonempty TOKEN_COMMA expression
        {
            $$ = $1;
            $$->push_back(std::unique_ptr<ExprNode>($3));
        }
    | expression
        {
            $$ = new std::vector<std::unique_ptr<ExprNode>>();
            $$->push_back(std::unique_ptr<ExprNode>($1));
        }
    ;

literal
    : TOKEN_INT_LITERAL
        { $$ = new LiteralAST("int", std::to_string($1), current_loc()); }
    | TOKEN_FLOAT_LITERAL
        { $$ = new LiteralAST("float", std::to_string($1), current_loc()); }
    | TOKEN_CHAR_LITERAL
        { $$ = new LiteralAST("char", std::string(1, $1), current_loc()); }
    | TOKEN_STRING_LITERAL
        { $$ = new LiteralAST("string", $1, current_loc()); free($1); }
    ;

%%

void yyerror(const char* s) {
    SourceLocation loc = current_loc();
    ErrorReporter::instance().error(std::string(s) + " near '" + (yytext ? yytext : "") + "'", loc);
}

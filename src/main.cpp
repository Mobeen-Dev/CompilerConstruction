#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ast.hpp"
#include "ast_printer.hpp"
#include "error_reporter.hpp"
#include "citu_parser.hpp"

extern FILE* yyin;
extern int yylex();
extern int yyparse();
extern int line_num;
extern int col_num;
extern int yyleng;
extern char* yytext;
extern void reset_lexer();
extern ProgramAST* g_program_ast;

std::string g_current_filename;

const char* token_id_to_name(int id) {
    switch (id) {
        case TOKEN_JABTAK:         return "TOKEN_JABTAK";
        case TOKEN_SHRO:           return "TOKEN_SHRO";
        case TOKEN_KHATAM:         return "TOKEN_KHATAM";
        case TOKEN_AGAR:           return "TOKEN_AGAR";
        case TOKEN_MAGAR:          return "TOKEN_MAGAR";
        case TOKEN_NIKAL:          return "TOKEN_NIKAL";
        case TOKEN_MEDALO:         return "TOKEN_MEDALO";
        case TOKEN_WAPIS:          return "TOKEN_WAPIS";
        case TOKEN_DIKHAO:         return "TOKEN_DIKHAO";
        case TOKEN_PARHO:          return "TOKEN_PARHO";
        case TOKEN_INT16:          return "TOKEN_INT16";
        case TOKEN_INT32:          return "TOKEN_INT32";
        case TOKEN_UINT16:         return "TOKEN_UINT16";
        case TOKEN_UINT32:         return "TOKEN_UINT32";
        case TOKEN_REAL:           return "TOKEN_REAL";
        case TOKEN_FLOAT:          return "TOKEN_FLOAT";
        case TOKEN_CHAR:           return "TOKEN_CHAR";
        case TOKEN_VOID:           return "TOKEN_VOID";
        case TOKEN_PLUS:           return "TOKEN_PLUS";
        case TOKEN_MINUS:          return "TOKEN_MINUS";
        case TOKEN_STAR:           return "TOKEN_STAR";
        case TOKEN_SLASH:          return "TOKEN_SLASH";
        case TOKEN_PERCENT:        return "TOKEN_PERCENT";
        case TOKEN_EQ:             return "TOKEN_EQ";
        case TOKEN_NEQ:            return "TOKEN_NEQ";
        case TOKEN_LT:             return "TOKEN_LT";
        case TOKEN_LTE:            return "TOKEN_LTE";
        case TOKEN_GT:             return "TOKEN_GT";
        case TOKEN_GTE:            return "TOKEN_GTE";
        case TOKEN_AND:            return "TOKEN_AND";
        case TOKEN_OR:             return "TOKEN_OR";
        case TOKEN_NOT:            return "TOKEN_NOT";
        case TOKEN_LPAREN:         return "TOKEN_LPAREN";
        case TOKEN_RPAREN:         return "TOKEN_RPAREN";
        case TOKEN_LBRACKET:       return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET:       return "TOKEN_RBRACKET";
        case TOKEN_SEMICOLON:      return "TOKEN_SEMICOLON";
        case TOKEN_COMMA:          return "TOKEN_COMMA";
        case TOKEN_INT_LITERAL:    return "TOKEN_INT_LITERAL";
        case TOKEN_FLOAT_LITERAL:  return "TOKEN_FLOAT_LITERAL";
        case TOKEN_CHAR_LITERAL:   return "TOKEN_CHAR_LITERAL";
        case TOKEN_STRING_LITERAL: return "TOKEN_STRING_LITERAL";
        case TOKEN_IDENTIFIER:     return "TOKEN_IDENTIFIER";
        default:                   return "TOKEN_UNKNOWN";
    }
}

void print_help(const char* prog) {
    std::cout << "Usage: " << prog << " [options] <source.citu>\n\n"
              << "Options:\n"
              << "  --tokens                 Run lexer and emit token stream (to stdout or -o file)\n"
              << "  --ast                    Run parser and emit AST (to stdout or -o file)\n"
              << "  --dump-tokens            Alias for --tokens\n"
              << "  --dump-ast               Alias for --ast\n"
              << "  --json                   Format AST output as standard JSON\n"
              << "  -o, --output <file>      Specify destination output file\n"
              << "  -v, --verbose            Verbose diagnostic logging\n"
              << "  -h, --help               Display this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    std::string source_file;
    std::string output_file;
    bool output_json = false;
    bool run_tokens_only = false;
    bool run_ast_output = false;
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            print_help(argv[0]);
            return 0;
        } else if (arg == "--tokens" || arg == "--dump-tokens") {
            run_tokens_only = true;
        } else if (arg == "--ast" || arg == "--dump-ast") {
            run_ast_output = true;
        } else if (arg == "--json") {
            output_json = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                output_file = argv[++i];
            }
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg[0] != '-') {
            source_file = arg;
        } else {
            std::cerr << "Error: Unknown command line option '" << arg << "'\n";
            return 1;
        }
    }

    if (source_file.empty()) {
        std::cerr << "Error: No input .citu source file specified.\n";
        return 1;
    }

    // Read full source content for diagnostics
    std::ifstream src_stream(source_file);
    if (!src_stream.is_open()) {
        std::cerr << "Error: Could not open source file '" << source_file << "'\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << src_stream.rdbuf();
    std::string file_content = buffer.str();
    src_stream.close();

    g_current_filename = source_file;
    ErrorReporter::instance().set_file(source_file, file_content);

    FILE* fp = fopen(source_file.c_str(), "r");
    if (!fp) {
        std::cerr << "Error: Cannot open file '" << source_file << "' for reading.\n";
        return 1;
    }

    yyin = fp;
    reset_lexer();

    // Mode 1: Tokens only
    if (run_tokens_only) {
        std::ostringstream token_out;
        int tok;
        while ((tok = yylex()) != 0) {
            std::string loc_str = std::to_string(line_num) + ":" + std::to_string(col_num - yyleng);
            std::string tok_name = token_id_to_name(tok);

            token_out << loc_str;
            if (loc_str.length() < 8) token_out << "\t";
            token_out << "\t" << tok_name;
            if (tok_name.length() < 20) token_out << "\t";
            token_out << "\t\"" << yytext << "\"";

            if (tok == TOKEN_INT_LITERAL || tok == TOKEN_FLOAT_LITERAL) {
                token_out << "\t\t" << yytext;
            }
            token_out << "\n";
        }

        fclose(fp);

        if (ErrorReporter::instance().has_errors()) {
            ErrorReporter::instance().print_diagnostics();
            return 1;
        }

        if (!output_file.empty()) {
            std::ofstream out_f(output_file);
            if (!out_f.is_open()) {
                std::cerr << "Error: Could not open output file '" << output_file << "' for writing.\n";
                return 1;
            }
            out_f << token_out.str();
            out_f.close();
            if (verbose) {
                std::cout << "[Citu Lexer] Token stream written to: " << output_file << "\n";
            }
        } else {
            std::cout << token_out.str();
        }
        return 0;
    }

    // Mode 2: Parse and build AST
    int parse_result = yyparse();
    fclose(fp);

    if (parse_result != 0 || ErrorReporter::instance().has_errors() || !g_program_ast) {
        ErrorReporter::instance().print_diagnostics();
        std::cerr << "\033[1;31m[Citu Compiler] Compilation failed with "
                  << ErrorReporter::instance().get_error_count() << " error(s).\033[0m\n";
        return 1;
    }

    std::string ast_representation;
    if (output_json) {
        ASTJsonPrinter json_printer;
        ast_representation = json_printer.print(g_program_ast);
    } else {
        ASTTreePrinter tree_printer;
        ast_representation = tree_printer.print(g_program_ast);
    }

    if (!output_file.empty()) {
        std::ofstream out_f(output_file);
        if (!out_f.is_open()) {
            std::cerr << "Error: Could not open AST output file '" << output_file << "' for writing.\n";
            return 1;
        }
        out_f << ast_representation << "\n";
        out_f.close();
        if (verbose) {
            std::cout << "[Citu Parser] AST representation written to: " << output_file << "\n";
        }
    } else if (run_ast_output) {
        std::cout << ast_representation << "\n";
    } else {
        std::cout << "\033[1;32m[Citu Compiler] Success: '" << source_file << "' parsed without errors.\033[0m\n";
    }

    delete g_program_ast;
    return 0;
}

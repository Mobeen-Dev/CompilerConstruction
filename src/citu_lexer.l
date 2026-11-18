%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include "ast.hpp"
#include "location.hpp"
#include "error_reporter.hpp"
#include "citu_parser.hpp"

int line_num = 1;
int col_num = 1;

static void update_loc() {
    col_num += yyleng;
}

static void update_loc_newline() {
    line_num++;
    col_num = 1;
}

static void update_loc_multiline_comment(const char* text) {
    for (int i = 0; text[i] != '\0'; ++i) {
        if (text[i] == '\n') {
            line_num++;
            col_num = 1;
        } else {
            col_num++;
        }
    }
}
%}

%option noyywrap
%option yylineno

DIGIT       [0-9]
HEX_DIGIT   [0-9a-fA-F]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INT_CONST   {DIGIT}+
HEX_CONST   0[xX]{HEX_DIGIT}+
FLOAT_CONST {DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?
STRING      \"([^\"\\]|\\.)*\"
CHAR        \'([^\'\\]|\\.)\'
WS          [ \t\r]+
NEWLINE     \n

%%

"jabtak"        { update_loc(); return TOKEN_JABTAK; }
"shro"          { update_loc(); return TOKEN_SHRO; }
"khatam"        { update_loc(); return TOKEN_KHATAM; }
"agar"          { update_loc(); return TOKEN_AGAR; }
"magar"         { update_loc(); return TOKEN_MAGAR; }
"nikal"         { update_loc(); return TOKEN_NIKAL; }
"medalo"        { update_loc(); return TOKEN_MEDALO; }
"wapis"         { update_loc(); return TOKEN_WAPIS; }
"dikhao"        { update_loc(); return TOKEN_DIKHAO; }
"parho"         { update_loc(); return TOKEN_PARHO; }

"int16"         { update_loc(); return TOKEN_INT16; }
"int32"         { update_loc(); return TOKEN_INT32; }
"uint16"        { update_loc(); return TOKEN_UINT16; }
"uint32"        { update_loc(); return TOKEN_UINT32; }
"real"          { update_loc(); return TOKEN_REAL; }
"float"         { update_loc(); return TOKEN_FLOAT; }
"char"          { update_loc(); return TOKEN_CHAR; }
"void"          { update_loc(); return TOKEN_VOID; }

"="             { update_loc(); return TOKEN_EQ; }
"=="            { update_loc(); return TOKEN_EQ; }
"!="            { update_loc(); return TOKEN_NEQ; }
"<="            { update_loc(); return TOKEN_LTE; }
">="            { update_loc(); return TOKEN_GTE; }
"<"             { update_loc(); return TOKEN_LT; }
">"             { update_loc(); return TOKEN_GT; }

"+"             { update_loc(); return TOKEN_PLUS; }
"-"             { update_loc(); return TOKEN_MINUS; }
"*"             { update_loc(); return TOKEN_STAR; }
"/"             { update_loc(); return TOKEN_SLASH; }
"%"             { update_loc(); return TOKEN_PERCENT; }

"&&"            { update_loc(); return TOKEN_AND; }
"||"            { update_loc(); return TOKEN_OR; }
"!"             { update_loc(); return TOKEN_NOT; }

"("             { update_loc(); return TOKEN_LPAREN; }
")"             { update_loc(); return TOKEN_RPAREN; }
"["             { update_loc(); return TOKEN_LBRACKET; }
"]"             { update_loc(); return TOKEN_RBRACKET; }
";"             { update_loc(); return TOKEN_SEMICOLON; }
","             { update_loc(); return TOKEN_COMMA; }

{FLOAT_CONST}   {
    update_loc();
    yylval.float_val = atof(yytext);
    return TOKEN_FLOAT_LITERAL;
}

{HEX_CONST}     {
    update_loc();
    yylval.int_val = (int)strtol(yytext, NULL, 16);
    return TOKEN_INT_LITERAL;
}

{INT_CONST}     {
    update_loc();
    yylval.int_val = atoi(yytext);
    return TOKEN_INT_LITERAL;
}

{CHAR}          {
    update_loc();
    char c = yytext[1];
    if (yytext[1] == '\\') {
        switch (yytext[2]) {
            case 'n': c = '\n'; break;
            case 't': c = '\t'; break;
            case 'r': c = '\r'; break;
            case '\\': c = '\\'; break;
            case '\'': c = '\''; break;
            default: c = yytext[2]; break;
        }
    }
    yylval.char_val = c;
    return TOKEN_CHAR_LITERAL;
}

{STRING}        {
    update_loc();
    // Strip leading and trailing quotes
    std::string s(yytext + 1, yyleng - 2);
    yylval.str_val = strdup(s.c_str());
    return TOKEN_STRING_LITERAL;
}

{ID}            {
    update_loc();
    yylval.str_val = strdup(yytext);
    return TOKEN_IDENTIFIER;
}

"//"[^\n]*      {
    update_loc();
    /* Discard single-line comment */
}

"/*"([^*]|\*+[^/*])*\*+"/" {
    update_loc_multiline_comment(yytext);
    /* Discard multiline comment */
}

{WS}            {
    update_loc();
    /* Skip whitespace */
}

{NEWLINE}       {
    update_loc_newline();
    /* Skip newline */
}

.               {
    extern std::string g_current_filename;
    SourceLocation loc;
    loc.filename = g_current_filename;
    loc.first_line = line_num;
    loc.first_column = col_num;
    loc.last_line = line_num;
    loc.last_column = col_num;
    ErrorReporter::instance().error(std::string("Unrecognized lexical token '") + yytext + "'", loc);
    update_loc();
}

%%

void reset_lexer() {
    line_num = 1;
    col_num = 1;
    if (YY_CURRENT_BUFFER) {
        yy_flush_buffer(YY_CURRENT_BUFFER);
    }
}

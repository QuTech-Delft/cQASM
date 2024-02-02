%define api.pure full
%define api.prefix {cqasm_version_}
%locations

%code requires {
    /**
     * \file
     * Parser, generated by Bison.
     */
    #include <memory>
    #include <cstdio>
    #include <cstdint>
    #include <string>
    #include "cqasm-error.hpp"
    #include "cqasm-version.hpp"
    using namespace cqasm::version;
    typedef void* yyscan_t;

    #define YYSTYPE CQASM_VERSION_STYPE
    #define YYLTYPE CQASM_VERSION_LTYPE

}

%code {
    int yylex(YYSTYPE* yylvalp, YYLTYPE* yyllocp, yyscan_t scanner);
    void yyerror(YYLTYPE* yyllocp, yyscan_t scanner, const std::string &filename, cqasm::version::Version &version, const char* msg);
}

%param { yyscan_t scanner }
%parse-param { const std::string &filename }
%parse-param { cqasm::version::Version &version }

/* YYSTYPE union */
%union {
    char            *str;
    Version         *vers;
};

/* Typenames for nonterminals */
%type <vers> Version Root

/* Version statement */
%token VERSION

/* Version statement */
%token <str> INT_LITERAL

/* Error marker tokens */
%token UNKNOWN END_OF_FILE

%start Root

%%

Version : INT_LITERAL               { version.push_back(std::atoll($1)); std::free($1); }
        | Version '.' INT_LITERAL   { version.push_back(std::atoll($3)); std::free($3); }
        ;

Root    : VERSION Version           {}
        ;

%%

void yyerror(YYLTYPE* yyllocp, yyscan_t unused, const std::string &filename, cqasm::version::Version &version, const char* msg) {
    (void) unused;
    (void) version;
    std::ostringstream sb;
    sb << filename
       << ":"  << yyllocp->first_line
       << ":"  << yyllocp->first_column
       << ": " << msg;
    throw cqasm::error::ParseError(sb.str());
}

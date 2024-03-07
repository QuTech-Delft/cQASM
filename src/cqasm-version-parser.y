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
    #include "cqasm-annotations.hpp"
    #include "cqasm-error.hpp"
    #include "cqasm-version.hpp"
    using namespace cqasm::version;
    typedef void* yyscan_t;

    #define YYSTYPE CQASM_VERSION_STYPE
    #define YYLTYPE CQASM_VERSION_LTYPE

}

%code {
    int yylex(YYSTYPE* yylvalp, YYLTYPE* yyllocp, yyscan_t scanner);
    void yyerror(YYLTYPE* yyllocp, yyscan_t scanner, const std::string &file_name, cqasm::version::Version &version, const char* msg);
}

%param { yyscan_t scanner }
%parse-param { const std::string &file_name }
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

void yyerror(YYLTYPE* yyllocp, yyscan_t unused, const std::string &file_name, cqasm::version::Version &version, const char* msg) {
    (void) unused;
    (void) version;
    throw cqasm::error::ParseError(
        std::string(msg),
        file_name,
        { { static_cast<std::uint32_t>(yyllocp->first_line), static_cast<std::uint32_t>(yyllocp->first_column) },
          { static_cast<std::uint32_t>(yyllocp->last_line), static_cast<std::uint32_t>(yyllocp->last_column) } }
    );
}

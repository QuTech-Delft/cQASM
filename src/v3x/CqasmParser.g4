parser grammar CqasmParser;

options {
	tokenVocab = CqasmLexer;
}

// Actual grammar start
program:
    version (NEW_LINE | SEMICOLON)
    (statement (NEW_LINE | SEMICOLON))*
    EOF
    ;

version: VERSION VERSION_NUMBER;

statement:
    QUBIT_TYPE (OPEN_BRACKET INT CLOSE_BRACKET)? ID  # qubitTypeDefinition
    | BIT_TYPE (OPEN_BRACKET INT CLOSE_BRACKET)? ID  # bitTypeDefinition
    | expression EQUAL MEASURE expression  # measureStatement
    | ID expressionList  # instruction
    ;

expressionList: expression (COMMA expression)*;

indexList: indexEntry (COMMA indexEntry)*;

indexEntry:
    expression  # indexItem
    | expression COLON expression  # indexRange
    ;

expression:
    INT  # int
    | FLOAT  # float
    | ID  # id
    | ID OPEN_BRACKET indexList CLOSE_BRACKET  # index
    ;

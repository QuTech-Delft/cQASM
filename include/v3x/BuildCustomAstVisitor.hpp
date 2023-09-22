 #pragma once

#include "v3x/CqasmParser.h"
#include "v3x/CqasmParserVisitor.h"

#include <any>


namespace cqasm::v3x::parser {

class BuildCustomAstVisitor : public CqasmParserVisitor {
public:
    virtual std::any visitProgram(CqasmParser::ProgramContext *context) = 0;
    virtual std::any visitVersion(CqasmParser::VersionContext *context) = 0;
    virtual std::any visitQubits(CqasmParser::QubitsContext *context) = 0;
    virtual std::any visitStatement(CqasmParser::StatementContext *context) = 0;
    virtual std::any visitMapping(CqasmParser::MappingContext *context) = 0;
    virtual std::any visitVariable(CqasmParser::VariableContext *context) = 0;
    virtual std::any visitInstruction(CqasmParser::InstructionContext *context) = 0;
    virtual std::any visitExpressionList(CqasmParser::ExpressionListContext *context) = 0;
    virtual std::any visitExpression(CqasmParser::ExpressionContext *context) = 0;
    virtual std::any visitIndex(CqasmParser::IndexContext *context) = 0;
};

}  // namespace cqasm::v3x::parser

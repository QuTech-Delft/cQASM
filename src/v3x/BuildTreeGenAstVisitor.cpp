#include "cqasm-annotations.hpp"
#include "cqasm-tree.hpp"
#include "v3x/cqasm-ast.hpp"
#include "v3x/BuildTreeGenAstVisitor.hpp"
#include "v3x/CustomErrorListener.hpp"

#include <algorithm>  // for_each
#include <antlr4-runtime.h>
#include <cassert> // assert
#include <cstdint>  // uint32_t
#include <regex>
#include <stdexcept>  // runtime_error
#include <string>  // stod, stoll


namespace cqasm::v3x::parser {

using namespace cqasm::v3x::ast;
using namespace cqasm::error;

BuildTreeGenAstVisitor::BuildTreeGenAstVisitor(std::string file_name)
: file_name_{ std::move(file_name) }
, error_listener_p_{ nullptr } {}

void BuildTreeGenAstVisitor::addErrorListener(CustomErrorListener *errorListener) {
    error_listener_p_ = errorListener;
}

void BuildTreeGenAstVisitor::syntaxError(size_t line, size_t char_position_in_line, const std::string &text) const {
    assert(error_listener_p_);
    error_listener_p_->syntaxError(line, char_position_in_line, text);
}

void BuildTreeGenAstVisitor::setNodeAnnotation(const ast::One<ast::Node> &node, antlr4::Token *token) const {
    auto token_size = token->getStopIndex() - token->getStartIndex() + 1;
    // ANTLR provides a zero-based character position in line
    // We change it here to a one-based index, which is the more human-readable, and the common option in text editors
    node->set_annotation(cqasm::annotations::SourceLocation{
        file_name_,
        static_cast<std::uint32_t>(token->getLine()),
        static_cast<std::uint32_t>(token->getCharPositionInLine() + 1),
        static_cast<std::uint32_t>(token->getLine()),
        static_cast<std::uint32_t>(token->getCharPositionInLine() + 1 + token_size)
    });
}

std::int64_t BuildTreeGenAstVisitor::get_int_value(size_t line, size_t char_position_in_line, const std::string &text) const {
    try {
        return std::stoll(text);
    } catch (std::out_of_range&) {
        syntaxError(line, char_position_in_line, fmt::format("value '{}' is out of the INTEGER_LITERAL range", text));
    }
    return {};
}

double BuildTreeGenAstVisitor::get_float_value(size_t line, size_t char_position_in_line, const std::string &text) const {
    try {
        return std::stod(text);
    } catch (std::out_of_range&) {
        syntaxError(line, char_position_in_line, fmt::format("value '{}' is out of the FLOAT_LITERAL range", text));
    }
    return {};
}

bool BuildTreeGenAstVisitor::get_bool_value(antlr4::tree::TerminalNode *node) const {
    assert(node->getSymbol()->getType() == CqasmParser::BOOLEAN_LITERAL);
    return node->getText() == "true";
}

std::int64_t BuildTreeGenAstVisitor::get_int_value(antlr4::tree::TerminalNode *node) const {
    const auto token = node->getSymbol();
    assert(token->getType() == CqasmParser::INTEGER_LITERAL);
    return get_int_value(token->getLine(), token->getCharPositionInLine(), node->getText());
}

double BuildTreeGenAstVisitor::get_float_value(antlr4::tree::TerminalNode *node) const {
    const auto token = node->getSymbol();
    const auto &text = node->getText();
    assert(token->getType() == CqasmParser::FLOAT_LITERAL);
    return get_float_value(token->getLine(), token->getCharPositionInLine(), text);
}

std::any BuildTreeGenAstVisitor::visitProgram(CqasmParser::ProgramContext *context) {
    auto ret = cqasm::tree::make<Program>();
    ret->version = std::any_cast<One<Version>>(visitVersion(context->version()));
    ret->statements = std::any_cast<One<StatementList>>(visitStatements(context->statements()));
    return ret;
}

std::any BuildTreeGenAstVisitor::visitVersion(CqasmParser::VersionContext *context) {
    auto ret = cqasm::tree::make<Version>();
    const auto token = context->VERSION_NUMBER()->getSymbol();
    const auto &text = context->VERSION_NUMBER()->getText();
    const std::regex pattern{ "([0-9]+)(?:\\.([0-9]+))?" };
    std::smatch matches{};
    std::regex_match(text, matches, pattern);
    ret->items.push_back(get_int_value(token->getLine(), token->getCharPositionInLine(), matches[1]));
    if (matches[2].matched) {
        ret->items.push_back(get_int_value(token->getLine(), token->getCharPositionInLine() + matches.position(2),
            matches[2]));
    }
    setNodeAnnotation(ret, token);
    return ret;
}

std::any BuildTreeGenAstVisitor::visitStatements(CqasmParser::StatementsContext *context) {
    auto ret = cqasm::tree::make<StatementList>();
    const auto &statements = context->statement();
    std::for_each(statements.begin(), statements.end(), [this, &ret](const auto &statement_ctx) {
        ret->items.add(std::any_cast<One<Statement>>(statement_ctx->accept(this)));
    });
    return ret;
}

std::any BuildTreeGenAstVisitor::visitStatementSeparator(CqasmParser::StatementSeparatorContext *) {
    return {};
}

std::any BuildTreeGenAstVisitor::visitQubitTypeDeclaration(CqasmParser::QubitTypeDeclarationContext *context) {
    auto array_size_declaration_ctx = context->arraySizeDeclaration();
    auto size = (array_size_declaration_ctx)
        ? tree::Maybe<IntegerLiteral>{ std::any_cast<One<IntegerLiteral>>(array_size_declaration_ctx->accept(this)).get_ptr() }
        : tree::Maybe<IntegerLiteral>{};
    auto ret = cqasm::tree::make<Variable>(
        One<Identifier>{ cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText()) },
        cqasm::tree::make<Keyword>(context->QUBIT_TYPE()->getText()),
        size
    );
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitBitTypeDeclaration(CqasmParser::BitTypeDeclarationContext *context) {
    auto array_size_declaration_ctx = context->arraySizeDeclaration();
    auto size = (array_size_declaration_ctx)
        ? tree::Maybe<IntegerLiteral>{ std::any_cast<One<IntegerLiteral>>(array_size_declaration_ctx->accept(this)).get_ptr() }
        : tree::Maybe<IntegerLiteral>{};
    auto ret = cqasm::tree::make<Variable>(
        One<Identifier>{ cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText()) },
        cqasm::tree::make<Keyword>(context->BIT_TYPE()->getText()),
        size
    );
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitAxisTypeDefinition(CqasmParser::AxisTypeDeclarationContext *context) {
    auto size = tree::make<IntegerLiteral>(3);
    auto ret = cqasm::tree::make<Variable>(
        One<Identifier>{ cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText()) },
        cqasm::tree::make<Keyword>(context->AXIS_TYPE()->getText()),
        size
    );
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitAxisTypeInitialization(CqasmParser::AxisTypeDeclarationContext *context) {
    auto ret = cqasm::tree::make<Initialization>();
    ret->var = std::any_cast<One<Statement>>(visitAxisTypeDefinition(context));
    ret->rhs = std::any_cast<One<Expression>>(context->expression()->accept(this));
    const auto token = context->EQUALS()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitAxisTypeDeclaration(CqasmParser::AxisTypeDeclarationContext *context) {
    return (context->expression())  // rhs
        ? visitAxisTypeInitialization(context)
        : visitAxisTypeDefinition(context);
}

std::any BuildTreeGenAstVisitor::visitBoolTypeDefinition(CqasmParser::BoolTypeDeclarationContext *context) {
    auto array_size_declaration_ctx = context->arraySizeDeclaration();
    auto size = (array_size_declaration_ctx)
        ? tree::Maybe<IntegerLiteral>{ std::any_cast<One<IntegerLiteral>>(array_size_declaration_ctx->accept(this)).get_ptr() }
        : tree::Maybe<IntegerLiteral>{};
    auto ret = cqasm::tree::make<Variable>(
        One<Identifier>{ cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText()) },
        cqasm::tree::make<Keyword>(context->BOOL_TYPE()->getText()),
        size
    );
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitBoolTypeInitialization(CqasmParser::BoolTypeDeclarationContext *context) {
    auto ret = cqasm::tree::make<Initialization>();
    ret->var = std::any_cast<One<Statement>>(visitBoolTypeDefinition(context));
    ret->rhs = std::any_cast<One<Expression>>(context->expression()->accept(this));
    const auto token = context->EQUALS()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitBoolTypeDeclaration(CqasmParser::BoolTypeDeclarationContext *context) {
    return (context->expression())  // rhs
        ? visitBoolTypeInitialization(context)
        : visitBoolTypeDefinition(context);
}

std::any BuildTreeGenAstVisitor::visitIntTypeDefinition(CqasmParser::IntTypeDeclarationContext *context) {
    auto array_size_declaration_ctx = context->arraySizeDeclaration();
    auto size = (array_size_declaration_ctx)
        ? tree::Maybe<IntegerLiteral>{ std::any_cast<One<IntegerLiteral>>(array_size_declaration_ctx->accept(this)).get_ptr() }
        : tree::Maybe<IntegerLiteral>{};
    auto ret = cqasm::tree::make<Variable>(
        One<Identifier>{ cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText()) },
        cqasm::tree::make<Keyword>(context->INT_TYPE()->getText()),
        size
    );
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitIntTypeInitialization(CqasmParser::IntTypeDeclarationContext *context) {
    auto ret = cqasm::tree::make<Initialization>();
    ret->var = std::any_cast<One<Statement>>(visitIntTypeDefinition(context));
    ret->rhs = std::any_cast<One<Expression>>(context->expression()->accept(this));
    const auto token = context->EQUALS()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitIntTypeDeclaration(CqasmParser::IntTypeDeclarationContext *context) {
    return (context->expression())  // rhs
        ? visitIntTypeInitialization(context)
        : visitIntTypeDefinition(context);
}

std::any BuildTreeGenAstVisitor::visitFloatTypeDefinition(CqasmParser::FloatTypeDeclarationContext *context) {
    auto array_size_declaration_ctx = context->arraySizeDeclaration();
    auto size = (array_size_declaration_ctx)
        ? tree::Maybe<IntegerLiteral>{ std::any_cast<One<IntegerLiteral>>(array_size_declaration_ctx->accept(this)).get_ptr() }
        : tree::Maybe<IntegerLiteral>{};
    auto ret = cqasm::tree::make<Variable>(
        One<Identifier>{ cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText()) },
        cqasm::tree::make<Keyword>(context->FLOAT_TYPE()->getText()),
        size
    );
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitFloatTypeInitialization(CqasmParser::FloatTypeDeclarationContext *context) {
    auto ret = cqasm::tree::make<Initialization>();
    ret->var = std::any_cast<One<Statement>>(visitFloatTypeDefinition(context));
    ret->rhs = std::any_cast<One<Expression>>(context->expression()->accept(this));
    const auto token = context->EQUALS()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitFloatTypeDeclaration(CqasmParser::FloatTypeDeclarationContext *context) {
    return (context->expression())  // rhs
        ? visitFloatTypeInitialization(context)
        : visitFloatTypeDefinition(context);
}

std::any BuildTreeGenAstVisitor::visitArraySizeDeclaration(CqasmParser::ArraySizeDeclarationContext *context) {
    auto int_ctx = context->INTEGER_LITERAL();
    return cqasm::tree::make<IntegerLiteral>(get_int_value(int_ctx));
}

std::any BuildTreeGenAstVisitor::visitMeasureInstruction(CqasmParser::MeasureInstructionContext *context) {
    auto ret = cqasm::tree::make<MeasureInstruction>();
    ret->name = cqasm::tree::make<Identifier>(context->MEASURE()->getText());
    ret->condition = cqasm::tree::Maybe<Expression>{};
    ret->lhs = std::any_cast<One<Expression>>(context->expression(0)->accept(this));
    ret->rhs = std::any_cast<One<Expression>>(context->expression(1)->accept(this));
    const auto token = context->MEASURE()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitInstruction(CqasmParser::InstructionContext *context) {
    auto ret = cqasm::tree::make<Instruction>();
    ret->name = cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText());
    ret->condition = cqasm::tree::Maybe<Expression>{};
    ret->operands = std::any_cast<One<ExpressionList>>(visitExpressionList(context->expressionList()));
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Statement>{ ret };
}

std::any BuildTreeGenAstVisitor::visitExpressionList(CqasmParser::ExpressionListContext *context) {
    auto ret = cqasm::tree::make<ExpressionList>();
    const auto &expressions = context->expression();
    std::for_each(expressions.begin(), expressions.end(), [this, &ret](auto &expression_ctx) {
        ret->items.add(std::any_cast<One<Expression>>(expression_ctx->accept(this)));
    });
    return ret;
}

std::any BuildTreeGenAstVisitor::visitIndexList(CqasmParser::IndexListContext *context) {
    auto ret = cqasm::tree::make<IndexList>();
    const auto &index_entries = context->indexEntry();
    std::for_each(index_entries.begin(), index_entries.end(), [this, &ret](auto &index_entry_ctx) {
        auto index_entry = std::any_cast<One<IndexEntry>>(index_entry_ctx->accept(this));
        ret->items.add(index_entry);
    });
    return ret;
}

std::any BuildTreeGenAstVisitor::visitIndexItem(CqasmParser::IndexItemContext *context) {
    return One<IndexEntry>{ cqasm::tree::make<IndexItem>(
        std::any_cast<One<Expression>>(context->expression()->accept(this))
    )};
}

std::any BuildTreeGenAstVisitor::visitIndexRange(CqasmParser::IndexRangeContext *context) {
    return One<IndexEntry>{ cqasm::tree::make<IndexRange>(
        std::any_cast<One<Expression>>(context->expression(0)->accept(this)),
        std::any_cast<One<Expression>>(context->expression(1)->accept(this))
    )};
}

std::any BuildTreeGenAstVisitor::visitParensExpression(CqasmParser::ParensExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitUnaryPlusMinusExpression(CqasmParser::UnaryPlusMinusExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitUnaryBitwiseNotExpression(CqasmParser::UnaryBitwiseNotExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitUnaryLogicalNotExpression(CqasmParser::UnaryLogicalNotExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitPowerExpression(CqasmParser::PowerExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitProductExpression(CqasmParser::ProductExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitAdditionExpression(CqasmParser::AdditionExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitShiftExpression(CqasmParser::ShiftExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitComparisonExpression(CqasmParser::ComparisonExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitEqualityExpression(CqasmParser::EqualityExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitBitwiseAndExpression(CqasmParser::BitwiseAndExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitBitwiseXorExpression(CqasmParser::BitwiseXorExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitBitwiseOrExpression(CqasmParser::BitwiseOrExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitLogicalAndExpression(CqasmParser::LogicalAndExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitLogicalXorExpression(CqasmParser::LogicalXorExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitLogicalOrExpression(CqasmParser::LogicalOrExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitTernaryConditionalExpression(CqasmParser::TernaryConditionalExpressionContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitFunctionCall(CqasmParser::FunctionCallContext */* context */) {
    throw std::runtime_error{ "unimplemented" };
}

std::any BuildTreeGenAstVisitor::visitIndex(CqasmParser::IndexContext *context) {
    auto ret = cqasm::tree::make<Index>();
    ret->expr = cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText());
    ret->indices = std::any_cast<One<IndexList>>(visitIndexList(context->indexList()));
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Expression>{ ret };
}

std::any BuildTreeGenAstVisitor::visitIdentifier(CqasmParser::IdentifierContext *context) {
    auto ret = cqasm::tree::make<Identifier>(context->IDENTIFIER()->getText());
    const auto token = context->IDENTIFIER()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Expression>{ ret };
}

std::any BuildTreeGenAstVisitor::visitAxisInitializationList(CqasmParser::AxisInitializationListContext *context) {
    auto expression_list = cqasm::tree::make<ExpressionList>();
    const auto &expressions = context->expression();
    std::for_each(expressions.begin(), expressions.end(), [this, &expression_list](auto &expression_ctx) {
        expression_list->items.add(std::any_cast<One<Expression>>(expression_ctx->accept(this)));
    });
    auto ret = cqasm::tree::make<InitializationList>(expression_list);
    return One<Expression>{ ret };
}

std::any BuildTreeGenAstVisitor::visitInitializationList(CqasmParser::InitializationListContext *context) {
    auto ret = cqasm::tree::make<InitializationList>(
        std::any_cast<One<ExpressionList>>(visitExpressionList(context->expressionList())));
    return One<Expression>{ ret };
}

std::any BuildTreeGenAstVisitor::visitBooleanLiteral(CqasmParser::BooleanLiteralContext *context) {
    auto value = get_bool_value(context->BOOLEAN_LITERAL());
    auto ret = cqasm::tree::make<BooleanLiteral>(value);
    const auto token = context->BOOLEAN_LITERAL()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Expression>{ ret };
}

std::any BuildTreeGenAstVisitor::visitIntegerLiteral(CqasmParser::IntegerLiteralContext *context) {
    auto value = get_int_value(context->INTEGER_LITERAL());
    auto ret = cqasm::tree::make<IntegerLiteral>(value);
    const auto token = context->INTEGER_LITERAL()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Expression>{ ret };
}

std::any BuildTreeGenAstVisitor::visitFloatLiteral(CqasmParser::FloatLiteralContext *context) {
    auto value = get_float_value(context->FLOAT_LITERAL());
    auto ret = cqasm::tree::make<FloatLiteral>(value);
    const auto token = context->FLOAT_LITERAL()->getSymbol();
    setNodeAnnotation(ret, token);
    return One<Expression>{ ret };
}

}  // namespace cqasm::v3x::parser

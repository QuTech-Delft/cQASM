/** \file
 * Contains helper classes and objects for the lexer and parser generated by
 * flex/bison, as well as the entry points for invoking the parser directly, in
 * case you don't need semantic analysis.
 */

#pragma once

#include "cqasm-annotations.hpp"
#include "v1x/cqasm-parse-result.hpp"
#include "v3x/cqasm_lexer.h"

#include "antlr4-runtime/antlr4-runtime.h"

#include <fstream>  // ifstream
#include <memory>  // unique_ptr
#include <string>


namespace cqasm {
namespace v3x {

/**
 * Namespace for the parser functions and classes.
 */
namespace parser {

// SourceLocation used to live in this namespace, before the v3x namespace was a thing.
// Make sure it exists here for compatibility.
using SourceLocation = annotations::SourceLocation;


struct ScannerAdaptor {
    virtual ~ScannerAdaptor() = default;

    virtual void parse(const std::string &file_name, cqasm::v1x::parser::ParseResult &result) = 0;
};

class ScannerAntlr : public ScannerAdaptor {
protected:
    void parse_(const std::string & /* file_name */, cqasm::v1x::parser::ParseResult & /* result */,
                antlr4::ANTLRInputStream &is);
public:
    ScannerAntlr();
    ~ScannerAntlr() override;
    void parse(const std::string &file_name, cqasm::v1x::parser::ParseResult &result) = 0;
};

class ScannerAntlrFile : public ScannerAntlr {
    std::ifstream ifs_;
public:
    explicit ScannerAntlrFile(const std::string &file_path);
    ~ScannerAntlrFile() override;
    void parse(const std::string &file_name, cqasm::v1x::parser::ParseResult &result) override;
};

class ScannerAntlrString : public ScannerAntlr {
    std::string data_;
public:
    explicit ScannerAntlrString(const std::string &data);
    ~ScannerAntlrString() override;
    void parse(const std::string &file_name, cqasm::v1x::parser::ParseResult &result) override;
};


/**
 * Parse using the given file path.
 * Throws an AnalysisError if this fails.
 */
cqasm::v1x::parser::ParseResult parse_file(const std::string &file_path, const std::string &file_name = "<unknown>");

/**
 * Parse the given string.
 * A file_name may be given in addition for use within error messages.
 */
cqasm::v1x::parser::ParseResult parse_string(const std::string &data, const std::string &file_name="<unknown>");


/**
 * Internal helper class for parsing cQASM files.
 */
class ParseHelper {
    /**
     * Scanner doing the actual parsing.
     */
    std::unique_ptr<ScannerAdaptor> scanner_up_;

    /**
     * Name of the file being parsed.
     */
    std::string file_name;

public:
    explicit ParseHelper(std::unique_ptr<ScannerAdaptor> scanner_up, std::string file_name = "<unknown>");

    /**
     * Does the actual parsing.
     */
    cqasm::v1x::parser::ParseResult parse();
};

} // namespace parser
} // namespace v3x
} // namespace cqasm

/** \file
 * Contains helper classes and objects for the lexer and parser generated by flex/bison,
 * as well as the entry points for invoking the parser directly,
 * in case you don't need semantic analysis.
 */

#pragma once

#include "cqasm-annotations.hpp"
#include "cqasm-ast.hpp"
#include "cqasm-parse-result.hpp"

#include <cstdio>


namespace cqasm {
namespace v1x {

/**
 * Namespace for the parser functions and classes.
 */
namespace parser {

// SourceLocation used to live in this namespace, before the v1x namespace was a thing.
// Make sure it exists here for compatibility.
using SourceLocation = annotations::SourceLocation;

/**
 * Parse the given file path.
 */
ParseResult parse_file(const std::string &file_path);

/**
 * Parse using the given file pointer.
 */
ParseResult parse_file(FILE* fp, const std::string &file_name = "<unknown>");

/**
 * Parse the given string.
 * A file_name may be given in addition for use within error messages.
 */
ParseResult parse_string(const std::string &data, const std::string &file_name="<unknown>");

/**
 * Internal helper class for parsing cQASM files.
 */
class ParseHelper {
public:
    /**
     * File pointer being scanned, if no data was specified.
     */
    FILE *fptr = nullptr;

    /**
     * Flex data buffer, if data was specified.
     */
    void *buf = nullptr;

    /**
     * Flex reentrant scanner data.
     */
    void *scanner = nullptr;

    /**
     * Name of the file being parsed.
     */
    std::string file_name;

    /**
     * The parse result.
     */
    ParseResult result;

private:
    friend ParseResult parse_file(const std::string &file_path);
    friend ParseResult parse_file(FILE* fp, const std::string &file_name);
    friend ParseResult parse_string(const std::string &data, const std::string &file_name);

    /**
     * Parse a string or file with flex/bison.
     * If use_file is set, the file specified by file_path is read and data is ignored.
     * Otherwise, file_path is used only for error messages, and data is read instead.
     * Don't use this directly, use parse().
     */
    ParseHelper(std::string file_path, const std::string &data, bool use_file);

    /**
     * Construct the analyzer internals for the given file_name, and analyze the file.
     */
    ParseHelper(std::string file_name, FILE *fptr);

    /**
     * Initializes the scanner. Returns whether this was successful.
     */
    bool construct();

    /**
     * Does the actual parsing.
     */
    void parse();

public:
    /**
     * Destroys the parse helper.
     */
    virtual ~ParseHelper();

    /**
     * Pushes an error.
     */
    void push_error(const error::ParseError &error);

    /**
     * Builds and pushes an error.
     */
    void push_error(const std::string &message,
        int first_line, int first_column,
        int last_line, int last_column);
};

} // namespace parser
} // namespace v1x
} // namespace cqasm

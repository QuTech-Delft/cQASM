#include "generator.hpp"

#include <iostream>  // endl
#include <stdexcept>  // invalid_argument, runtime_error


/**
 * Namespace for the \ref func-gen program.
 */
namespace func_gen {

/**
 * Generates the function that registers the functions in a FunctionTable.
 */
void Generator::generate_register_function() {
    header_ofs_ << R"(
/**
 * Registers a bunch of functions usable during constant propagation into the
 * given function table.
 */
void register_into(resolver::FunctionTable &table);
)";
    source_ofs_ << R"(
/**
 * Registers a bunch of functions usable during constant propagation into the
 * given function table.
 */
void register_into(resolver::FunctionTable &table) {
)";
    for (const auto &func : funcs_) {
        source_ofs_ << "    table.add(";
        source_ofs_ << "\"" << func.cqasm_name << "\", ";
        source_ofs_ << "types::from_spec(\"" << func.cqasm_args << "\"), ";
        source_ofs_ << func.cpp_name;
        source_ofs_ << ");" << std::endl;
    }
    source_ofs_ << std::endl << "}" << std::endl;
}

/**
 * Generates the function header in both the header and source file. The
 * start of the function body must follow. This also registers the function
 * to be added to the function that populates FunctionTables.
 */
void Generator::generate_impl_header(const Function &func) {
    auto proto = "values::Value " + func.cpp_name + "(const values::Values &v)";
    header_ofs_ << proto << ";" << std::endl;
    source_ofs_ << std::endl << proto << " {" << std::endl;
    funcs_.push_back(func);
}

/**
 * Like generate_impl_header(), but in addition, generates some boilerplate
 * code for safely casting all the arguments to constant values (throwing
 * a compile error if they're not constant) and storing them in variables
 * a..z. This is, of course, only applicable for functions that are only
 * evaluable during semantic analysis.
 */
void Generator::generate_const_impl_header(const Function &func) {
    generate_impl_header(func);
    source_ofs_ << "    values::check_const(v);" << std::endl;
    size_t index = 0;
    for (auto arg_typ : func.cqasm_args) {
        source_ofs_ << "    auto " << (char)('a' + index) << " = v[" << index << "]";
        switch (arg_typ) {
            case 'b': source_ofs_ << "->as_const_bool()->value"; break;
            case 'a': source_ofs_ << "->as_const_axis()->value"; break;
            case 'i': source_ofs_ << "->as_const_int()->value"; break;
            case 'r': source_ofs_ << "->as_const_real()->value"; break;
            case 'c': source_ofs_ << "->as_const_complex()->value"; break;
            case 'm': source_ofs_ << "->as_const_real_matrix()->value"; break;
            case 'u':
            case 'n': source_ofs_ << "->as_const_complex_matrix()->value"; break;
            case 's': source_ofs_ << "->as_const_string()->value"; break;
            case 'j': source_ofs_ << "->as_const_json()->value"; break;
            default: throw std::invalid_argument("unknown arg type");
        }
        source_ofs_ << ";" << std::endl;
        index++;
    }
}

/**
 * Generates the end of a function body, with an appropriate return
 * statement. return_type must be one of:
 *
 *  - 'b': returns a ConstBool, return_expr must be a primitive::Bool.
 *  - 'a': returns a ConstAxis, return_expr must be a primitive::Axis.
 *  - 'i': returns a ConstInt, return_expr must be a primitive::Int.
 *  - 'r': returns a ConstReal, return_expr must be a primitive::Real.
 *  - 'c': returns a ConstComplex, return_expr must be a primitive::Complex.
 *  - 'm': returns a ConstRealMatrix, return_expr must be a primitive::RMatrix.
 *  - 'n': returns a ConstComplexMatrix, return_expr must be a primitive::CMatrix.
 *  - 's': returns a ConstString, return_expr must be a primitive::Str.
 *  - 'j': returns a ConstJson, return_expr must be a primitive::Str.
 *  - 'Q': returns a QubitRefs, return_expr must be a Many<ConstInt>.
 *  - 'B': returns a BitRefs, return_expr must be a Many<ConstInt>.
 */
void Generator::generate_impl_footer(const std::string &return_expr, char return_type) {
    source_ofs_ << "    return tree::make<values::";
    switch (return_type) {
        case 'b': source_ofs_ << "ConstBool"; break;
        case 'a': source_ofs_ << "ConstAxis"; break;
        case 'i': source_ofs_ << "ConstInt"; break;
        case 'r': source_ofs_ << "ConstReal"; break;
        case 'c': source_ofs_ << "ConstComplex"; break;
        case 'm': source_ofs_ << "ConstRealMatrix"; break;
        case 'n': source_ofs_ << "ConstComplexMatrix"; break;
        case 's': source_ofs_ << "ConstString"; break;
        case 'j': source_ofs_ << "ConstJson"; break;
        case 'Q': source_ofs_ << "QubitRefs"; break;
        case 'B': source_ofs_ << "BitRefs"; break;
        default: throw std::invalid_argument("unknown type code");
    }
    source_ofs_ << ">(" << return_expr << ");" << std::endl;
    source_ofs_ << "}" << std::endl;
}

/**
 * Generates a basic constant scalar function, such as integer addition for
 * instance. name must be the cQASM name for the function (either just the
 * function name or operator+ etc), return_type must be the return type
 * code, arg_types must be the argument type codes, and impl must be a
 * C++ expression implementing the function, operating on variables a..z
 * representing the arguments, and returning the primitive value associated
 * with return_type.
 */
void Generator::generate_const_scalar_op(
    const std::string &name,
    const char return_type,
    const std::string &arg_types,
    const std::string &impl
) {
    generate_const_impl_header(Function(name, arg_types));
    generate_impl_footer(impl, return_type);
}

/**
 * Constructs a generator for the function table.
 */
Generator::Generator(
    const std::string &header_filename,
    const std::string &source_filename,
    const std::string &version
) :
    funcs_(),
    header_ofs_(header_filename),
    source_ofs_(source_filename),
    version_(version)
{
    // Check that the files were opened properly.
    if (!header_ofs_.is_open()) {
        throw std::runtime_error("failed to open header file");
    }
    if (!source_ofs_.is_open()) {
        throw std::runtime_error("failed to open source file");
    }

    // Print the headers for the header and source files.
    auto pos = header_filename.rfind('/');
    auto header_name = (pos == header_filename.npos) ? header_filename : header_filename.substr(pos + 1);
    header_ofs_ << R"(
/** \file
 * Header file generated by \ref func-gen.
 */

#pragma once

#include ")" << version_ << R"(/cqasm-resolver.hpp"

namespace cqasm {
namespace )" << version_ << R"( {
namespace functions {
)";
    source_ofs_ << R"(
/** \file
 * Source file generated by \ref func-gen.
 */

#include <cmath>
#include <complex>
#include ")" << version_ << R"(/)" << header_name << R"("

namespace cqasm {
namespace )" << version_ << R"( {

/**
 * Namespace for the functions generated by \ref func-gen.
 */
namespace functions {

/**
 * Division that always rounds down (towards negative infinity), like Python's
 * integer division. Because rounding to zero is a useless mechanic.
 */
static int64_t div_floor(int64_t a, int64_t b) {
    int64_t res = a / b;
    int64_t rem = a % b;
    // Correct division result downwards if up-rounding happened,
    // (for non-zero remainder of sign different than the divisor).
    int64_t corr = (rem != 0 && ((rem < 0) != (b < 0)));
    return res - corr;
}

/**
 * Modulo to go along with div_floor.
 */
static int64_t mod_floor(int64_t a, int64_t b) {
    int64_t rem = a % b;
    if (rem != 0 && ((rem < 0) != (b < 0))) {
        rem += b;
    }
    return rem;
}

)";
}

/**
 * Finishes writing the header & source file, then destroys the generator.
 */
Generator::~Generator() {
    generate_register_function();
    auto footer = R"(
} // namespace functions
} // namespace )" + version_ + R"(
} // namespace cqasm
)";
    source_ofs_ << footer;
    header_ofs_ << footer;
}

} // namespace func_gen

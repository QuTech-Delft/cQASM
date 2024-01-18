#include "v3x/generator.hpp"


/**
 * Namespace for the \ref func-gen program.
 */
namespace func_gen {

/**
 * Like generate_impl_header(), but in addition,
 * generates some boilerplate code for safely casting all the arguments to constant values
 * (throwing a compile error if they're not constant) and
 * storing them in variables a..z.
 * This is, of course, only applicable for functions that are only evaluable during semantic analysis.
 */
void V3xGenerator::generate_const_impl_header(const Function &func) {
    generate_impl_header(func);
    source_ofs_ << "    values::check_const(v);" << std::endl;
    size_t index = 0;
    for (auto arg_typ : func.cqasm_args) {
        source_ofs_ << "    auto " << (char)('a' + index) << " = v[" << index << "]";
        switch (arg_typ) {
            case 'a': source_ofs_ << "->as_const_axis()->value"; break;
            case 'b': source_ofs_ << "->as_const_bool()->value"; break;
            case 'i': source_ofs_ << "->as_const_int()->value"; break;
            case 'r': source_ofs_ << "->as_const_real()->value"; break;
            case 'c': source_ofs_ << "->as_const_complex()->value"; break;
            case 's': source_ofs_ << "->as_const_string()->value"; break;
            case 'X': source_ofs_ << "->as_const_bool_array()->value"; break;
            case 'Y': source_ofs_ << "->as_const_int_array()->value"; break;
            case 'Z': source_ofs_ << "->as_const_real_array()->value"; break;
            case 'I': source_ofs_ << "->as_index_ref()->value"; break;
            default: throw std::invalid_argument("unknown arg type");
        }
        source_ofs_ << ";" << std::endl;
        index++;
    }
}

/**
 * Generates the end of a function body, with an appropriate return statement.
 * return_type must be one of:
 *
 *  - 'a': returns a ConstAxis, return_expr must be a primitive::Axis.
 *  - 'b': returns a ConstBool, return_expr must be a primitive::Bool.
 *  - 'i': returns a ConstInt, return_expr must be a primitive::Int.
 *  - 'r': returns a ConstReal, return_expr must be a primitive::Real.
 *  - 'c': returns a ConstComplex, return_expr must be a primitive::Complex.
 *  - 's': returns a ConstString, return_expr must be a primitive::Str.
 *  - 'X': returns a ConstBoolArray, return_expr must be a Many<ConstBool>.
 *  - 'Y': returns a ConstIntArray, return_expr must be a Many<ConstInt>.
 *  - 'Z': returns a ConstRealArray, return_expr must be a Many<ConstReal>.
 *  - 'I': returns an IndexRef, return_expr must be a Many<ConstInt>.
 */
void V3xGenerator::generate_impl_footer(const std::string &return_expr, char return_type) {
    source_ofs_ << "    return tree::make<values::";
    switch (return_type) {
        case 'a': source_ofs_ << "ConstAxis"; break;
        case 'b': source_ofs_ << "ConstBool"; break;
        case 'i': source_ofs_ << "ConstInt"; break;
        case 'r': source_ofs_ << "ConstReal"; break;
        case 'c': source_ofs_ << "ConstComplex"; break;
        case 's': source_ofs_ << "ConstString"; break;
        case 'X': source_ofs_ << "ConstBoolArray"; break;
        case 'Y': source_ofs_ << "ConstIntArray"; break;
        case 'Z': source_ofs_ << "ConstRealArray"; break;
        case 'I': source_ofs_ << "IndexRef"; break;
        default: throw std::invalid_argument("unknown type code");
    }
    source_ofs_ << ">(" << return_expr << ");" << std::endl;
    source_ofs_ << "}" << std::endl;
}

/**
 * Constructs a v3x generator for the function table.
 */
V3xGenerator::V3xGenerator(const std::string &header_filename, const std::string &source_filename)
: Generator{ header_filename, source_filename, v3x_version }
{}

} // namespace func_gen

#include "v1x/generator.hpp"


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
void V1xGenerator::generate_const_impl_header(const Function &func) {
    generate_impl_header(func);
    source_ofs_ << "    values::check_const(v);\n";
    size_t index = 0;
    for (auto arg_typ : func.cqasm_args) {
        source_ofs_ << "    auto " << (char)('a' + index) << " = v[" << index << "]";
        switch (arg_typ) {
            case 'a': source_ofs_ << "->as_const_axis()->value"; break;
            case 'b': source_ofs_ << "->as_const_bool()->value"; break;
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
        source_ofs_ << ";\n";
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
 *  - 'm': returns a ConstRealMatrix, return_expr must be a primitive::RMatrix.
 *  - 'n': returns a ConstComplexMatrix, return_expr must be a primitive::CMatrix.
 *  - 's': returns a ConstString, return_expr must be a primitive::Str.
 *  - 'j': returns a ConstJson, return_expr must be a primitive::Str.
 *  - 'Q': returns a QubitRefs, return_expr must be a Many<ConstInt>.
 *  - 'B': returns a BitRefs, return_expr must be a Many<ConstInt>.
 */
void V1xGenerator::generate_impl_footer(const std::string &return_expr, char return_type) {
    source_ofs_ << "    return tree::make<values::";
    switch (return_type) {
        case 'a': source_ofs_ << "ConstAxis"; break;
        case 'b': source_ofs_ << "ConstBool"; break;
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
    source_ofs_ << ">(" << return_expr << ");\n";
    source_ofs_ << "}\n";
}

/**
 * Constructs a v1x generator for the function table.
 */
V1xGenerator::V1xGenerator(const std::string &header_filename, const std::string &source_filename)
: Generator{ header_filename, source_filename, v1x_version }
{}

} // namespace func_gen

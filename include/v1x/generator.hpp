#pragma once

#include "../generator.hpp"


/**
 * Namespace for the \ref func-gen program.
 */
namespace func_gen {

constinit const std::string v1x_version{ "v1x" };

/**
 * Generator class.
 */
class V1xGenerator : public Generator {
    /**
     * Like generate_impl_header(), but in addition,
     * generates some boilerplate code for safely casting all the arguments to constant values
     * (throwing a compile error if they're not constant) and
     * storing them in variables a..z.
     * This is, of course, only applicable for functions that are only evaluable during semantic analysis.
     */
    void generate_const_impl_header(const Function &func) override;

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
    void generate_impl_footer(const std::string &return_expr, char return_type) override;

public:
    /**
     * Constructs a v1x generator for the function table.
     */
    V1xGenerator(const std::string &header_filename, const std::string &source_filename);

    /**
     * Finishes writing the header & source file, then destroys the v1x generator.
     */
    ~V1xGenerator() override = default;
};

} // namespace func_gen

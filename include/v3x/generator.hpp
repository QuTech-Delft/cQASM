#pragma once

#include "../generator.hpp"

#include <string_view>


/**
 * Namespace for the \ref func-gen program.
 */
namespace func_gen {

constinit const std::string_view v3x_version{ "v3x" };

/**
 * Generator class.
 */
class V3xGenerator : public Generator {
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
     *  - 'X': returns a ConstBoolArray, return_expr must be a Many<ConstBool>.
     *  - 'Y': returns a ConstIntArray, return_expr must be a Many<ConstInt>.
     *  - 'Z': returns a ConstRealArray, return_expr must be a Many<ConstReal>.
     *  - 'I': returns an IndexRef, return_expr must be a Many<ConstReal>.
     */
    void generate_impl_footer(const std::string &return_expr, char return_type) override;

public:
    /**
     * Constructs a v3x generator for the function table.
     */
    V3xGenerator(const std::string &header_filename, const std::string &source_filename);

    /**
     * Finishes writing the header & source file, then destroys the v3x generator.
     */
    ~V3xGenerator() override = default;
};

} // namespace func_gen

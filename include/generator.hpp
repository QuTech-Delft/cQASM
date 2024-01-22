#pragma once

#include "function.hpp"  // Function

#include <fstream>
#include <string>
#include <vector>


/**
 * Namespace for the \ref func-gen program.
 */
namespace func_gen {

/**
 * Generator class.
 */
class Generator {
protected:
    /**
     * Vector of all functions generated so far.
     */
    std::vector<Function> funcs_;

    /**
     * Output stream for the header file.
     */
    std::ofstream header_ofs_;

    /**
     * Output stream for the source file.
     */
    std::ofstream source_ofs_;

    /**
     * Version name.
     */
    std::string version_;

    /**
     * Generates the function that registers the functions in a FunctionTable.
     */
    void generate_register_default_functions_into();

    /**
     * Generates the function header in both the header and source file.
     * The start of the function body must follow.
     * This also registers the function to be added to the function that populates FunctionTables.
     */
    void generate_impl_header(const Function &func);

    /**
     * Like generate_impl_header(), but in addition,
     * generates some boilerplate code for safely casting all the arguments to constant values
     * (throwing a compile error if they're not constant) and
     * storing them in variables a..z.
     * This is, of course, only applicable for functions that are only evaluable during semantic analysis.
     */
    virtual void generate_const_impl_header(const Function &func) = 0;

    /**
     * Generates the end of a function body, with an appropriate return statement.
     */
    virtual void generate_impl_footer(const std::string &return_expr, char return_type) = 0;

public:
    /**
     * Generates a basic constant scalar function, such as integer addition for instance.
     * name must be the cQASM name for the function (either just the function name or operator+ etc),
     * return_type must be the return type code,
     * arg_types must be the argument type codes, and
     * impl must be a C++ expression implementing the function,
     * operating on variables a..z representing the arguments, and
     * returning the primitive value associated with return_type.
     */
    void generate_const_scalar_op(
        const std::string &name,
        const char return_type,
        const std::string &arg_types,
        const std::string &impl);

    /**
     * Constructs a generator for the function table.
     */
    Generator(
        const std::string &header_filename,
        const std::string &source_filename,
        std::string version);

    /**
     * Finishes writing the header & source file, then destroys the generator.
     */
    virtual ~Generator();
};

} // namespace func_gen

pragma once

#include <string>


/**
 * Namespace for the \ref func-gen program.
 */
namespace func_gen {

/**
 * Represents a previously generated function, to be added in the register
 * function once it is generated.
 */
class Function {
    /**
     * Name of the function implementation as generated in the C++ file.
     */
    const std::string cpp_name;

    /**
     * Name of the function as exposed to cQASM.
     */
    const std::string cqasm_name;

    /**
     * Argument type spec for overload resolution, type checking, and type
     * promotion. See cqasm::types::from_spec.
     */
    const std::string cqasm_args;

private:

    /**
     * Generates the C++ name for the function.
     */
    static std::string unique_name(const std::string &name, const std::string &args);
public:

    /**
     * Constructs a normal function. name must be a valid identifier.
     */
    Function(const std::string &name, const std::string &args);

};

} // namespace func_gen

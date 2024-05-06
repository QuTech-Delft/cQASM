#include "function.hpp"


/**
 * Namespace for the \ref func-gen program.
 */
namespace func_gen {

/**
 * Generates the C++ name for the function.
 */
/* static */ std::string Function::unique_name(const std::string &name, const std::string &args) {
    if (name == "operator+") {
        return "op_add_" + args;
    } else if (name == "operator-") {
        if (args.size() == 1) {
            return "op_neg_" + args;
        } else {
            return "op_sub_" + args;
        }
    } else if (name == "operator*") {
        return "op_mul_" + args;
    } else if (name == "operator/") {
        return "op_div_" + args;
    } else if (name == "operator//") {
        return "op_int_div_" + args;
    } else if (name == "operator%") {
        return "op_mod_" + args;
    } else if (name == "operator**") {
        return "op_pow_" + args;
    } else if (name == "operator<<") {
        return "op_shl_" + args;
    } else if (name == "operator>>") {
        return "op_sra_" + args;
    } else if (name == "operator>>>") {
        return "op_srl_" + args;
    } else if (name == "operator==") {
        return "op_eq_" + args;
    } else if (name == "operator!=") {
        return "op_ne_" + args;
    } else if (name == "operator>=") {
        return "op_ge_" + args;
    } else if (name == "operator>") {
        return "op_gt_" + args;
    } else if (name == "operator<=") {
        return "op_le_" + args;
    } else if (name == "operator<") {
        return "op_lt_" + args;
    } else if (name == "operator~") {
        return "op_binv_" + args;
    } else if (name == "operator&") {
        return "op_band_" + args;
    } else if (name == "operator^") {
        return "op_bxor_" + args;
    } else if (name == "operator|") {
        return "op_bor_" + args;
    } else if (name == "operator!") {
        return "op_linv_" + args;
    } else if (name == "operator&&") {
        return "op_land_" + args;
    } else if (name == "operator^^") {
        return "op_lxor_" + args;
    } else if (name == "operator||") {
        return "op_lor_" + args;
    } else if (name == "operator?:") {
        return "op_tcnd_" + args;
    } else {
        return "fn_" + name + "_" + args;
    }
}

/**
 * Constructs a normal function. name must be a valid identifier.
 */
Function::Function(const std::string &name, const std::string &args)
    : cpp_name(unique_name(name, args)), cqasm_name(name), cqasm_args(args)
{}

} // namespace func_gen

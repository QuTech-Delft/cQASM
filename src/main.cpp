#include "generator.hpp"  // Generator
#include "v1x/generator.hpp"  // V1xGenerator
#include "v3x/generator.hpp"  // V3xGenerator

#include <iostream>  // cerr
#include <memory>  // make_unique, unique_ptr
#include <sstream>  // ostringstream
#include <string_view>


void print_usage() {
    std::cerr << "Usage: func-gen <header.hpp> <source.cpp> <version>\n";
    std::cerr << "Where: <version> can be whether 'v1x' or 'v3x'\n";
}


void generate_common_functions(func_gen::Generator* generator) {
    // Basic scalar arithmetic operators
    generator->generate_const_scalar_op("operator+", 'c', "cc", "a + b");
    generator->generate_const_scalar_op("operator+", 'r', "rr", "a + b");
    generator->generate_const_scalar_op("operator+", 'i', "ii", "a + b");
    generator->generate_const_scalar_op("operator-", 'c', "cc", "a - b");
    generator->generate_const_scalar_op("operator-", 'r', "rr", "a - b");
    generator->generate_const_scalar_op("operator-", 'i', "ii", "a - b");
    generator->generate_const_scalar_op("operator-", 'c', "c", "-a");
    generator->generate_const_scalar_op("operator-", 'r', "r", "-a");
    generator->generate_const_scalar_op("operator-", 'i', "i", "-a");
    generator->generate_const_scalar_op("operator*", 'c', "cc", "a * b");
    generator->generate_const_scalar_op("operator*", 'r', "rr", "a * b");
    generator->generate_const_scalar_op("operator*", 'i', "ii", "a * b");
    generator->generate_const_scalar_op("operator/", 'c', "cc", "a / b");
    generator->generate_const_scalar_op("operator/", 'r', "rr", "a / b");
    generator->generate_const_scalar_op("operator//", 'i', "ii", "div_floor(a, b)");
    generator->generate_const_scalar_op("operator%", 'i', "ii", "mod_floor(a, b)");
    generator->generate_const_scalar_op("operator**", 'c', "cc", "std::pow(a, b)");
    generator->generate_const_scalar_op("operator**", 'r', "rr", "std::pow(a, b)");

    // Relational operators
    generator->generate_const_scalar_op("operator==", 'b', "cc", "a == b");
    generator->generate_const_scalar_op("operator!=", 'b', "cc", "a != b");
    for (const char *type : {"rr", "ii", "bb"}) {
        generator->generate_const_scalar_op("operator==", 'b', type, "a == b");
        generator->generate_const_scalar_op("operator!=", 'b', type, "a != b");
        generator->generate_const_scalar_op("operator>=", 'b', type, "a >= b");
        generator->generate_const_scalar_op("operator>", 'b', type, "a > b");
        generator->generate_const_scalar_op("operator<=", 'b', type, "a <= b");
        generator->generate_const_scalar_op("operator<", 'b', type, "a < b");
    }

    // Bitwise operators
    generator->generate_const_scalar_op("operator~", 'i', "i", "~a");
    generator->generate_const_scalar_op("operator&", 'i', "ii", "a & b");
    generator->generate_const_scalar_op("operator^", 'i', "ii", "a ^ b");
    generator->generate_const_scalar_op("operator|", 'i', "ii", "a | b");
    generator->generate_const_scalar_op("operator<<", 'i', "ii", "a << b");
    generator->generate_const_scalar_op("operator>>", 'i', "ii", "a >> b");
    generator->generate_const_scalar_op("operator>>>", 'i', "ii", "(int64_t)(((uint64_t)a) >> b)");

    // Logical operators
    generator->generate_const_scalar_op("operator!", 'b', "b", "!a");
    generator->generate_const_scalar_op("operator&&", 'b', "bb", "a && b");
    generator->generate_const_scalar_op("operator^^", 'b', "bb", "!a ^ !b");
    generator->generate_const_scalar_op("operator||", 'b', "bb", "a || b");

    // Ternary conditional
    for (const char *type : {"bcc", "brr", "bii", "bbb"}) {
        generator->generate_const_scalar_op("operator?:", type[1], type, "a ? b : c");
    }

    // Scalar root, exponent, and trigonometric functions for reals and complex numbers
    for (const char *type : {"c", "r"}) {
        generator->generate_const_scalar_op("sqrt", type[0], type, "std::sqrt(a)");
        generator->generate_const_scalar_op("exp", type[0], type, "std::exp(a)");
        generator->generate_const_scalar_op("log", type[0], type, "std::log(a)");
        for (const char *prefix : {"", "a"}) {
            for (const char *suffix : {"", "h"}) {
                for (const char *func : {"sin", "cos", "tan"}) {
                    std::ostringstream ss;
                    ss << prefix << func << suffix;
                    generator->generate_const_scalar_op(
                        ss.str(), type[0], type, "std::" + ss.str() + "(a)");
                }
            }
        }
    }

    // Absolute value function
    generator->generate_const_scalar_op("abs", 'r', "r", "std::abs(a)");
    generator->generate_const_scalar_op("abs", 'i', "i", "std::abs(a)");

    // Complex number manipulation functions
    generator->generate_const_scalar_op("complex", 'c', "rr", "primitives::Complex(a, b)");
    generator->generate_const_scalar_op("polar", 'c', "rr", "std::polar<double>(a, b)");
    generator->generate_const_scalar_op("real", 'r', "c", "std::real<double>(a)");
    generator->generate_const_scalar_op("imag", 'r', "c", "std::imag<double>(a)");
    generator->generate_const_scalar_op("arg", 'r', "c", "std::arg<double>(a)");
    generator->generate_const_scalar_op("norm", 'r', "c", "std::norm<double>(a)");
    generator->generate_const_scalar_op("conj", 'c', "c", "std::conj<double>(a)");
}


void generate_v1x_specific_functions(func_gen::Generator* generator) {
    // Basic scalar arithmetic operators
    generator->generate_const_scalar_op("operator+", 's', "ss", "a + b");
}


void test_v1x(const std::string &header_file_path, const std::string &source_file_path) {
    std::unique_ptr<func_gen::Generator> generator{};
    generator = std::make_unique<func_gen::V1xGenerator>(header_file_path, source_file_path);
    generate_common_functions(generator.get());
    generate_v1x_specific_functions(generator.get());
}


void test_v3x(const std::string &header_file_path, const std::string &source_file_path) {
    std::unique_ptr<func_gen::Generator> generator{};
    generator = std::make_unique<func_gen::V3xGenerator>(header_file_path, source_file_path);
    generate_common_functions(generator.get());
}


int main(int argc, char *argv[]) {
    // Check command line
    if (argc != 4) {
        print_usage();
        return 1;
    }
    
    // Check version, and test v1x or v3x generators
    std::string_view version{ argv[3] };
    if (version == "v1x") {
        test_v1x(argv[1], argv[2]);
    } else if (version == "v3x") {
        test_v3x(argv[1], argv[2]);
    } else {
        print_usage();
        return 2;
    }

    return 0;
}

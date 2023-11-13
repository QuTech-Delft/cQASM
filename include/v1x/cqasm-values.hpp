/** \file
 * Defines classes representing the \ref cqasm::v1x::values "values"
 * (collective name for constants, references, and dynamically evaluated expressions)
 * available within cQASM's type system, as well as some utility functions.
 */

#pragma once

#include "cqasm-ast.hpp"
#include "cqasm-types.hpp"
#include "v1x/cqasm-values-gen.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>


/**
 * Namespace for the classes representing the values
 * (including dynamically evaluated expressions or references to registers) available within cQASM.
 */
namespace cqasm::v1x::values {

/**
 * A cQASM value, either known at compile-time or an expression for something only known at runtime.
 */
using Value = tree::One<Node>;

/**
 * Zero or more cQASM values.
 */
using Values = tree::Any<Node>;

/**
 * Type-checks and (if necessary) promotes the given value to the given type.
 * Also checks assignability of the value if the type says the value must be assignable.
 * Returns null if the check/promotion fails,
 * otherwise returns the constructed value by way of a smart pointer.
 * If the type was an exact match, this may return the given value without modification or a clone thereof.
 */
Value promote(const Value &value, const types::Type &type);

/**
 * Returns the type of the given value.
 */
types::Type type_of(const Value &value);

/**
 * Returns the types of the given values.
 */
types::Types types_of(const Values &values);

/**
 * Throws an AnalysisError if the given value is not a constant,
 * i.e. if it doesn't have a known value at this time.
 */
void check_const(const Value &value);

/**
 * Throws an AnalysisError if any of the given values are not a constant,
 * i.e. if it doesn't have a known value at this time.
 */
void check_const(const Values &values);

/**
 * Stream << overload for a single value.
 */
std::ostream &operator<<(std::ostream &os, const Value &value);

/**
 * Stream << overload for zero or more values.
 */
std::ostream &operator<<(std::ostream &os, const Values &values);

} // namespace cqasm::v1x::values


template <> struct fmt::formatter<cqasm::v1x::values::Value> : fmt::ostream_formatter {};
template <> struct fmt::formatter<cqasm::v1x::values::Values> : fmt::ostream_formatter {};

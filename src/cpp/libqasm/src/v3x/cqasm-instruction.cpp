/** \file
 * Implementation for \ref include/v3x/cqasm-instruction.hpp "v3x/cqasm-instruction.hpp".
 */

#include "v3x/cqasm-instruction.hpp"

#include "v3x/cqasm-semantic.hpp"

namespace cqasm::v3x {

namespace instruction {

/**
 * Creates a new instruction.
 * param_types is a shorthand type specification string as parsed by cqasm::types::from_spec().
 * If you need more control, you can also manipulate param_types directly.
 */
Instruction::Instruction(std::string name, const std::optional<std::string> &param_types)
: name{ std::move(name) }
, param_types{ types::from_spec(param_types.value_or("")) } {}

/**
 * Equality operator.
 */
bool Instruction::operator==(const Instruction &rhs) const {
    return name == rhs.name && param_types == rhs.param_types;
}

/**
 * Stream << overload for instructions.
 */
std::ostream &operator<<(std::ostream &os, const Instruction &instruction) {
    return os << instruction.name << instruction.param_types;
}

/**
 * Stream << overload for instruction references.
 */
std::ostream &operator<<(std::ostream &os, const InstructionRef &instruction) {
    return instruction.empty() ? os << "unresolved" : os << *instruction;
}

}  // namespace instruction

namespace primitives {

template <>
void serialize(const instruction::InstructionRef &obj, ::tree::cbor::MapWriter &map) {
    if (obj.empty()) {
        return;
    }
    map.append_string("n", obj->name);
    auto aw = map.append_array("t");
    for (const auto &t : obj->param_types) {
        aw.append_binary(::tree::base::serialize(::tree::base::Maybe<types::TypeBase>{ t.get_ptr() }));
    }
    aw.close();
}

template <>
instruction::InstructionRef deserialize(const ::tree::cbor::MapReader &map) {
    if (!map.count("n")) {
        return {};
    }
    auto instruction = tree::make<instruction::Instruction>(map.at("n").as_string(), "");
    auto ar = map.at("t").as_array();
    for (const auto &element : ar) {
        instruction->param_types.add(::tree::base::deserialize<types::Node>(element.as_binary()));
    }
    return instruction;
}

}  // namespace primitives

}  // namespace cqasm::v3x

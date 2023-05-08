/** \file
 * Implementation for \ref include/v3/cqasm-error-model.hpp "v3/cqasm-error-model.hpp".
 */

#include "../../include/cqasm-utils.hpp"
#include "v3/cqasm-error-model.hpp"
#include "v3/cqasm-semantic.hpp"

namespace cqasm {
namespace v3 {
namespace error_model {

/**
 * Creates a new error model. param_types is a shorthand type specification
 * string as parsed by cqasm::types::from_spec(). If you need more control,
 * you can also manipulate param_types directly.
 */
ErrorModel::ErrorModel(
    const std::string &name,
    const std::string &param_types
) :
    name(name),
    param_types(types::from_spec(param_types))
{}

/**
 * Equality operator.
 */
bool ErrorModel::operator==(const ErrorModel &rhs) const {
    return utils::case_insensitive_equals(name, rhs.name) && param_types == rhs.param_types;
}

/**
 * Stream << overload for error models.
 */
std::ostream &operator<<(std::ostream &os, const ErrorModel &model) {
    os << model.name << model.param_types;
    return os;
}

/**
 * Stream << overload for error model references.
 */
std::ostream &operator<<(std::ostream &os, const ErrorModelRef &model) {
    if (model.empty()) {
        os << "unresolved";
    } else {
        os << *model;
    }
    return os;
}

} // namespace error_model

namespace primitives {

template <>
void serialize(const error_model::ErrorModelRef &obj, ::tree::cbor::MapWriter &map) {
    if (obj.empty()) {
        return;
    }
    map.append_string("n", obj->name);
    auto aw = map.append_array("t");
    for (const auto &t : obj->param_types) {
        aw.append_binary(::tree::base::serialize(t));
    }
    aw.close();
}

template <>
error_model::ErrorModelRef deserialize(const ::tree::cbor::MapReader &map) {
    if (!map.count("n")) {
        return {};
    }
    auto model = tree::make<error_model::ErrorModel>(map.at("n").as_string(), "");
    auto ar = map.at("t").as_array();
    for (size_t i = 0; i < ar.size(); i++) {
        model->param_types.add(::tree::base::deserialize<types::Node>(ar.at(i).as_binary()));
    }
    return std::move(model);
}

} // namespace primitives
} // namespace v3
} // namespace cqasm

//
// Created by ZQD on 2026/6/3.
//

#include "PropertyValue.h"

PropertyValue::PropertyValue() {
    m_Value = std::nullopt;
}

PropertyValue::PropertyValue(bool value) {
    SetValue(value);
}

PropertyValue::PropertyValue(double value) {
    SetValue(value);
}

PropertyValue::PropertyValue(int value) {
    SetValue(value);
}

PropertyValue::PropertyValue(const std::string &value) {
    SetValue(value);
}

PropertyValue::PropertyValue(const std::vector<int> &value) {
    SetValue(value);
}

PropertyValue::PropertyValue(const std::vector<double> &value) {
    SetValue(value);
}

PropertyValue::PropertyValue(const GeometryTypes::Point3D &value) {
    SetValue(value);
}

PropertyValue::PropertyValue(const GeometryTypes::RTransform &value) {
    SetValue(value);
}

bool PropertyValue::HasValue() const {
    return m_Value.has_value();
}

std::string PropertyValue::GetTypeName() const {
    if (!m_Value.has_value()) {
        return "empty";
    }

    return std::visit([]<typename T0>([[maybe_unused]] const T0 &value) -> std::string {
        using T = std::decay_t<T0>;
        if constexpr (std::is_same_v<T, bool>) {
            return "bool";
        } else if constexpr (std::is_same_v<T, int>) {
            return "int";
        } else if constexpr (std::is_same_v<T, double>) {
            return "double";
        } else if constexpr (std::is_same_v<T, std::vector<int> >) {
            return "vector<int>";
        } else if constexpr (std::is_same_v<T, std::vector<double> >) {
            return "vector<double>";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "string";
        } else if constexpr (std::is_same_v<T, GeometryTypes::Point3D>) {
            return "Point3D";
        } else if constexpr (std::is_same_v<T, GeometryTypes::RTransform>) {
            return "RTransform";
        } else {
            return "unknown";
        }
    }, m_Value.value());
}

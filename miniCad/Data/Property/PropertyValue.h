//
// Created by ZQD on 2026/6/3.
//

#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "../Geometry/GeometryTypes.h"


class PropertyValue {
public:
    using TV = std::variant<bool, int, double, std::vector<int>, std::vector<double>, std::string,
        GeometryTypes::Point3D, GeometryTypes::RTransform>;

    explicit PropertyValue();

    explicit PropertyValue(bool value);

    explicit PropertyValue(double value);

    explicit PropertyValue(int value);

    explicit PropertyValue(const std::string &value);

    explicit PropertyValue(const std::vector<int> &value);

    explicit PropertyValue(const std::vector<double> &value);

    explicit PropertyValue(const GeometryTypes::Point3D &value);

    explicit PropertyValue(const GeometryTypes::RTransform &value);

    ~PropertyValue() = default;

    [[nodiscard]] bool HasValue() const;

public:
    template<typename T>
    T GetValue() const {
        return std::get<T>(m_Value.value());
    }

    template<typename T>
    void SetValue(T &&value) {
        m_Value = std::forward<T>(value);
    }

    template<typename T>
    bool GetValueR(T &value) const;

    [[nodiscard]] std::string GetTypeName() const;

private:
    std::optional<TV> m_Value;
};

template<typename T>
bool PropertyValue::GetValueR(T &value) const {
    if (!m_Value.has_value()) {
        return false;
    }

    if (const auto data = std::get_if<T>(&m_Value.value())) {
        value = *data;
        return true;
    }

    return false;
}



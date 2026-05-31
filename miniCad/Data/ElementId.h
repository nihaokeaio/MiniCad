//
// Created by ZQD on 2026/5/31.
//

#pragma once

#include <cstdint>
#include <string>
#include <iostream>


struct ElementId {
    using value_type = uint64_t;


    explicit ElementId(uint64_t id);

    explicit ElementId(const std::string &value);

    bool operator==(const ElementId &rhs) const;

    [[nodiscard]] value_type GetValue() const;

    static const ElementId InvalidId;

    [[nodiscard]] bool IsValid() const;

    value_type m_Value;
};

inline std::ostream &operator<<(std::ostream &oss, const ElementId &val) {
    return oss << val.GetValue();
}

// for hasher
template<>
struct std::hash<ElementId> {
    size_t operator()(const ElementId &_Keyval) const noexcept {
        static std::hash<ElementId::value_type> hasher;
        return hasher(_Keyval.GetValue());
    }
}; // namespace std


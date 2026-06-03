//
// Created by ZQD on 2026/6/3.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "PropertyValue.h"


class PropertySet {
public:
    PropertySet();

    ~PropertySet();

    void Set(const std::string &key, const PropertyValue &value);

    template<typename T>
    void SetT(const std::string &key, const T &value);


    template<typename T>
    bool Get(const std::string &key, T &value) const;

    PropertyValue Get(const std::string &key) const;

    void Remove(const std::string &key);

    void Clear();

    bool Exists(const std::string &key) const;

private:
    std::unordered_map<std::string, PropertyValue> m_Properties;
};

template<typename T>
void PropertySet::SetT(const std::string &key, const T &value) {
    Set(key, PropertyValue(value));
}

template<typename T>
bool PropertySet::Get(const std::string &key, T &value) const {
    if (const auto it = m_Properties.find(key); it != m_Properties.end()) {
        return it->second.GetValueR(value);
    }
    return false;
}



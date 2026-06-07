//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include <string>
#include <unordered_map>

#include "ElementType.h"
#include "Property/PropertyValue.h"

struct ElementCreateParams {
    ElementType type;
    std::unordered_map<std::string, PropertyValue> properties;
};

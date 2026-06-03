//
// Created by ZQD on 2026/6/3.
//

#pragma once

#include <memory>

#include "ElementType.h"

class Element;

class ElementFactory {
public:
    static std::unique_ptr<Element> Create(ElementType type);
};

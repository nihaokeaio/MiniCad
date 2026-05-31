//
// Created by ZQD on 2026/5/31.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <Utils/GlobalUniqueId.h>

#include "ElementId.h"

class Element;

class Document {
public:
    void RegisterElement(std::unique_ptr<Element> &&element);

    void UnregisterElement(const ElementId &elementId);

    static ElementId NewElementId();

    Element *FindElement(const ElementId &elementId);

private:
    std::unordered_map<ElementId, std::unique_ptr<Element> > m_Elements;
};

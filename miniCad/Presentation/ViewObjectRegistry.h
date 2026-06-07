//
// Created by ZQD on 2026/5/31.
//

#pragma once
#include <AIS_InteractiveObject.hxx>
#include <Standard_Handle.hxx>

#include "ElementId.h"

class ViewObjectRegistry {
public:
    void Register(Handle(AIS_InteractiveObject) object, ElementId id);

    void UnRegister(ElementId id);

    ElementId FindElement(Handle(AIS_InteractiveObject) object);

    std::vector<Handle(AIS_InteractiveObject) > FindElementAisObjects(ElementId id);

    Handle(AIS_InteractiveObject) FindFirstElementAisObject(ElementId id);

private:
    std::unordered_map<void *, ElementId> m_AisObjectMap;
    std::unordered_map<ElementId, std::vector<Handle(AIS_InteractiveObject) > > m_ElementToObjects;;
};



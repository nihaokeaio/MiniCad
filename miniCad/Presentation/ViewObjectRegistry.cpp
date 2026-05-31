//
// Created by ZQD on 2026/5/31.
//

#include "ViewObjectRegistry.h"

#include <cassert>

void ViewObjectRegistry::Register(opencascade::handle<AIS_InteractiveObject> object, ElementId id) {
    assert(id!=ElementId::InvalidId);
    m_AisObjectMap.insert({object.get(), id});
    if (const auto iter = m_ElementToObjects.find(id); iter != m_ElementToObjects.end()) {
        iter->second.push_back(object);
    } else {
        m_ElementToObjects.insert({id, {object}});
    }
}

void ViewObjectRegistry::UnRegister(const ElementId id) {
    assert(id!=ElementId::InvalidId);
    if (const auto iter = m_ElementToObjects.find(id); iter != m_ElementToObjects.end()) {
        for (auto object: iter->second) {
            if (const auto it = m_AisObjectMap.find(object.get()); it != m_AisObjectMap.end()) {
                m_AisObjectMap.erase(it);
            }
        }
        m_ElementToObjects.erase(id);
    }
}

ElementId ViewObjectRegistry::FindElement(opencascade::handle<AIS_InteractiveObject> object) {
    if (const auto iter = m_AisObjectMap.find(object.get()); iter != m_AisObjectMap.end()) {
        return iter->second;
    }
    return ElementId::InvalidId;
}

std::vector<opencascade::handle<AIS_InteractiveObject> > ViewObjectRegistry::FindElementAisObjects(ElementId id) {
    assert(id!=ElementId::InvalidId);
    if (const auto iter = m_ElementToObjects.find(id); iter != m_ElementToObjects.end()) {
        return iter->second;
    }
    return {};
}

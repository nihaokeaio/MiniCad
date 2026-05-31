//
// Created by ZQD on 2026/5/31.
//

#include "Document.h"

#include "Element.h"

void Document::RegisterElement(std::unique_ptr<Element> &&element) {
    assert(element->GetId()!=ElementId::InvalidId);
    element->SetDocument(this);
    m_Elements.insert(std::make_pair(element->GetId(), std::move(element)));
}

void Document::UnregisterElement(const ElementId &elementId) {
    if (const auto it = m_Elements.find(elementId); it != m_Elements.end()) {
        m_Elements.erase(it);
    }
}

ElementId Document::NewElementId() {
    return ElementId(GlobalUniqueId::Instance().NextId());
}

Element *Document::FindElement(const ElementId &elementId) {
    const auto iter = m_Elements.find(elementId);
    if (iter != m_Elements.end()) {
        return iter->second.get();
    }
    return nullptr;
}

//
// Created by ZQD on 2026/5/30.
//

#include "Element.h"

Element::Element() : m_Id(ElementId::InvalidId), m_Document(nullptr) {
    m_Name = "Element";
}

Document *Element::GetDocument() const {
    return m_Document;
}

void Element::SetDocument(Document *doc) {
    m_Document = doc;
}

ElementId Element::GetId() const {
    return m_Id;
}

void Element::SetId(const ElementId &elementId) {
    m_Id = elementId;
}

Element::~Element() = default;

QString Element::GetName() {
    return m_Name;
}

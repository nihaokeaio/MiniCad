//
// Created by ZQD on 2026/6/4.
//

#include "ChangePropertyCommand.h"

#include "Document.h"
#include "Element.h"

ChangePropertyCommand::ChangePropertyCommand(Document *document, ElementId elementId, const std::string &propertyKey,
                                             const PropertyValue &newValue)
    : Command(document), m_ElementId(elementId), m_PropertyKey(propertyKey), m_NewValue(newValue) {
}

void ChangePropertyCommand::Execute() {
    if (!m_ElementId.IsValid() || m_PropertyKey.empty()) {
        return;
    }

    if (const auto element = m_Document->FindElement(m_ElementId)) {
        if (!m_HasOldValue) {
            m_OldValue = element->Properties().Get(m_PropertyKey);
            m_HasOldValue = true;
        }
        element->SetProperty(m_PropertyKey, m_NewValue);
    }
}

void ChangePropertyCommand::Undo() {
    if (!m_HasOldValue) {
        return;
    }

    if (const auto element = m_Document->FindElement(m_ElementId)) {
        element->SetProperty(m_PropertyKey, m_OldValue);
    }
}

void ChangePropertyCommand::Redo() {
    if (const auto element = m_Document->FindElement(m_ElementId)) {
        element->SetProperty(m_PropertyKey, m_NewValue);
    }
}

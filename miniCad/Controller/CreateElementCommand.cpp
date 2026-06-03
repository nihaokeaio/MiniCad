//
// Created by ZQD on 2026/6/3.
//

#include "CreateElementCommand.h"

#include "Document.h"
#include "Element.h"
#include "ElementFactory.h"

CreateElementCommand::CreateElementCommand(Document *document, ElementType elementType)
    : Command(document), m_ElementType(elementType) {
}

CreateElementCommand::~CreateElementCommand() = default;

void CreateElementCommand::Execute() {
    if (m_RemovedElement) {
        m_Document->RegisterElement(std::move(m_RemovedElement));
        return;
    }

    auto element = ElementFactory::Create(m_ElementType);
    if (!element) {
        return;
    }

    const auto rawElement = element.get();
    m_Document->RegisterElement(std::move(element));
    m_ElementId = rawElement->GetId();
}

void CreateElementCommand::Undo() {
    if (m_ElementId.IsValid()) {
        m_RemovedElement = m_Document->UnregisterElement(m_ElementId);
    }
}

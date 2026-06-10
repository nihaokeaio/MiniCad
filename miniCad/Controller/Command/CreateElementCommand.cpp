//
// Created by ZQD on 2026/6/3.
//

#include "CreateElementCommand.h"

#include "Document.h"
#include "../../Data/Element/Element.h"
#include "../../Data/Element/ElementFactory.h"

CreateElementCommand::CreateElementCommand(Document *document, ElementCreateParams params)
    : Command(document), m_Params(std::move(params)) {
}

CreateElementCommand::~CreateElementCommand() = default;

void CreateElementCommand::Execute() {
    if (m_RemovedElement) {
        m_Document->RegisterElement(std::move(m_RemovedElement));
        return;
    }

    auto element = ElementFactory::Create(m_Params.type);
    if (!element) {
        return;
    }

    for (const auto &[key, value]: m_Params.properties) {
        element->Properties().Set(key, value);
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

void CreateElementCommand::Redo() {
    Execute();
}

//
// Created by ZQD on 2026/6/1.
//

#include "CreateBoxCommand.h"

#include "BoxElement.h"
#include "Document.h"
#include "Element.h"

CreateBoxCommand::CreateBoxCommand(Document *document) : Command(document) {
}

CreateBoxCommand::~CreateBoxCommand() = default;

void CreateBoxCommand::Execute() {
    if (m_RemovedElement) {
        m_Document->RegisterElement(std::move(m_RemovedElement));
        return;
    }

    auto box = std::make_unique<BoxElement>();
    box->m_Width = 100;
    box->m_Height = 100;
    box->m_Length = 100;
    const auto rawBox = box.get();
    m_Document->RegisterElement(std::move(box));
    m_ElementId = rawBox->GetId();
}

void CreateBoxCommand::Undo() {
    if (m_ElementId.IsValid()) {
        m_RemovedElement = m_Document->UnregisterElement(m_ElementId);
    }
}

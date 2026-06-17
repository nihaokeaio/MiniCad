//
// Created by ZQD on 2026/6/1.
//

#include "CadController.h"

#include <cassert>

#include "Command/ChangePropertyCommand.h"
#include "Command/CommandManager.h"
#include "Command/CreateElementCommand.h"
#include "Command/TransformElementsCommand.h"
#include "Document.h"

CadController::CadController(Document *document, CommandManager *commandManager)
    : m_Document(document), m_CommandManager(commandManager) {
    assert(m_Document != nullptr);
    assert(m_CommandManager != nullptr);
}

void CadController::CreateElement(ElementType elementType) const {
    CreateElement(ElementCreateParams{elementType, {}});
}

void CadController::CreateElement(const ElementCreateParams &params) const {
    m_CommandManager->ExecuteCommand(std::make_unique<CreateElementCommand>(m_Document, params));
}

void CadController::ChangeElementProperty(ElementId id, const std::string &key, const PropertyValue &value) {
    m_CommandManager->ExecuteCommand(std::make_unique<ChangePropertyCommand>(m_Document, id, key, value));
}

void CadController::TransformElements(const std::vector<ElementTransformChange> &changes) const {
    if (changes.empty()) {
        return;
    }
    m_CommandManager->ExecuteCommand(std::make_unique<TransformElementsCommand>(m_Document, changes));
}

void CadController::Undo() const {
    m_CommandManager->Undo();
}

void CadController::Redo() const {
    m_CommandManager->Redo();
}

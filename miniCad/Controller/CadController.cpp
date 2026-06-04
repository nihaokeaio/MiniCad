//
// Created by ZQD on 2026/6/1.
//

#include "CadController.h"

#include <cassert>

#include "ChangePropertyCommand.h"
#include "CommandManager.h"
#include "CreateElementCommand.h"
#include "Document.h"

CadController::CadController(Document *document, CommandManager *commandManager)
    : m_Document(document), m_CommandManager(commandManager) {
    assert(m_Document != nullptr);
    assert(m_CommandManager != nullptr);
}

void CadController::CreateBox() const {
    m_CommandManager->ExecuteCommand(std::make_unique<CreateElementCommand>(m_Document, ElementType::Box));
}

void CadController::CreateCylinder() const {
    m_CommandManager->ExecuteCommand(std::make_unique<CreateElementCommand>(m_Document, ElementType::Cylinder));
}

void CadController::ChangeElementProperty(ElementId id, const std::string &key, const PropertyValue &value) {
    m_CommandManager->ExecuteCommand(std::make_unique<ChangePropertyCommand>(m_Document, id, key, value));
}

void CadController::Undo() const {
    m_CommandManager->Undo();
}

void CadController::Redo() const {
    m_CommandManager->Redo();
}

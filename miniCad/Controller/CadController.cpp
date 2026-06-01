//
// Created by ZQD on 2026/6/1.
//

#include "CadController.h"

#include <cassert>

#include "CommandManager.h"
#include "CreateBoxCommand.h"
#include "Document.h"

CadController::CadController(Document *document, CommandManager *commandManager)
    : m_Document(document), m_CommandManager(commandManager) {
    assert(m_Document != nullptr);
    assert(m_CommandManager != nullptr);
}

void CadController::CreateBox() const {
    m_CommandManager->ExecuteCommand(std::make_unique<CreateBoxCommand>(m_Document));
}

void CadController::Undo() const {
    m_CommandManager->Undo();
}

void CadController::Redo() const {
    m_CommandManager->Redo();
}

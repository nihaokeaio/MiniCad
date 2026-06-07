//
// Created by ZQD on 2026/6/1.
//

#include "CommandManager.h"

void CommandManager::ExecuteCommand(std::unique_ptr<Command> command) {
    if (!command) {
        return;
    }

    command->Execute();
    m_UndoStack.push_back(std::move(command));
    m_RedoStack.clear();
}

void CommandManager::Undo() {
    if (m_UndoStack.empty()) {
        return;
    }

    auto command = std::move(m_UndoStack.back());
    m_UndoStack.pop_back();
    command->Undo();
    m_RedoStack.push_back(std::move(command));
}

void CommandManager::Redo() {
    if (m_RedoStack.empty()) {
        return;
    }

    auto command = std::move(m_RedoStack.back());
    m_RedoStack.pop_back();
    command->Redo();
    m_UndoStack.push_back(std::move(command));
}

//
// Created by ZQD on 2026/6/1.
//

#pragma once
#include <memory>
#include <vector>

#include "Command.h"


class CommandManager {
public:
    CommandManager() = default;

    void ExecuteCommand(std::unique_ptr<Command> command);

    void Undo();

    void Redo();

private:
    std::vector<std::unique_ptr<Command> > m_UndoStack;
    std::vector<std::unique_ptr<Command> > m_RedoStack;
};



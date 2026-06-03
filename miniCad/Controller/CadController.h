//
// Created by ZQD on 2026/6/1.
//

#pragma once

class CommandManager;
class Document;

class CadController {
public:
    CadController(Document *document, CommandManager *commandManager);

    void CreateBox() const;

    void CreateCylinder() const;

    void Undo() const;

    void Redo() const;

private:
    Document *m_Document = nullptr;
    CommandManager *m_CommandManager = nullptr;
};

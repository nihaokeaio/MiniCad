//
// Created by ZQD on 2026/6/1.
//

#pragma once

class Document;

class Command {
public:
    explicit Command(Document *document);

    virtual ~Command();

    virtual void Execute() = 0;

    virtual void Undo() = 0;

    virtual void Redo() =0;

protected:
    Document *m_Document = nullptr;
};

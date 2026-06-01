//
// Created by ZQD on 2026/6/1.
//

#pragma once
#include <memory>

#include "Command.h"
#include "ElementId.h"

class Element;


class CreateBoxCommand final : public Command {
public:
    explicit CreateBoxCommand(Document *document);

    ~CreateBoxCommand() override;

    void Execute() override;

    void Undo() override;

private:
    ElementId m_ElementId{ElementId::InvalidId};
    std::unique_ptr<Element> m_RemovedElement;
};



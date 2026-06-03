//
// Created by ZQD on 2026/6/3.
//

#pragma once

#include <memory>

#include "Command.h"
#include "ElementId.h"
#include "ElementType.h"

class Element;

class CreateElementCommand final : public Command {
public:
    CreateElementCommand(Document *document, ElementType elementType);

    ~CreateElementCommand() override;

    void Execute() override;

    void Undo() override;

private:
    ElementType m_ElementType;
    ElementId m_ElementId{ElementId::InvalidId};
    std::unique_ptr<Element> m_RemovedElement;
};

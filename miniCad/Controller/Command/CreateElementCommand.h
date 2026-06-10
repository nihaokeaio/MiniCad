//
// Created by ZQD on 2026/6/3.
//

#pragma once

#include <memory>

#include "Command.h"
#include "../../Data/Element/ElementId.h"
#include "../ElementCreateParams.h"

class Element;

class CreateElementCommand final : public Command {
public:
    CreateElementCommand(Document *document, ElementCreateParams params);

    ~CreateElementCommand() override;

    void Execute() override;

    void Undo() override;

    void Redo() override;

private:
    ElementCreateParams m_Params;
    ElementId m_ElementId{ElementId::InvalidId};
    std::unique_ptr<Element> m_RemovedElement;
};

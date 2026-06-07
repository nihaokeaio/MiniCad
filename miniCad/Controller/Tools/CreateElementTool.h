//
// Created by ZQD on 2026/6/7.
//

#pragma once
#include "../Interaction/InteractionHandler.h"
#include "ElementType.h"


class CreateElementTool : public InteractionHandler {
public:
    CreateElementTool(InteractionContext *context, ElementType elementType);

    bool MousePress(QMouseEvent *event) override;

    bool MouseMove(QMouseEvent *event) override;

private:
    ElementType m_ElementType;
};



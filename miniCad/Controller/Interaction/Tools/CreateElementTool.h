//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include "ElementType.h"
#include "../InteractionHandler.h"

class CreateElementTool : public InteractionHandler {
public:
    CreateElementTool(InteractionContext *context, ElementType elementType, bool continuous = false);

    bool MousePress(QMouseEvent *event) override;

    InteractionPostAction OnMousePressAfter(QMouseEvent *event, bool handled) override;

    bool MouseMove(QMouseEvent *event) override;

private:
    ElementType m_ElementType;
    bool m_Continuous = false;
};

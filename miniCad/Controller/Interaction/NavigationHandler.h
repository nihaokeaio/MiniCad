//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include <QPoint>

#include "InteractionHandler.h"

class NavigationHandler : public InteractionHandler {
public:
    explicit NavigationHandler(InteractionContext *context);

    bool MousePress(QMouseEvent *event) override;

    bool MouseMove(QMouseEvent *event) override;

    bool Wheel(QWheelEvent *event) override;

private:
    QPoint m_LastMousePos;
};

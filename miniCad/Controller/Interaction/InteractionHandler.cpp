//
// Created by ZQD on 2026/6/7.
//

#include "InteractionHandler.h"

InteractionHandler::InteractionHandler(InteractionContext *context) : m_Context(context) {
}

bool InteractionHandler::MousePress(QMouseEvent *) {
    return false;
}

InteractionPostAction InteractionHandler::OnMousePressAfter(QMouseEvent *, bool) {
    return InteractionPostAction::None;
}

bool InteractionHandler::MouseRelease(QMouseEvent *) {
    return false;
}

bool InteractionHandler::MouseMove(QMouseEvent *) {
    return false;
}

bool InteractionHandler::Wheel(QWheelEvent *) {
    return false;
}

bool InteractionHandler::KeyPress(const QKeyEvent *event) {
    return false;
}

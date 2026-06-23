//
// Created by ZQD on 2026/6/7.
//

#pragma once
#include <qevent.h>

struct InteractionContext;
class QWheelEvent;
class QMouseEvent;

enum class InteractionPostAction {
    None,
    RestoreSelectionHandler
};

class InteractionHandler {
public:
    explicit InteractionHandler(InteractionContext *context);

    virtual ~InteractionHandler() = default;

    virtual bool MousePress(QMouseEvent *event);

    virtual InteractionPostAction OnMousePressAfter(QMouseEvent *event, bool handled);

    virtual bool MouseRelease(QMouseEvent *event);

    virtual bool MouseMove(QMouseEvent *event);

    virtual bool Wheel(QWheelEvent *event);

    virtual bool KeyPress(const QKeyEvent *event);

protected:
    InteractionContext *m_Context = nullptr;
};

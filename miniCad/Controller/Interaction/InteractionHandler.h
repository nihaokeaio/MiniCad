//
// Created by ZQD on 2026/6/7.
//

#pragma once

struct InteractionContext;
class QWheelEvent;
class QMouseEvent;

class InteractionHandler {
public:
    explicit InteractionHandler(InteractionContext *context);

    virtual ~InteractionHandler() = default;

    virtual bool MousePress(QMouseEvent *event) {
        return false;
    }

    virtual bool MouseRelease(QMouseEvent *event) {
        return false;
    }

    virtual bool MouseMove(QMouseEvent *event) {
        return false;
    }

    virtual bool Wheel(QWheelEvent *event) {
        return false;
    }

protected:
    InteractionContext *m_Context = nullptr;
};

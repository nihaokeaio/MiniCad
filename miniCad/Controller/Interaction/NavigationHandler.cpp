//
// Created by ZQD on 2026/6/7.
//

#include "NavigationHandler.h"

#include <AIS_InteractiveContext.hxx>
#include <QMouseEvent>
#include <QWheelEvent>
#include <V3d_View.hxx>

#include "InteractionManager.h"

NavigationHandler::NavigationHandler(InteractionContext *context) : InteractionHandler(context) {
}

bool NavigationHandler::MousePress(QMouseEvent *event) {
    m_LastMousePos = event->pos();
    if (event->button() == Qt::RightButton) {
        m_Context->view->StartRotation(event->x(), event->y());
        return true;
    }
    return event->button() == Qt::MiddleButton;
}

bool NavigationHandler::MouseMove(QMouseEvent *event) {
    QPoint delta = event->pos() - m_LastMousePos;
    bool handled = false;

    if (event->buttons() & Qt::RightButton) {
        m_Context->view->Rotation(event->x(), event->y());
        handled = true;
    }

    if (event->buttons() & Qt::MiddleButton) {
        m_Context->view->Pan(delta.x(), -delta.y());
        handled = true;
    }

    return handled;
}

bool NavigationHandler::Wheel(QWheelEvent *event) {
    const double factor = event->angleDelta().y() < 0 ? 0.9 : 1.1;
    m_Context->view->SetScale(m_Context->view->Scale() * factor);
    return true;
}

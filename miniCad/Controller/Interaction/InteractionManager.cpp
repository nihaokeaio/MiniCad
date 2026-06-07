//
// Created by ZQD on 2026/6/7.
//

#include "InteractionManager.h"

#include <QMouseEvent>

#include "../Tools/CreateElementTool.h"
#include "NavigationHandler.h"
#include "SelectionHandler.h"

InteractionManager::InteractionManager(std::unique_ptr<InteractionContext> context) {
    m_Context = std::move(context);
    Initiate();
}

InteractionManager::~InteractionManager() = default;

void InteractionManager::Initiate() {
    m_GlobalHandlers.push_back(std::make_unique<NavigationHandler>(m_Context.get()));
    m_ActiveHandler = std::make_unique<SelectionHandler>(m_Context.get());
}

void InteractionManager::SetActiveHandler(std::unique_ptr<InteractionHandler> handler) {
    m_ActiveHandler = std::move(handler);
}

void InteractionManager::SetSelectionHandler() {
    m_ActiveHandler = std::make_unique<SelectionHandler>(m_Context.get());
}

void InteractionManager::SetCreateElementTool(ElementType elementType) {
    m_ActiveHandler = std::make_unique<CreateElementTool>(m_Context.get(), elementType);
}

void InteractionManager::MousePress(QMouseEvent *event) const {
    if (DispatchGlobalMousePress(event)) {
        return;
    }
    if (m_ActiveHandler) {
        m_ActiveHandler->MousePress(event);
    }
}

void InteractionManager::MouseRelease(QMouseEvent *event) const {
    if (DispatchGlobalMouseRelease(event)) {
        return;
    }
    if (m_ActiveHandler) {
        m_ActiveHandler->MouseRelease(event);
    }
}

void InteractionManager::MouseMove(QMouseEvent *event) const {
    if (DispatchGlobalMouseMove(event)) {
        return;
    }
    if (m_ActiveHandler) {
        m_ActiveHandler->MouseMove(event);
    }
}

void InteractionManager::Wheel(QWheelEvent *event) const {
    if (DispatchGlobalWheel(event)) {
        return;
    }
    if (m_ActiveHandler) {
        m_ActiveHandler->Wheel(event);
    }
}

bool InteractionManager::DispatchGlobalMousePress(QMouseEvent *event) const {
    for (const auto &handler: m_GlobalHandlers) {
        if (handler->MousePress(event)) {
            return true;
        }
    }
    return false;
}

bool InteractionManager::DispatchGlobalMouseRelease(QMouseEvent *event) const {
    for (const auto &handler: m_GlobalHandlers) {
        if (handler->MouseRelease(event)) {
            return true;
        }
    }
    return false;
}

bool InteractionManager::DispatchGlobalMouseMove(QMouseEvent *event) const {
    for (const auto &handler: m_GlobalHandlers) {
        if (handler->MouseMove(event)) {
            return true;
        }
    }
    return false;
}

bool InteractionManager::DispatchGlobalWheel(QWheelEvent *event) const {
    for (const auto &handler: m_GlobalHandlers) {
        if (handler->Wheel(event)) {
            return true;
        }
    }
    return false;
}

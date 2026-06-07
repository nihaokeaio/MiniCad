//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include <memory>
#include <Standard_Handle.hxx>
#include <vector>

#include "ElementType.h"

class InteractionHandler;
class AIS_InteractiveContext;
class V3d_View;
class Document;
class ViewObjectRegistry;
class SelectionManager;
class CadController;
class PreviewManager;

class QMouseEvent;
class QWheelEvent;

struct InteractionContext {
    Handle(AIS_InteractiveContext) aisContext;
    Handle(V3d_View) view;
    Document *document = nullptr;
    ViewObjectRegistry *registry = nullptr;
    SelectionManager *selection = nullptr;
    CadController *controller = nullptr;
    PreviewManager *preview = nullptr;
};

class InteractionManager {
public:
    explicit InteractionManager(std::unique_ptr<InteractionContext> context);

    ~InteractionManager();

    void Initiate();

public:
    void SetActiveHandler(std::unique_ptr<InteractionHandler> handler);

    void SetSelectionHandler();

    void SetCreateElementTool(ElementType elementType);

public:
    void MousePress(QMouseEvent *event) const;

    void MouseRelease(QMouseEvent *event) const;

    void MouseMove(QMouseEvent *event) const;

    void Wheel(QWheelEvent *event) const;

private:
    bool DispatchGlobalMousePress(QMouseEvent *event) const;

    bool DispatchGlobalMouseRelease(QMouseEvent *event) const;

    bool DispatchGlobalMouseMove(QMouseEvent *event) const;

    bool DispatchGlobalWheel(QWheelEvent *event) const;

private:
    std::unique_ptr<InteractionContext> m_Context;
    std::vector<std::unique_ptr<InteractionHandler> > m_GlobalHandlers;
    std::unique_ptr<InteractionHandler> m_ActiveHandler;
};

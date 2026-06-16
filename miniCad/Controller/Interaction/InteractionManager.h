//
// Created by ZQD on 2026/6/7.
//

#pragma once

#include <memory>
#include <qevent.h>
#include <Standard_Handle.hxx>
#include <vector>

#include "../../Data/Element/ElementType.h"
#include "InteractionHandler.h"

class Scene;
class ViewController;
class CoordinateResolver;
class AIS_InteractiveContext;
class V3d_View;
class Document;
class ViewObjectRegistry;
class SelectionManager;
class CadController;
class PreviewManager;
class ViewStateAdaptor;

class QMouseEvent;
class QWheelEvent;

struct InteractionContext {
public:
    InteractionContext(const Handle(AIS_InteractiveContext) &aisContext, const Handle(V3d_View) &view,
                       Document *document, ViewObjectRegistry *registry,
                       SelectionManager *selectionManager, CadController *controller, PreviewManager *previewManager,
                       ViewStateAdaptor *viewStateAdaptor, CoordinateResolver *coordinateResolver, Scene *scene);

    ~InteractionContext();

public:
    Handle(AIS_InteractiveContext) m_AisContext;
    Handle(V3d_View) m_View;
    Document *m_Document = nullptr;
    ViewObjectRegistry *m_Registry = nullptr;
    SelectionManager *m_Selection = nullptr;
    CadController *m_Controller = nullptr;
    PreviewManager *m_PreviewManager = nullptr;
    ViewStateAdaptor *m_ViewStateAdaptor = nullptr;
    CoordinateResolver *m_CoordinateResolver = nullptr;
    Scene *m_Scene = nullptr;
};

class InteractionManager {
public:
    explicit InteractionManager(std::unique_ptr<InteractionContext> context);

    ~InteractionManager();

    void Initiate();

public:
    void SetActiveHandler(std::unique_ptr<InteractionHandler> handler);

    void SetSelectionHandler();

    void SetCreateElementTool(ElementType elementType, bool continuous = false);

public:
    void MousePress(QMouseEvent *event);

    void MouseRelease(QMouseEvent *event);

    void MouseMove(QMouseEvent *event);

    void Wheel(QWheelEvent *event);

    void KeyPress(const QKeyEvent *event) const;

private:
    bool DispatchGlobalMousePress(QMouseEvent *event) const;

    bool DispatchGlobalMouseRelease(QMouseEvent *event) const;

    bool DispatchGlobalMouseMove(QMouseEvent *event) const;

    bool DispatchGlobalWheel(QWheelEvent *event) const;

    void ApplyPostAction(InteractionPostAction action);

private:
    std::unique_ptr<InteractionContext> m_Context;
    std::vector<std::unique_ptr<InteractionHandler> > m_GlobalHandlers;
    std::unique_ptr<InteractionHandler> m_ActiveHandler;
    std::unique_ptr<ViewController> m_ViewController;
};

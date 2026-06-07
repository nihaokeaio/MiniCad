//
// Created by ZQD on 2026/5/31.
//

#include "MainWindow.h"

#include "AppContext.h"
#include <QToolBar>

#include "CadView.h"
#include "Element.h"
#include "Controller/CadController.h"
#include "Controller/Interaction/InteractionManager.h"

MainWindow::MainWindow(AppContext *context) : m_Context(context) {
    auto toolbar = addToolBar("Main Toolbar");
    auto action = toolbar->addAction("Print Selection");
    auto createBox = toolbar->addAction("Box");
    auto createCylinder = toolbar->addAction("Cylinder");
    auto addBoxWidth = toolbar->addAction("AddBoxWidth");
    auto undo = toolbar->addAction("Undo");
    auto redo = toolbar->addAction("Redo");

    QObject::connect(action, &QAction::triggered, m_Context->GetCadView(), &CadView::PrintSelection);
    QObject::connect(createBox, &QAction::triggered, this, [context]() {
        context->GetInteractionManager()->SetCreateElementTool(ElementType::Box);
    });
    QObject::connect(createCylinder, &QAction::triggered, this, [context]() {
        context->GetInteractionManager()->SetCreateElementTool(ElementType::Cylinder);
    });
    QObject::connect(addBoxWidth, &QAction::triggered, this, [context]() {
        const auto selection = context->GetSelectManager();
        if (!selection->HasSelection()) {
            return;
        }

        const auto selectedId = selection->GetSingleSelected();
        const auto element = context->GetDocument()->FindElement(selectedId);
        if (!element) {
            return;
        }

        double width = 0.0;
        if (!element->GetProperty("Width", width)) {
            return;
        }

        context->GetCadController()->ChangeElementProperty(selectedId, "Width", PropertyValue(width + 10));
    });
    QObject::connect(undo, &QAction::triggered, this, [context]() {
        context->GetCadController()->Undo();
    });
    QObject::connect(redo, &QAction::triggered, this, [context]() {
        context->GetCadController()->Redo();
    });
}

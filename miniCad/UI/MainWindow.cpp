//
// Created by ZQD on 2026/5/31.
//

#include "MainWindow.h"

#include "AppContext.h"
#include <QToolBar>

#include "CadView.h"
#include "Controller/CadController.h"

MainWindow::MainWindow(AppContext *context) : m_Context(context) {
    auto toolbar = addToolBar("Main Toolbar");
    auto action = toolbar->addAction("Print Selection");
    auto createBox = toolbar->addAction("Box");
    auto undo = toolbar->addAction("Undo");
    auto redo = toolbar->addAction("Redo");

    QObject::connect(action, &QAction::triggered, m_Context->GetCadView(), &CadView::PrintSelection);
    QObject::connect(createBox, &QAction::triggered, this, [context]() {
        context->GetCadController()->CreateBox();
    });
    QObject::connect(undo, &QAction::triggered, this, [context]() {
        context->GetCadController()->Undo();
    });
    QObject::connect(redo, &QAction::triggered, this, [context]() {
        context->GetCadController()->Redo();
    });
}

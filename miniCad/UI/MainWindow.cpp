//
// Created by ZQD on 2026/5/31.
//

#include "MainWindow.h"

#include "AppContext.h"
#include "BoxElement.h"
#include <QToolBar>

#include "CadView.h"

MainWindow::MainWindow(AppContext *context) : m_Context(context) {
    auto toolbar = addToolBar("Main Toolbar");
    auto action = toolbar->addAction("Print Selection");
    auto createBox = toolbar->addAction("Box");

    QObject::connect(action, &QAction::triggered, m_Context->m_View, &CadView::PrintSelection);
    QObject::connect(createBox, &QAction::triggered, this, &MainWindow::CreateBox);
}

void MainWindow::CreateBox() const {
    auto box = std::make_unique<BoxElement>();
    box->m_Width = 100;
    box->m_Height = 100;
    box->m_Length = 100;
    m_Context->m_Document->RegisterElement(std::move(box));
}

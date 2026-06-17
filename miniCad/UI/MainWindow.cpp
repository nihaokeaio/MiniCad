//
// Created by ZQD on 2026/5/31.
//

#include "MainWindow.h"

#include "AppContext.h"
#include <QDebug>
#include <QToolBar>

#include <QElapsedTimer>
#include <random>
#include <utility>

#include "CadView.h"
#include "Document.h"
#include "../Data/Element/Element.h"
#include "../Data/Element/ElementFactory.h"
#include "../Data/Geometry/GeometryTypes.h"
#include "Controller/CadController.h"
#include "Controller/Interaction/InteractionManager.h"

#include <gp_Vec.hxx>

MainWindow::MainWindow(AppContext *context) : m_Context(context) {
    auto toolbar = addToolBar("Main Toolbar");
    auto action = toolbar->addAction("Print Selection");
    auto createBox = toolbar->addAction("Box");
    auto createCylinder = toolbar->addAction("Cylinder");
    auto createPoint = toolbar->addAction("Point");
    auto createSegment = toolbar->addAction("Segment");
    auto addBoxWidth = toolbar->addAction("AddBoxWidth");
    auto randomBenchmark = toolbar->addAction("Random10K");
    auto moveTool = toolbar->addAction("MoveTool");
    auto undo = toolbar->addAction("Undo");
    auto redo = toolbar->addAction("Redo");

    QObject::connect(action, &QAction::triggered, m_Context->GetCadView(), &CadView::PrintSelection);
    QObject::connect(createBox, &QAction::triggered, this, [context]() {
        context->GetInteractionManager()->SetCreateElementTool(ElementType::Box);
    });
    QObject::connect(createCylinder, &QAction::triggered, this, [context]() {
        context->GetInteractionManager()->SetCreateElementTool(ElementType::Cylinder);
    });
    QObject::connect(createPoint, &QAction::triggered, this, [context]() {
        context->GetInteractionManager()->SetCreateElementTool(ElementType::Point);
    });
    QObject::connect(createSegment, &QAction::triggered, this, [context]() {
        context->GetInteractionManager()->SetCreateElementTool(ElementType::Segment);
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

    QObject::connect(moveTool, &QAction::triggered, this, [context]() {
        context->GetInteractionManager()->SetMoveTool();
    });

    QObject::connect(randomBenchmark, &QAction::triggered, this, [this]() {
        CreateRandomBenchmarkBoxes();
    });
    QObject::connect(undo, &QAction::triggered, this, [context]() {
        context->GetCadController()->Undo();
    });
    QObject::connect(redo, &QAction::triggered, this, [context]() {
        context->GetCadController()->Redo();
    });
}

void MainWindow::CreateRandomBenchmarkBoxes() const {
    constexpr int modelCount = 1000;
    std::mt19937 rng{42};
    std::uniform_real_distribution<double> positionDist{-5000.0, 5000.0};
    std::uniform_real_distribution<double> sizeDist{20.0, 80.0};

    QElapsedTimer timer;
    timer.start();

    auto document = m_Context->GetDocument();
    for (int index = 0; index < modelCount; ++index) {
        auto element = ElementFactory::Create(ElementType::Box);
        if (!element) {
            continue;
        }

        element->Properties().Set("Length", PropertyValue(sizeDist(rng)));
        element->Properties().Set("Width", PropertyValue(sizeDist(rng)));
        element->Properties().Set("Height", PropertyValue(sizeDist(rng)));

        GeometryTypes::RTransform transform;
        transform.SetTranslation(gp_Vec(positionDist(rng), positionDist(rng), positionDist(rng) * 0.1));
        element->Properties().Set("LocalTransform", PropertyValue(transform));

        document->RegisterElement(std::move(element));
    }

    qDebug() << "[PickBenchmark]"
            << "created random boxes:" << modelCount
            << "elapsed:" << timer.elapsed() << "ms";
}

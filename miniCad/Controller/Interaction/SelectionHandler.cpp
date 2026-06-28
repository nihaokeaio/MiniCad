//
// Created by ZQD on 2026/6/7.
//

#include "SelectionHandler.h"

#include <QDebug>
#include <QMouseEvent>
#include <variant>

#include "GeomCalculator.h"
#include "../../Data/Element/ElementId.h"
#include "InteractionManager.h"
#include "SelectionManager.h"
#include "Timer.h"
#include "Presentation/ViewState/ViewStateAdaptor.h"
#include "Picking/ScenePicker.h"

SelectionHandler::SelectionHandler(InteractionContext *context) : InteractionHandler(context) {
    m_ScenePicker = std::make_unique<ScenePicker>(context->m_Scene);
}

bool SelectionHandler::MousePress(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        return false;
    }
    return true;
}

bool SelectionHandler::MouseRelease(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        return false;
    }

    const gp_Lin ray(GeomCalculator::GetMouseScreenRay(event->x(), event->y(), m_Context->m_View));
    PickSettings bvhSettings;
    Timer timer;
    const auto pick = m_ScenePicker->Pick(PickQuery{ray, bvhSettings});
    const auto bvhUs = timer.StopMicroseconds(true);


    PickSettings linearSettings;
    const auto linearPick = m_ScenePicker->Pick(PickQuery{ray, linearSettings});
    const auto linearUs = timer.StopMicroseconds();

    const auto pickedElement = pick.has_value() ? std::get_if<ElementPickTarget>(&pick->pickTarget) : nullptr;
    const auto linearPickedElement = linearPick.has_value()
                                         ? std::get_if<ElementPickTarget>(&linearPick->pickTarget)
                                         : nullptr;

    qDebug() << "[PickBenchmark]"
             << "BVH:" << bvhUs << "us"
             << "Linear:" << linearUs << "us"
             << "BVH hit:" << (pickedElement != nullptr ? pickedElement->elementId.GetValue() : 0)
             << "Linear hit:" << (linearPickedElement != nullptr ? linearPickedElement->elementId.GetValue() : 0);

    if (!pick.has_value()) {
        m_Context->m_Selection->Clear();
        if (m_Context->m_ViewStateAdaptor) {
            m_Context->m_ViewStateAdaptor->ClearSelection();
        }
        return true;
    }

    const auto elementTarget = std::get_if<ElementPickTarget>(&pick->pickTarget);
    if (elementTarget == nullptr) {
        return true;
    }

    m_Context->m_Selection->SetSelected(*elementTarget);
    if (m_Context->m_ViewStateAdaptor) {
        m_Context->m_ViewStateAdaptor->ApplySelection(*m_Context->m_Selection);
    }
    return true;
}

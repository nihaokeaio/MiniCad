//
// Created by ZQD on 2026/6/7.
//

#include "SelectionHandler.h"

#include <AIS_InteractiveContext.hxx>
#include <QMouseEvent>

#include "GeomCalculator.h"
#include "../../Data/Element/ElementId.h"
#include "InteractionManager.h"
#include "SelectionManager.h"
#include "ViewObjectRegistry.h"
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
    const auto pick = m_ScenePicker->PickByBoundingBox(ray);
    m_Context->m_AisContext->ClearSelected(Standard_False);

    if (!pick.has_value()) {
        m_Context->m_Selection->Clear();
        m_Context->m_AisContext->UpdateCurrentViewer();
        return true;
    }

    m_Context->m_Selection->SetSelected(pick->elementId);
    for (const auto &aisObject: m_Context->m_Registry->FindElementAisObjects(pick->elementId)) {
        if (!aisObject.IsNull()) {
            m_Context->m_AisContext->SetSelected(aisObject, Standard_False);
        }
    }
    m_Context->m_AisContext->UpdateCurrentViewer();
    return true;
}

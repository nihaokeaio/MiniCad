//
// Created by ZQD on 2026/6/7.
//

#include "SelectionHandler.h"

#include <AIS_InteractiveContext.hxx>
#include <QMouseEvent>
#include <Standard_TypeDef.hxx>

#include "ElementId.h"
#include "InteractionManager.h"
#include "SelectionManager.h"
#include "ViewObjectRegistry.h"

SelectionHandler::SelectionHandler(InteractionContext *context) : InteractionHandler(context) {
}

bool SelectionHandler::MouseRelease(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        return false;
    }

    // TODO: Replace OCCT AIS picking with custom ray/BVH picking.
    m_Context->m_AisContext->MoveTo(event->x(), event->y(), m_Context->m_View, Standard_True);

    if (!m_Context->m_AisContext->HasDetected()) {
        m_Context->m_AisContext->ClearSelected(Standard_True);
        m_Context->m_Selection->Clear();
        return true;
    }

    m_Context->m_AisContext->SelectDetected();
    m_Context->m_AisContext->InitSelected();
    if (!m_Context->m_AisContext->MoreSelected()) {
        m_Context->m_Selection->Clear();
        return true;
    }
    const auto ais = m_Context->m_AisContext->SelectedInteractive();
    if (ais.IsNull()) {
        m_Context->m_Selection->Clear();
        return true;
    }

    const ElementId id = m_Context->m_Registry->FindElement(ais);
    m_Context->m_Selection->SetSelected(id);
    return true;
}

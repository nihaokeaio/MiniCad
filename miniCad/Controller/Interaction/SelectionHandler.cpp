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
    m_Context->aisContext->MoveTo(event->x(), event->y(), m_Context->view, Standard_True);

    if (!m_Context->aisContext->HasDetected()) {
        m_Context->aisContext->ClearSelected(Standard_True);
        m_Context->selection->Clear();
        return true;
    }

    m_Context->aisContext->SelectDetected();
    m_Context->aisContext->InitSelected();
    if (!m_Context->aisContext->MoreSelected()) {
        m_Context->selection->Clear();
        return true;
    }
    const auto ais = m_Context->aisContext->SelectedInteractive();
    if (ais.IsNull()) {
        m_Context->selection->Clear();
        return true;
    }

    const ElementId id = m_Context->registry->FindElement(ais);
    m_Context->selection->SetSelected(id);
    return true;
}

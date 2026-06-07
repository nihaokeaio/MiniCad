//
// Created by ZQD on 2026/6/7.
//

#include "CreateElementTool.h"

#include <vector>
#include <AIS_InteractiveContext.hxx>
#include "../CadController.h"
#include "../ElementCreateParams.h"
#include "../Preview/PreviewManager.h"
#include "../Interaction/InteractionManager.h"
#include <QMouseEvent>
#include <V3d_View.hxx>

CreateElementTool::CreateElementTool(InteractionContext *context, ElementType elementType)
    : InteractionHandler(context), m_ElementType(elementType) {
    if (m_Context->preview) {
        m_Context->preview->BeginElementPreview(ElementCreateParams{m_ElementType, {}});
    }
}

bool CreateElementTool::MousePress(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton)
        return false;

    m_Context->aisContext->MoveTo(event->x(), event->y(), m_Context->view, Standard_True);

    Standard_Real X, Y, Z;
    m_Context->view->Convert(event->x(), event->y(), X, Y, Z);
    ElementCreateParams params{m_ElementType, {{"Position", PropertyValue(std::vector<double>{X, Y, Z})}}};
    m_Context->controller->CreateElement(params);
    if (m_Context->preview) {
        m_Context->preview->Clear();
    }
    return true;
}

bool CreateElementTool::MouseMove(QMouseEvent *event) {
    m_Context->aisContext->MoveTo(event->x(), event->y(), m_Context->view, Standard_True);
    if (m_Context->preview) {
        Standard_Real X, Y, Z;
        m_Context->view->Convert(event->x(), event->y(), X, Y, Z);
        m_Context->preview->UpdateElementPreviewPosition({X, Y, Z});
    }
    return false;
}

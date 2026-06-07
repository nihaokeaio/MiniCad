//
// Created by ZQD on 2026/6/7.
//

#include "CreateElementTool.h"

#include <QMouseEvent>
#include <V3d_View.hxx>

#include "GeometryTypes.h"
#include "../../CadController.h"
#include "../../ElementCreateParams.h"
#include "../../Preview/PreviewManager.h"
#include "../InteractionManager.h"

CreateElementTool::CreateElementTool(InteractionContext *context, ElementType elementType, bool continuous)
    : InteractionHandler(context), m_ElementType(elementType), m_Continuous(continuous) {
    if (m_Context->preview) {
        m_Context->preview->BeginElementPreview(ElementCreateParams{m_ElementType, {}});
    }
}

bool CreateElementTool::MousePress(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        return false;
    }

    Standard_Real x, y, z;
    m_Context->view->Convert(event->x(), event->y(), x, y, z);
    ElementCreateParams params{
        m_ElementType,
        {{"Position", PropertyValue(GeometryTypes::Point3D(x, y, z))}}
    };
    m_Context->controller->CreateElement(params);
    if (m_Context->preview) {
        m_Context->preview->ExitPreviewState();
    }
    return true;
}

InteractionPostAction CreateElementTool::OnMousePressAfter(QMouseEvent *, bool handled) {
    if (!handled) {
        return InteractionPostAction::None;
    }

    if (m_Continuous) {
        if (m_Context->preview) {
            m_Context->preview->BeginElementPreview(ElementCreateParams{m_ElementType, {}});
        }
        return InteractionPostAction::None;
    }

    return InteractionPostAction::RestoreSelectionHandler;
}

bool CreateElementTool::MouseMove(QMouseEvent *event) {
    if (m_Context->preview) {
        Standard_Real x, y, z;
        m_Context->view->Convert(event->x(), event->y(), x, y, z);
        ElementCreateParams params{
            m_ElementType,
            {{"Position", PropertyValue(GeometryTypes::Point3D(x, y, z))}}
        };
        m_Context->preview->UpdateElementPreview(params);
    }
    return false;
}

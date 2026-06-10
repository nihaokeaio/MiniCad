//
// Created by ZQD on 2026/6/7.
//

#include "CreateElementTool.h"

#include <QMouseEvent>
#include <V3d_View.hxx>
#include <gp_Vec.hxx>

#include "../../../Data/Geometry/GeometryTypes.h"
#include "../../CadController.h"
#include "../../ElementCreateParams.h"
#include "../../Preview/PreviewManager.h"
#include "../InteractionManager.h"
#include "Controller/Interaction/CoordinateResolver.h"

CreateElementTool::CreateElementTool(InteractionContext *context, ElementType elementType, bool continuous)
    : InteractionHandler(context), m_ElementType(elementType), m_Continuous(continuous) {
    if (m_Context->m_PreviewManager) {
        m_Context->m_PreviewManager->BeginElementPreview(ElementCreateParams{m_ElementType, {}});
    }
}

bool CreateElementTool::MousePress(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        return false;
    }

    auto ints = m_Context->m_CoordinateResolver->ScreenToWorkPlane(event->x(), event->y());
    if (!ints.has_value()) {
        return false;
    }
    GeometryTypes::RTransform transform;
    transform.SetTranslation(gp_Vec(ints.value()));
    ElementCreateParams params{
        m_ElementType,
        {{"LocalTransform", PropertyValue(transform)}}
    };
    m_Context->m_Controller->CreateElement(params);
    if (m_Context->m_PreviewManager) {
        m_Context->m_PreviewManager->ExitPreviewState();
    }
    return true;
}

InteractionPostAction CreateElementTool::OnMousePressAfter(QMouseEvent *, bool handled) {
    if (!handled) {
        return InteractionPostAction::None;
    }

    if (m_Continuous) {
        if (m_Context->m_PreviewManager) {
            m_Context->m_PreviewManager->BeginElementPreview(ElementCreateParams{m_ElementType, {}});
        }
        return InteractionPostAction::None;
    }

    return InteractionPostAction::RestoreSelectionHandler;
}

bool CreateElementTool::MouseMove(QMouseEvent *event) {
    if (m_Context->m_PreviewManager) {
        auto ints = m_Context->m_CoordinateResolver->ScreenToWorkPlane(event->x(), event->y());
        if (!ints.has_value()) {
            return false;
        }
        GeometryTypes::RTransform transform;
        transform.SetTranslation(gp_Vec(ints.value()));
        ElementCreateParams params{
            m_ElementType,
            {{"LocalTransform", PropertyValue(transform)}}
        };
        m_Context->m_PreviewManager->UpdateElementPreview(params);
    }
    return false;
}

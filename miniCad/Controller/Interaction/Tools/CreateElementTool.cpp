//
// Created by ZQD on 2026/6/7.
//

#include "CreateElementTool.h"

#include <QMouseEvent>
#include <V3d_View.hxx>
#include <gp_Vec.hxx>

#include "../../../Data/Geometry/GeometryTypes.h"
#include "Element/Element.h"
#include "Element/ElementFactory.h"
#include "../../CadController.h"
#include "../../ElementCreateParams.h"
#include "../InteractionManager.h"
#include "Controller/Interaction/CoordinateResolver.h"
#include "Presentation/ViewState/ViewStateAdaptor.h"

namespace {
    void BeginElementPreview(ViewStateAdaptor *viewStateAdaptor, ElementType elementType) {
        if (viewStateAdaptor == nullptr) {
            return;
        }

        const auto element = ElementFactory::Create(elementType);
        if (!element) {
            return;
        }

        viewStateAdaptor->ShowPreviewShape(element->BuildShape());
    }

    void UpdateElementPreview(ViewStateAdaptor *viewStateAdaptor, const GeometryTypes::RTransform &transform) {
        if (viewStateAdaptor == nullptr) {
            return;
        }

        viewStateAdaptor->UpdatePreviewTransform(transform);
    }
}

CreateElementTool::CreateElementTool(InteractionContext *context, ElementType elementType, bool continuous)
    : InteractionHandler(context), m_ElementType(elementType), m_Continuous(continuous) {
    BeginElementPreview(m_Context->m_ViewStateAdaptor, m_ElementType);
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
    if (m_Context->m_ViewStateAdaptor) {
        m_Context->m_ViewStateAdaptor->ClearPreview();
    }
    return true;
}

InteractionPostAction CreateElementTool::OnMousePressAfter(QMouseEvent *, bool handled) {
    if (!handled) {
        return InteractionPostAction::None;
    }

    if (m_Continuous) {
        BeginElementPreview(m_Context->m_ViewStateAdaptor, m_ElementType);
        return InteractionPostAction::None;
    }

    return InteractionPostAction::RestoreSelectionHandler;
}

bool CreateElementTool::MouseMove(QMouseEvent *event) {
    auto ints = m_Context->m_CoordinateResolver->ScreenToWorkPlane(event->x(), event->y());
    if (!ints.has_value()) {
        return false;
    }

    GeometryTypes::RTransform transform;
    transform.SetTranslation(gp_Vec(ints.value()));
    UpdateElementPreview(m_Context->m_ViewStateAdaptor, transform);
    return false;
}

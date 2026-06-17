//
// Created by ZQD on 2026/6/17.
//

#include "TransformElementTool.h"

#include "Document.h"
#include "SelectionManager.h"

#include <optional>
#include <QMouseEvent>
#include <unordered_set>
#include <Bnd_Box.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include "Controller/CadController.h"
#include "Controller/Interaction/CoordinateResolver.h"
#include "Controller/Interaction/InteractionManager.h"
#include "Element/Element.h"
#include "Presentation/ViewStateAdaptor.h"

namespace {
    std::optional<Types::Point3f> SelectionCenter(Document *document, const std::unordered_set<ElementId> &selected) {
        if (document == nullptr || selected.empty()) {
            return std::nullopt;
        }

        Bnd_Box bounds;
        for (const auto elementId: selected) {
            const auto element = document->FindElement(elementId);
            if (element == nullptr) {
                continue;
            }

            const auto elementBounds = element->GetBoundingBox();
            if (!elementBounds.IsVoid()) {
                bounds.Add(elementBounds);
            }
        }

        if (bounds.IsVoid()) {
            return std::nullopt;
        }

        return Types::Point3f((bounds.CornerMin().XYZ() + bounds.CornerMax().XYZ()) * 0.5);
    }
}

TransformElementTool::TransformElementTool(InteractionContext *context) : InteractionHandler(context) {
}

TransformElementTool::~TransformElementTool() {
    CancelDragState();
}

bool TransformElementTool::MousePress(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton || m_TransformState == TransformState::Dragging) {
        return false;
    }

    return BeginDrag(event);
}

bool TransformElementTool::MouseRelease(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton || m_TransformState != TransformState::Dragging) {
        return false;
    }

    const bool updatedReleasePoint = UpdateDrag(event);
    (void)updatedReleasePoint;
    m_Context->m_Controller->TransformElements(m_TransformChanges);
    ClearDragState();
    return true;
}

bool TransformElementTool::MouseMove(QMouseEvent *event) {
    if (m_TransformState != TransformState::Dragging) {
        return false;
    }

    return UpdateDrag(event);
}

InteractionPostAction TransformElementTool::OnMousePressAfter(QMouseEvent *, bool) {
    return InteractionPostAction::None;
}

bool TransformElementTool::BeginDrag(QMouseEvent *event) {
    if (m_Context == nullptr ||
        m_Context->m_Document == nullptr ||
        m_Context->m_Selection == nullptr ||
        m_Context->m_CoordinateResolver == nullptr) {
        return false;
    }

    const auto &selected = m_Context->m_Selection->Selected();
    if (selected.empty()) {
        return false;
    }

    const auto center = SelectionCenter(m_Context->m_Document, selected);
    if (!center.has_value()) {
        return false;
    }

    m_DragPlane = gp_Pln(*center, gp_Dir(0.0, 0.0, 1.0));
    const auto startPoint = m_Context->m_CoordinateResolver->ScreenToPlane(event->x(), event->y(), m_DragPlane);
    if (!startPoint.has_value()) {
        return false;
    }

    m_StartPoint = *startPoint;
    m_CurrentPoint = m_StartPoint;
    m_TransformChanges.clear();
    m_TransformChanges.reserve(selected.size());

    for (const auto elementId: selected) {
        const auto element = m_Context->m_Document->FindElement(elementId);
        if (element == nullptr) {
            continue;
        }

        const auto oldTransform = element->GetLocalTransform();
        m_TransformChanges.push_back(ElementTransformChange{elementId, oldTransform, oldTransform});
    }

    if (m_TransformChanges.empty()) {
        return false;
    }

    m_TransformState = TransformState::Dragging;
    return true;
}

bool TransformElementTool::UpdateDrag(QMouseEvent *event) {
    const auto currentPoint = m_Context->m_CoordinateResolver->ScreenToPlane(event->x(), event->y(), m_DragPlane);
    if (!currentPoint.has_value()) {
        return false;
    }

    m_CurrentPoint = *currentPoint;
    ApplyDelta(m_CurrentPoint);
    return true;
}

void TransformElementTool::ApplyDelta(const Types::Point3f &currentPoint) {
    const auto delta = currentPoint.XYZ() - m_StartPoint.XYZ();
    std::vector<ElementViewTransform> viewTransforms;
    viewTransforms.reserve(m_TransformChanges.size());

    for (auto &change: m_TransformChanges) {
        auto newTransform = change.oldTransform;
        newTransform.SetTranslationPart(gp_Vec(change.oldTransform.TranslationPart() + delta));
        change.newTransform = newTransform;
        viewTransforms.push_back(ElementViewTransform{change.elementId, newTransform});
    }

    if (m_Context->m_ViewStateAdaptor != nullptr) {
        m_Context->m_ViewStateAdaptor->ApplyElementTransforms(viewTransforms);
    }
}

void TransformElementTool::RestoreOriginalTransforms() {
    std::vector<ElementViewTransform> viewTransforms;
    viewTransforms.reserve(m_TransformChanges.size());

    for (const auto &change: m_TransformChanges) {
        viewTransforms.push_back(ElementViewTransform{change.elementId, change.oldTransform});
    }

    if (m_Context != nullptr && m_Context->m_ViewStateAdaptor != nullptr) {
        m_Context->m_ViewStateAdaptor->ApplyElementTransforms(viewTransforms);
    }
}

void TransformElementTool::ClearDragState() {
    m_TransformChanges.clear();
    m_TransformState = TransformState::Idle;
}

void TransformElementTool::CancelDragState() {
    if (m_TransformState != TransformState::Dragging) {
        return;
    }

    RestoreOriginalTransforms();
    ClearDragState();
}

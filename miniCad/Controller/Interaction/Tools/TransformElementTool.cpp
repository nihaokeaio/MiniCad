//
// Created by ZQD on 2026/6/17.
//

#include "TransformElementTool.h"

#include "Document.h"
#include "SelectionManager.h"

#include <algorithm>
#include <optional>
#include <QMouseEvent>
#include <unordered_set>
#include <Bnd_Box.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include "Controller/CadController.h"
#include "Controller/Interaction/CoordinateResolver.h"
#include "Controller/Interaction/InteractionManager.h"
#include "Element/Element.h"
#include "GeomCalculator.h"
#include "Presentation/ViewState/TransformGuideAdaptor.h"
#include "Presentation/ViewState/ViewStateAdaptor.h"

using namespace TransformElementSpace;

namespace {
    constexpr double MinScaleFactor = 0.01;
    constexpr double ScaleSensitivity = 0.01;

    std::optional<Types::Point3f> SelectionCenter(Document *document, const std::unordered_set<ElementId> &selected) {
        if (!document || selected.empty()) {
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

    gp_Dir AxisDirection(TransformConstraint constraint) {
        switch (constraint) {
            case TransformConstraint::XAxis:
                return gp::DX();
            case TransformConstraint::YAxis:
                return gp::DY();
            case TransformConstraint::ZAxis:
                return gp::DZ();
            case TransformConstraint::Free:
            default:
                return gp::DZ();
        }
    }

    double AxisParameter(const gp_Pnt &point, const gp_Lin &axis) {
        return gp_Vec(axis.Location(), point).Dot(gp_Vec(axis.Direction()));
    }

    std::optional<double> ClosestAxisParameter(const gp_Lin &ray, const gp_Lin &axis) {
        const gp_XYZ rayOrigin = ray.Location().XYZ();
        const gp_XYZ rayDirection = ray.Direction().XYZ();
        const gp_XYZ axisOrigin = axis.Location().XYZ();
        const gp_XYZ axisDirection = axis.Direction().XYZ();
        const gp_XYZ offset = rayOrigin - axisOrigin;

        const double a = rayDirection.Dot(rayDirection);
        const double b = rayDirection.Dot(axisDirection);
        const double c = axisDirection.Dot(axisDirection);
        const double d = rayDirection.Dot(offset);
        const double e = axisDirection.Dot(offset);
        const double denominator = a * c - b * b;
        if (std::abs(denominator) <= gp::Resolution()) {
            return std::nullopt;
        }

        return (a * e - b * d) / denominator;
    }

    double SignedAngleAroundAxis(const gp_Vec &from, const gp_Vec &to, const gp_Dir &axis) {
        if (from.SquareMagnitude() <= gp::Resolution() || to.SquareMagnitude() <= gp::Resolution()) {
            return 0.0;
        }

        gp_Vec fromUnit = from;
        gp_Vec toUnit = to;
        fromUnit.Normalize();
        toUnit.Normalize();

        const double cosAngle = std::clamp(fromUnit.Dot(toUnit), -1.0, 1.0);
        double angle = std::acos(cosAngle);
        if (fromUnit.Crossed(toUnit).Dot(gp_Vec(axis)) < 0.0) {
            angle = -angle;
        }
        return angle;
    }
}

TransformElementTool::TransformElementTool(InteractionContext *context) : InteractionHandler(context) {
    RefreshGuideFromSelection();
}

TransformElementTool::~TransformElementTool() {
    CancelDragState();
    HideTransformGuide();
}

bool TransformElementTool::MousePress(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton || m_Session.state == TransformState::Dragging) {
        return false;
    }

    return BeginDrag(event);
}

bool TransformElementTool::MouseRelease(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton || m_Session.state != TransformState::Dragging) {
        return false;
    }

    const bool updatedReleasePoint = UpdateDrag(event);
    (void)updatedReleasePoint;
    m_Context->m_Controller->TransformElements(m_Session.changes);
    ClearDragState();
    return true;
}

bool TransformElementTool::MouseMove(QMouseEvent *event) {
    if (m_Session.state != TransformState::Dragging) {
        return false;
    }

    return UpdateDrag(event);
}

bool TransformElementTool::KeyPress(const QKeyEvent *event) {
    bool handled = true;
    switch (event->key()) {
        case Qt::Key_G:
            m_Session.mode = TransformMode::Move;
            break;
        case Qt::Key_R:
            m_Session.mode = TransformMode::Rotate;
            break;
        case Qt::Key_S:
            m_Session.mode = TransformMode::Scale;
            break;
        case Qt::Key_X:
            m_Session.constraint = TransformConstraint::XAxis;
            break;
        case Qt::Key_Y:
            m_Session.constraint = TransformConstraint::YAxis;
            break;
        case Qt::Key_Z:
            m_Session.constraint = TransformConstraint::ZAxis;
            break;
        default:
            handled = false;
            break;
    }

    if (!handled) {
        return InteractionHandler::KeyPress(event);
    }

    UpdateIntersectionPlane();
    return true;
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

    m_Session.pivot = center.value();
    m_Session.guideVisible = true;
    UpdateIntersectionPlane();
    const auto startPoint = m_Context->m_CoordinateResolver->ScreenToPlane(event->x(), event->y(), m_Session.dragPlane);
    if (!startPoint.has_value()) {
        return false;
    }

    m_Session.startPoint = startPoint.value();
    m_Session.currentPoint = m_Session.startPoint;
    m_Session.changes.clear();
    m_Session.changes.reserve(selected.size());

    for (const auto elementId: selected) {
        const auto element = m_Context->m_Document->FindElement(elementId);
        if (element == nullptr) {
            continue;
        }

        const auto oldTransform = element->GetLocalTransform();
        m_Session.changes.push_back(ElementTransformChange{elementId, oldTransform, oldTransform});
    }

    if (m_Session.changes.empty()) {
        return false;
    }

    m_Session.state = TransformState::Dragging;
    UpdateTransformGuideState();
    return true;
}

bool TransformElementTool::UpdateDrag(QMouseEvent *event) {
    const auto delta = ResolveConstrainedDelta(event);
    ApplyDelta(delta);
    return true;
}

void TransformElementTool::ApplyDelta(const gp_Vec &delta) {
    std::vector<ElementViewTransform> viewTransforms;
    viewTransforms.reserve(m_Session.changes.size());

    for (auto &change: m_Session.changes) {
        auto newTransform = change.oldTransform;
        newTransform = BuildTransform(delta) * newTransform;
        change.newTransform = newTransform;
        viewTransforms.push_back(ElementViewTransform{change.elementId, newTransform});
    }

    if (m_Context->m_ViewStateAdaptor != nullptr) {
        m_Context->m_ViewStateAdaptor->ApplyElementTransforms(viewTransforms);
    }
}

GeometryTypes::RTransform TransformElementTool::BuildTransform(const gp_Vec &delta) const {
    switch (m_Session.mode) {
        case TransformMode::Rotate:
            return BuildRotateTransform(delta);
        case TransformMode::Scale:
            return BuildScaleTransform(delta);
        case TransformMode::Move:
        default:
            return BuildMoveTransform(delta);
    }
}

GeometryTypes::RTransform TransformElementTool::BuildMoveTransform(const gp_Vec &delta) const {
    GeometryTypes::RTransform transform;
    switch (m_Session.constraint) {
        case TransformConstraint::XAxis: {
            transform.SetTranslation(gp_Vec(delta.X(), 0.0, 0.0));
            return transform;
        }
        case TransformConstraint::YAxis: {
            transform.SetTranslation(gp_Vec(0.0, delta.Y(), 0.0));
            return transform;
        }
        case TransformConstraint::ZAxis: {
            transform.SetTranslation(gp_Vec(0.0, 0.0, delta.Z()));
            return transform;
        }
        default:
            transform.SetTranslation(delta);
            return transform;
    }
}

GeometryTypes::RTransform TransformElementTool::BuildRotateTransform(const gp_Vec &delta) const {
    GeometryTypes::RTransform transform;
    const gp_Dir axis = m_Session.constraint == TransformConstraint::Free
                            ? m_Session.dragPlane.Axis().Direction()
                            : AxisDirection(m_Session.constraint);

    const gp_Vec startVector(m_Session.pivot, m_Session.startPoint);
    const gp_Vec currentVector(m_Session.pivot, m_Session.startPoint.Translated(delta));
    const double angle = SignedAngleAroundAxis(startVector, currentVector, axis);
    transform.SetRotation(gp_Ax1(m_Session.pivot, axis), angle);
    return transform;
}

GeometryTypes::RTransform TransformElementTool::BuildScaleTransform(const gp_Vec &delta) const {
    GeometryTypes::RTransform transform;

    double scaleDelta = 0.0;
    if (m_Session.constraint == TransformConstraint::Free) {
        const double startDistance = m_Session.pivot.Distance(m_Session.startPoint);
        const double currentDistance = m_Session.pivot.Distance(m_Session.startPoint.Translated(delta));
        if (startDistance > gp::Resolution()) {
            const double scale = std::max(currentDistance / startDistance, MinScaleFactor);
            transform.SetScale(m_Session.pivot, scale);
            return transform;
        }
    } else {
        scaleDelta = delta.Dot(gp_Vec(AxisDirection(m_Session.constraint)));
    }

    const double scale = std::max(1.0 + scaleDelta * ScaleSensitivity, MinScaleFactor);
    transform.SetScale(m_Session.pivot, scale);
    return transform;
}

gp_Vec TransformElementTool::ResolveConstrainedDelta(const QMouseEvent *event) const {
    if (m_Session.constraint != TransformConstraint::Free &&
        (m_Session.mode == TransformMode::Move || m_Session.mode == TransformMode::Scale)) {
        return ResolveAxisDelta(event);
    }

    return ResolvePlaneDelta(event);
}

gp_Vec TransformElementTool::ResolveAxisDelta(const QMouseEvent *event) const {
    const gp_Ax1 mouseRay = GeomCalculator::GetMouseScreenRay(event->x(), event->y(), m_Context->m_View);
    const gp_Lin ray(mouseRay.Location(), mouseRay.Direction());
    const auto currentAxisParameter = ClosestAxisParameter(ray, m_Session.constraintAxis);
    if (!currentAxisParameter.has_value()) {
        return {};
    }

    const double startAxisParameter = AxisParameter(m_Session.startPoint, m_Session.constraintAxis);
    return gp_Vec(m_Session.constraintAxis.Direction()) * (currentAxisParameter.value() - startAxisParameter);
}

gp_Vec TransformElementTool::ResolvePlaneDelta(const QMouseEvent *event) const {
    const auto currentPoint = m_Context->m_CoordinateResolver->ScreenToPlane(
        event->x(), event->y(), m_Session.dragPlane);
    if (!currentPoint.has_value()) {
        return {};
    }
    const auto delta = currentPoint.value() - m_Session.startPoint.XYZ();
    return delta;
}

void TransformElementTool::UpdateIntersectionPlane() {
    switch (m_Session.constraint) {
        case TransformConstraint::XAxis: {
            m_Session.constraintAxis = gp_Lin(m_Session.pivot, gp::DX());
            if (m_Session.mode == TransformMode::Rotate) {
                m_Session.dragPlane = gp_Pln(m_Session.pivot, gp::DX());
            } else {
                m_Session.dragPlane = gp_Pln(m_Session.pivot, gp::DY());
            }
            break;
        }
        case TransformConstraint::YAxis: {
            m_Session.constraintAxis = gp_Lin(m_Session.pivot, gp::DY());
            if (m_Session.mode == TransformMode::Rotate) {
                m_Session.dragPlane = gp_Pln(m_Session.pivot, gp::DY());
            } else {
                m_Session.dragPlane = gp_Pln(m_Session.pivot, gp::DZ());
            }
            break;
        }
        case TransformConstraint::ZAxis: {
            m_Session.constraintAxis = gp_Lin(m_Session.pivot, gp::DZ());
            if (m_Session.mode == TransformMode::Rotate) {
                m_Session.dragPlane = gp_Pln(m_Session.pivot, gp::DZ());
            } else {
                m_Session.dragPlane = gp_Pln(m_Session.pivot, gp::DX());
            }
            break;
        }
        default:
            m_Session.constraintAxis = gp_Lin(m_Session.pivot, gp::DZ());
            m_Session.dragPlane = gp_Pln(m_Session.pivot, gp::DZ());
    }
    UpdateTransformGuideState();
}

void TransformElementTool::RestoreOriginalTransforms() {
    std::vector<ElementViewTransform> viewTransforms;
    viewTransforms.reserve(m_Session.changes.size());

    for (const auto &change: m_Session.changes) {
        viewTransforms.push_back(ElementViewTransform{change.elementId, change.oldTransform});
    }

    if (m_Context != nullptr && m_Context->m_ViewStateAdaptor != nullptr) {
        m_Context->m_ViewStateAdaptor->ApplyElementTransforms(viewTransforms);
    }
}

void TransformElementTool::ClearDragState() {
    m_Session.changes.clear();
    m_Session.state = TransformState::Idle;
    m_Session.constraint = TransformConstraint::Free;
    if (!RefreshGuideFromSelection()) {
        HideTransformGuide();
    }
}

void TransformElementTool::CancelDragState() {
    if (m_Session.state != TransformState::Dragging) {
        return;
    }

    RestoreOriginalTransforms();
    ClearDragState();
}

bool TransformElementTool::RefreshGuideFromSelection() {
    if (m_Context == nullptr || m_Context->m_Document == nullptr || m_Context->m_Selection == nullptr) {
        return false;
    }

    const auto center = SelectionCenter(m_Context->m_Document, m_Context->m_Selection->Selected());
    if (!center.has_value()) {
        return false;
    }

    m_Session.pivot = center.value();
    m_Session.guideVisible = true;
    UpdateIntersectionPlane();
    return true;
}

void TransformElementTool::HideTransformGuide() {
    m_Session.guideVisible = false;
    if (m_Context == nullptr || m_Context->m_ViewStateAdaptor == nullptr) {
        return;
    }

    m_Context->m_ViewStateAdaptor->ShowTransformGuide(std::make_shared<TransformGuideState>());
}

void TransformElementTool::UpdateTransformGuideState() const {
    if (m_Context == nullptr || m_Context->m_ViewStateAdaptor == nullptr) {
        return;
    }

    const auto transGuideState = std::make_shared<TransformGuideState>();
    transGuideState->visible = m_Session.guideVisible;
    transGuideState->pivot = m_Session.pivot;
    transGuideState->constraint = m_Session.constraint;
    transGuideState->mode = m_Session.mode;
    m_Context->m_ViewStateAdaptor->ShowTransformGuide(transGuideState);
}

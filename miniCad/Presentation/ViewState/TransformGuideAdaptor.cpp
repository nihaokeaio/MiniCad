//
// Created by ZQD on 2026/6/25.
//

#include "TransformGuideAdaptor.h"

#include <Quantity_Color.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include "Primitive/PresentationPrimitiveBuilder.h"

using namespace TransformElementSpace;

namespace {
    constexpr double GuideLength = 180.0;
    constexpr double ConstraintLineLength = 5000.0;
    constexpr double ArrowLength = 24.0;
    constexpr double ArrowWidth = 10.0;
    constexpr double RotateRadius = 120.0;
    constexpr double CenterRadius = 16.0;
    constexpr double NormalWidth = 2.0;
    constexpr double HighlightWidth = 4.0;

    enum class GuideVisualState {
        Normal,
        Hovered,
        Active
    };

    GizmoMode ToGizmoMode(TransformMode mode) {
        switch (mode) {
            case TransformMode::Rotate:
                return GizmoMode::Rotate;
            case TransformMode::Scale:
                return GizmoMode::Scale;
            case TransformMode::Move:
            default:
                return GizmoMode::Move;
        }
    }

    Quantity_Color AxisColor(TransformConstraint constraint, GuideVisualState visualState) {
        const bool highlight = visualState != GuideVisualState::Normal;
        if (!highlight) {
            switch (constraint) {
                case TransformConstraint::XAxis:
                    return {0.55, 0.12, 0.10, Quantity_TOC_RGB};
                case TransformConstraint::YAxis:
                    return {0.12, 0.48, 0.12, Quantity_TOC_RGB};
                case TransformConstraint::ZAxis:
                    return {0.12, 0.25, 0.58, Quantity_TOC_RGB};
                case TransformConstraint::XYPlane:
                    return {0.12, 0.25, 0.58, Quantity_TOC_RGB};
                case TransformConstraint::YZPlane:
                    return {0.55, 0.12, 0.10, Quantity_TOC_RGB};
                case TransformConstraint::ZXPlane:
                    return {0.12, 0.48, 0.12, Quantity_TOC_RGB};
                case TransformConstraint::Free:
                default:
                    return {0.55, 0.55, 0.55, Quantity_TOC_RGB};
            }
        }

        switch (constraint) {
            case TransformConstraint::XAxis:
                return {1.0, 0.08, 0.05, Quantity_TOC_RGB};
            case TransformConstraint::YAxis:
                return {0.15, 1.0, 0.15, Quantity_TOC_RGB};
            case TransformConstraint::ZAxis:
                return {0.15, 0.45, 1.0, Quantity_TOC_RGB};
            case TransformConstraint::XYPlane:
                return {0.15, 0.45, 1.0, Quantity_TOC_RGB};
            case TransformConstraint::YZPlane:
                return {1.0, 0.08, 0.05, Quantity_TOC_RGB};
            case TransformConstraint::ZXPlane:
                return {0.15, 1.0, 0.15, Quantity_TOC_RGB};
            case TransformConstraint::Free:
            default:
                return {0.9, 0.9, 0.9, Quantity_TOC_RGB};
        }
    }

    PresentationPrimitive::LineStyle GuideStyle(TransformConstraint constraint, GuideVisualState visualState) {
        return {
            AxisColor(constraint, visualState),
            visualState == GuideVisualState::Normal ? NormalWidth : HighlightWidth
        };
    }

    gp_Dir AxisDirection(TransformConstraint constraint) {
        switch (constraint) {
            case TransformConstraint::XAxis:
                return gp::DX();
            case TransformConstraint::YAxis:
                return gp::DY();
            case TransformConstraint::ZAxis:
                return gp::DZ();
            case TransformConstraint::XYPlane:
                return gp::DZ();
            case TransformConstraint::YZPlane:
                return gp::DX();
            case TransformConstraint::ZXPlane:
                return gp::DY();
            case TransformConstraint::Free:
            default:
                return gp::DZ();
        }
    }

    gp_Dir ArrowPerpendicular(TransformConstraint constraint) {
        switch (constraint) {
            case TransformConstraint::XAxis:
                return gp::DY();
            case TransformConstraint::YAxis:
            case TransformConstraint::ZAxis:
            case TransformConstraint::XYPlane:
            case TransformConstraint::YZPlane:
            case TransformConstraint::ZXPlane:
            case TransformConstraint::Free:
            default:
                return gp::DX();
        }
    }

    bool IsPlaneConstraint(TransformConstraint constraint) {
        return constraint == TransformConstraint::XYPlane ||
               constraint == TransformConstraint::YZPlane ||
               constraint == TransformConstraint::ZXPlane;
    }

    bool IsAxisConstraint(TransformConstraint constraint) {
        return constraint == TransformConstraint::XAxis ||
               constraint == TransformConstraint::YAxis ||
               constraint == TransformConstraint::ZAxis;
    }

    void PlaneBasis(TransformConstraint plane, gp_Dir &u, gp_Dir &v) {
        switch (plane) {
            case TransformConstraint::XYPlane:
                u = gp::DX();
                v = gp::DY();
                break;
            case TransformConstraint::YZPlane:
                u = gp::DY();
                v = gp::DZ();
                break;
            case TransformConstraint::ZXPlane:
                u = gp::DZ();
                v = gp::DX();
                break;
            default:
                u = gp::DX();
                v = gp::DY();
                break;
        }
    }

    TransformConstraint PlaneFirstAxis(TransformConstraint plane) {
        switch (plane) {
            case TransformConstraint::XYPlane:
                return TransformConstraint::XAxis;
            case TransformConstraint::YZPlane:
                return TransformConstraint::YAxis;
            case TransformConstraint::ZXPlane:
                return TransformConstraint::ZAxis;
            default:
                return TransformConstraint::XAxis;
        }
    }

    TransformConstraint PlaneSecondAxis(TransformConstraint plane) {
        switch (plane) {
            case TransformConstraint::XYPlane:
                return TransformConstraint::YAxis;
            case TransformConstraint::YZPlane:
                return TransformConstraint::ZAxis;
            case TransformConstraint::ZXPlane:
                return TransformConstraint::XAxis;
            default:
                return TransformConstraint::YAxis;
        }
    }

    GuideVisualState VisualStateForHandle(const TransformGuideState &state, const GizmoHandleId &handleId) {
        if (state.activeGizmoHandleId.has_value() && state.activeGizmoHandleId.value() == handleId) {
            return GuideVisualState::Active;
        }

        if (state.constraint != TransformConstraint::Free &&
            state.constraint == handleId.constraint &&
            ToGizmoMode(state.mode) == handleId.mode) {
            return GuideVisualState::Active;
        }

        if (state.hoveredGizmoHandleId.has_value() && state.hoveredGizmoHandleId.value() == handleId) {
            return GuideVisualState::Hovered;
        }

        return GuideVisualState::Normal;
    }

    PresentationPrimitive::LineStyle StyleForHandle(const TransformGuideState &state, const GizmoHandleId &handleId) {
        return GuideStyle(handleId.constraint, VisualStateForHandle(state, handleId));
    }

    GizmoHandleId MakeHandle(TransformMode mode, TransformConstraint constraint, GizmoHandleType type) {
        return {ToGizmoMode(mode), constraint, type};
    }

    void AddLine(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                 const gp_Pnt &from,
                 const gp_Pnt &to,
                 const PresentationPrimitive::LineStyle &style,
                 std::optional<GizmoHandleId> handleId = std::nullopt) {
        objects.push_back({PresentationPrimitive::MakeLine(from, to, style), handleId});
    }

    void AddCircle(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                   const gp_Pnt &center,
                   const gp_Dir &normal,
                   double radius,
                   const PresentationPrimitive::LineStyle &style,
                   std::optional<GizmoHandleId> handleId = std::nullopt) {
        objects.push_back({PresentationPrimitive::MakeCircle(center, normal, radius, style), handleId});
    }

    void AddAxisGuide(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                      const TransformGuideState &state,
                      TransformConstraint axis) {
        const GizmoHandleId handleId = MakeHandle(state.mode, axis, GizmoHandleType::Axis);
        const auto style = StyleForHandle(state, handleId);
        const gp_Dir direction = AxisDirection(axis);
        const gp_Dir perpendicular = ArrowPerpendicular(axis);
        const gp_Pnt from = state.pivot;
        const gp_Pnt to = state.pivot.Translated(gp_Vec(direction) * GuideLength);

        AddLine(objects, from, to, style, handleId);

        const gp_Pnt wingBase = to.Translated(gp_Vec(direction) * -ArrowLength);
        AddLine(objects, to, wingBase.Translated(gp_Vec(perpendicular) * ArrowWidth), style, handleId);
        AddLine(objects, to, wingBase.Translated(gp_Vec(perpendicular) * -ArrowWidth), style, handleId);
    }

    void AddPlaneGuide(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                       const TransformGuideState &state,
                       TransformConstraint plane) {
        constexpr double PlaneOffset = 52.0;
        constexpr double PlaneSize = 32.0;

        gp_Dir u;
        gp_Dir v;
        PlaneBasis(plane, u, v);

        const gp_Vec uOffset = gp_Vec(u) * PlaneOffset;
        const gp_Vec vOffset = gp_Vec(v) * PlaneOffset;
        const gp_Vec uSize = gp_Vec(u) * PlaneSize;
        const gp_Vec vSize = gp_Vec(v) * PlaneSize;

        const gp_Pnt p0 = state.pivot.Translated(uOffset + vOffset);
        const gp_Pnt p1 = p0.Translated(uSize);
        const gp_Pnt p2 = p0.Translated(uSize + vSize);
        const gp_Pnt p3 = p0.Translated(vSize);
        const GizmoHandleId handleId = MakeHandle(state.mode, plane, GizmoHandleType::Plane);
        const auto style = StyleForHandle(state, handleId);

        AddLine(objects, p0, p1, style, handleId);
        AddLine(objects, p1, p2, style, handleId);
        AddLine(objects, p2, p3, style, handleId);
        AddLine(objects, p3, p0, style, handleId);
    }

    void AddPlaneConstraintLines(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                                 const gp_Pnt &pivot,
                                 TransformConstraint plane) {
        gp_Dir u;
        gp_Dir v;
        PlaneBasis(plane, u, v);

        AddLine(
            objects,
            pivot.Translated(gp_Vec(u) * -ConstraintLineLength),
            pivot.Translated(gp_Vec(u) * ConstraintLineLength),
            GuideStyle(PlaneFirstAxis(plane), GuideVisualState::Active));
        AddLine(
            objects,
            pivot.Translated(gp_Vec(v) * -ConstraintLineLength),
            pivot.Translated(gp_Vec(v) * ConstraintLineLength),
            GuideStyle(PlaneSecondAxis(plane), GuideVisualState::Active));
    }

    void AddMoveScaleGuide(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                           const TransformGuideState &state) {
        if (state.mode == TransformMode::Move) {
            const GizmoHandleId centerHandleId = MakeHandle(
                state.mode,
                TransformConstraint::Free,
                GizmoHandleType::Center);
            AddCircle(
                objects,
                state.pivot,
                gp::DZ(),
                CenterRadius,
                StyleForHandle(state, centerHandleId),
                centerHandleId);
            AddPlaneGuide(objects, state, TransformConstraint::XYPlane);
            AddPlaneGuide(objects, state, TransformConstraint::YZPlane);
            AddPlaneGuide(objects, state, TransformConstraint::ZXPlane);
        }

        AddAxisGuide(objects, state, TransformConstraint::XAxis);
        AddAxisGuide(objects, state, TransformConstraint::YAxis);
        AddAxisGuide(objects, state, TransformConstraint::ZAxis);
    }

    void AddConstraintLine(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                           const gp_Pnt &pivot,
                           TransformConstraint constraint) {
        const gp_Dir direction = AxisDirection(constraint);
        const gp_Vec axisVector(direction);
        AddLine(
            objects,
            pivot.Translated(axisVector * -ConstraintLineLength),
            pivot.Translated(axisVector * ConstraintLineLength),
            GuideStyle(constraint, GuideVisualState::Active));
    }

    void AddRotateGuide(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                        const TransformGuideState &state) {
        for (const auto axis: {TransformConstraint::XAxis, TransformConstraint::YAxis, TransformConstraint::ZAxis}) {
            const GizmoHandleId handleId = MakeHandle(state.mode, axis, GizmoHandleType::Ring);
            AddCircle(
                objects,
                state.pivot,
                AxisDirection(axis),
                RotateRadius,
                StyleForHandle(state, handleId),
                handleId);
        }
    }

    void AddMainTransformGuide(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                               const TransformGuideState &state) {
        switch (state.mode) {
            case TransformMode::Rotate:
                if (state.constraint == TransformConstraint::Free) {
                    AddRotateGuide(objects, state);
                } else {
                    const GizmoHandleId handleId = MakeHandle(state.mode, state.constraint, GizmoHandleType::Ring);
                    AddCircle(
                        objects,
                        state.pivot,
                        AxisDirection(state.constraint),
                        RotateRadius,
                        StyleForHandle(state, handleId),
                        handleId);
                }
                break;
            case TransformMode::Scale:
            case TransformMode::Move:
            default:
                if (state.constraint == TransformConstraint::Free) {
                    AddMoveScaleGuide(objects, state);
                } else if (state.mode == TransformMode::Move && IsPlaneConstraint(state.constraint)) {
                    AddMoveScaleGuide(objects, state);
                    AddPlaneGuide(objects, state, state.constraint);
                } else if (state.mode == TransformMode::Move && IsAxisConstraint(state.constraint)) {
                    AddMoveScaleGuide(objects, state);
                } else {
                    AddAxisGuide(objects, state, state.constraint);
                }
                break;
        }
    }

    void AddConstraintTransformGuide(std::vector<TransformGuideAdaptor::GuideObject> &objects,
                                     const TransformGuideState &state) {
        if (state.constraint == TransformConstraint::Free) {
            return;
        }

        if (state.mode == TransformMode::Move && IsPlaneConstraint(state.constraint)) {
            AddPlaneConstraintLines(objects, state.pivot, state.constraint);
            return;
        }

        AddConstraintLine(objects, state.pivot, state.constraint);
    }

    gp_Trsf TranslationTransform(const gp_Pnt &point) {
        gp_Trsf transform;
        transform.SetTranslation(gp_Vec(gp::Origin(), point));
        return transform;
    }
}

TransformGuideAdaptor::TransformGuideAdaptor(const opencascade::handle<AIS_InteractiveContext> &context) : m_Context(
    context) {
}

void TransformGuideAdaptor::ShowTransformGuide(const std::shared_ptr<TransformGuideState> &transformGuideState) {
    if (m_Context.IsNull() || transformGuideState == nullptr || !transformGuideState->visible) {
        ClearObjects(false);
        if (!m_Context.IsNull()) {
            m_Context->UpdateCurrentViewer();
        }
        return;
    }

    if (!m_Topology.Matches(*transformGuideState)) {
        RebuildObjects(*transformGuideState);
    }

    UpdateObjectStyles(*transformGuideState);
    UpdateObjectTransforms(*transformGuideState);
    m_Context->UpdateCurrentViewer();
}

void TransformGuideAdaptor::ClearTransformGuide() {
    ClearObjects(true);
}

bool TransformGuideAdaptor::TopologyKey::Matches(const TransformGuideState &state) const {
    return valid && mode == state.mode && constraint == state.constraint;
}

void TransformGuideAdaptor::RebuildObjects(const TransformGuideState &state) {
    ClearObjects(false);

    TransformGuideState localState = state;
    localState.pivot = gp::Origin();
    localState.constraintPivot = gp::Origin();

    AddMainTransformGuide(m_MainGuideObjects, localState);
    AddConstraintTransformGuide(m_ConstraintGuideObjects, localState);

    for (const auto &guideObject: m_MainGuideObjects) {
        if (!guideObject.object.IsNull()) {
            m_Context->Display(guideObject.object, 0, -1, Standard_False);
        }
    }
    for (const auto &guideObject: m_ConstraintGuideObjects) {
        if (!guideObject.object.IsNull()) {
            m_Context->Display(guideObject.object, 0, -1, Standard_False);
        }
    }

    m_Topology.mode = state.mode;
    m_Topology.constraint = state.constraint;
    m_Topology.valid = true;
}

void TransformGuideAdaptor::UpdateObjectTransforms(const TransformGuideState &state) const {
    const gp_Trsf constraintTransform = TranslationTransform(state.constraintPivot.value_or(state.pivot));

    for (const auto &guideObject: m_MainGuideObjects) {
        if (!guideObject.object.IsNull()) {
            guideObject.object->SetLocalTransformation(state.transform);
        }
    }
    for (const auto &guideObject: m_ConstraintGuideObjects) {
        if (!guideObject.object.IsNull()) {
            guideObject.object->SetLocalTransformation(constraintTransform);
        }
    }
}

void TransformGuideAdaptor::UpdateObjectStyles(const TransformGuideState &state) const {
    for (const auto &guideObject: m_MainGuideObjects) {
        if (guideObject.object.IsNull() || !guideObject.handleId.has_value()) {
            continue;
        }

        PresentationPrimitive::ApplyLineStyle(guideObject.object, StyleForHandle(state, guideObject.handleId.value()));
        m_Context->Redisplay(guideObject.object, Standard_False);
    }
}

void TransformGuideAdaptor::ClearObjects(bool updateViewer) {
    if (m_Context.IsNull()) {
        m_MainGuideObjects.clear();
        m_ConstraintGuideObjects.clear();
        m_Topology.valid = false;
        return;
    }

    for (const auto &guideObject: m_MainGuideObjects) {
        if (!guideObject.object.IsNull()) {
            m_Context->Remove(guideObject.object, Standard_False);
        }
    }
    for (const auto &guideObject: m_ConstraintGuideObjects) {
        if (!guideObject.object.IsNull()) {
            m_Context->Remove(guideObject.object, Standard_False);
        }
    }
    m_MainGuideObjects.clear();
    m_ConstraintGuideObjects.clear();
    m_Topology.valid = false;

    if (updateViewer) {
        m_Context->UpdateCurrentViewer();
    }
}

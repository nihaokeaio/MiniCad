//
// Created by ZQD on 2026/6/25.
//

#include "TransformGuideAdaptor.h"

#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Edge.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

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

    Quantity_Color AxisColor(TransformConstraint constraint, bool highlight) {
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

    double AxisWidth(const TransformGuideState &state, TransformConstraint axis) {
        return state.constraint == TransformConstraint::Free || state.constraint == axis
                   ? HighlightWidth
                   : NormalWidth;
    }

    Handle(AIS_Shape) MakeLineObject(const gp_Pnt &from,
                                     const gp_Pnt &to,
                                     const Quantity_Color &color,
                                     double width) {
        const TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(from, to);
        Handle(AIS_Shape) ais = new AIS_Shape(edge);
        ais->SetColor(color);
        ais->Attributes()->SetLineAspect(new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width));
        ais->Attributes()->SetWireAspect(new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width));
        return ais;
    }

    Handle(AIS_Shape) MakeCircleObject(const gp_Pnt &center,
                                       const gp_Dir &normal,
                                       const Quantity_Color &color,
                                       double width) {
        const gp_Circ circle(gp_Ax2(center, normal), RotateRadius);
        const TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
        Handle(AIS_Shape) ais = new AIS_Shape(edge);
        ais->SetColor(color);
        ais->Attributes()->SetLineAspect(new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width));
        ais->Attributes()->SetWireAspect(new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width));
        return ais;
    }

    Handle(AIS_Shape) MakeCircleObject(const gp_Pnt &center,
                                       const gp_Dir &normal,
                                       double radius,
                                       const Quantity_Color &color,
                                       double width) {
        const gp_Circ circle(gp_Ax2(center, normal), radius);
        const TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
        Handle(AIS_Shape) ais = new AIS_Shape(edge);
        ais->SetColor(color);
        ais->Attributes()->SetLineAspect(new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width));
        ais->Attributes()->SetWireAspect(new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width));
        return ais;
    }

    void AddLine(std::vector<Handle(AIS_InteractiveObject)> &objects,
                 const gp_Pnt &from,
                 const gp_Pnt &to,
                 const Quantity_Color &color,
                 double width) {
        objects.push_back(MakeLineObject(from, to, color, width));
    }

    void AddAxisGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
                      const TransformGuideState &state,
                      TransformConstraint axis) {
        const bool highlight = state.constraint == TransformConstraint::Free || state.constraint == axis;
        const Quantity_Color color = AxisColor(axis, highlight);
        const double width = AxisWidth(state, axis);
        const gp_Dir direction = AxisDirection(axis);
        const gp_Dir perpendicular = ArrowPerpendicular(axis);
        const gp_Pnt from = state.pivot;
        const gp_Pnt to = state.pivot.Translated(gp_Vec(direction) * GuideLength);

        AddLine(objects, from, to, color, width);

        const gp_Pnt wingBase = to.Translated(gp_Vec(direction) * -ArrowLength);
        AddLine(objects, to, wingBase.Translated(gp_Vec(perpendicular) * ArrowWidth), color, width);
        AddLine(objects, to, wingBase.Translated(gp_Vec(perpendicular) * -ArrowWidth), color, width);
    }

    void AddPlaneGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
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
        const Quantity_Color color = AxisColor(plane, true);

        AddLine(objects, p0, p1, color, HighlightWidth);
        AddLine(objects, p1, p2, color, HighlightWidth);
        AddLine(objects, p2, p3, color, HighlightWidth);
        AddLine(objects, p3, p0, color, HighlightWidth);
    }

    void AddPlaneConstraintLines(std::vector<Handle(AIS_InteractiveObject)> &objects,
                                 const gp_Pnt &pivot,
                                 TransformConstraint plane) {
        gp_Dir u;
        gp_Dir v;
        PlaneBasis(plane, u, v);

        TransformConstraint uAxis = TransformConstraint::XAxis;
        TransformConstraint vAxis = TransformConstraint::YAxis;
        switch (plane) {
            case TransformConstraint::XYPlane:
                uAxis = TransformConstraint::XAxis;
                vAxis = TransformConstraint::YAxis;
                break;
            case TransformConstraint::YZPlane:
                uAxis = TransformConstraint::YAxis;
                vAxis = TransformConstraint::ZAxis;
                break;
            case TransformConstraint::ZXPlane:
                uAxis = TransformConstraint::ZAxis;
                vAxis = TransformConstraint::XAxis;
                break;
            default:
                break;
        }

        AddLine(
            objects,
            pivot.Translated(gp_Vec(u) * -ConstraintLineLength),
            pivot.Translated(gp_Vec(u) * ConstraintLineLength),
            AxisColor(uAxis, true),
            HighlightWidth);
        AddLine(
            objects,
            pivot.Translated(gp_Vec(v) * -ConstraintLineLength),
            pivot.Translated(gp_Vec(v) * ConstraintLineLength),
            AxisColor(vAxis, true),
            HighlightWidth);
    }

    void AddMoveScaleGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
                           const TransformGuideState &state) {
        if (state.mode == TransformMode::Move && state.constraint == TransformConstraint::Free) {
            objects.push_back(MakeCircleObject(
                state.pivot,
                gp::DZ(),
                CenterRadius,
                AxisColor(TransformConstraint::Free, true),
                HighlightWidth));
            AddPlaneGuide(objects, state, TransformConstraint::XYPlane);
            AddPlaneGuide(objects, state, TransformConstraint::YZPlane);
            AddPlaneGuide(objects, state, TransformConstraint::ZXPlane);
        }
        AddAxisGuide(objects, state, TransformConstraint::XAxis);
        AddAxisGuide(objects, state, TransformConstraint::YAxis);
        AddAxisGuide(objects, state, TransformConstraint::ZAxis);
    }

    void AddConstraintLine(std::vector<Handle(AIS_InteractiveObject)> &objects,
                           const gp_Pnt &pivot,
                           TransformConstraint constraint) {
        const gp_Dir direction = AxisDirection(constraint);
        const gp_Vec axisVector(direction);
        const Quantity_Color color = AxisColor(constraint, true);
        AddLine(
            objects,
            pivot.Translated(axisVector * -ConstraintLineLength),
            pivot.Translated(axisVector * ConstraintLineLength),
            color,
            HighlightWidth);
    }

    void AddRotateGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
                        const TransformGuideState &state) {
        for (const auto axis: {TransformConstraint::XAxis, TransformConstraint::YAxis, TransformConstraint::ZAxis}) {
            const bool highlight = state.constraint == TransformConstraint::Free || state.constraint == axis;
            objects.push_back(MakeCircleObject(
                state.pivot,
                AxisDirection(axis),
                AxisColor(axis, highlight),
                AxisWidth(state, axis)));
        }
    }

    void AddMainTransformGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
                               const TransformGuideState &state) {
        switch (state.mode) {
            case TransformMode::Rotate:
                if (state.constraint == TransformConstraint::Free) {
                    AddRotateGuide(objects, state);
                } else {
                    objects.push_back(MakeCircleObject(
                        state.pivot,
                        AxisDirection(state.constraint),
                        AxisColor(state.constraint, true),
                        HighlightWidth));
                }
                break;
            case TransformMode::Scale:
            case TransformMode::Move:
            default:
                if (state.constraint == TransformConstraint::Free) {
                    AddMoveScaleGuide(objects, state);
                } else if (state.mode == TransformMode::Move && IsPlaneConstraint(state.constraint)) {
                    TransformGuideState freeState = state;
                    freeState.constraint = TransformConstraint::Free;
                    AddMoveScaleGuide(objects, freeState);
                    AddPlaneGuide(objects, state, state.constraint);
                } else if (state.mode == TransformMode::Move && IsAxisConstraint(state.constraint)) {
                    TransformGuideState freeState = state;
                    freeState.constraint = TransformConstraint::Free;
                    AddMoveScaleGuide(objects, freeState);
                } else {
                    AddAxisGuide(objects, state, state.constraint);
                }
                break;
        }
    }

    void AddConstraintTransformGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
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

    for (const auto &object: m_MainGuideObjects) {
        if (!object.IsNull()) {
            m_Context->Display(object, 0, -1, Standard_False);
        }
    }
    for (const auto &object: m_ConstraintGuideObjects) {
        if (!object.IsNull()) {
            m_Context->Display(object, 0, -1, Standard_False);
        }
    }

    m_Topology.mode = state.mode;
    m_Topology.constraint = state.constraint;
    m_Topology.valid = true;
}

void TransformGuideAdaptor::UpdateObjectTransforms(const TransformGuideState &state) const {
    const gp_Trsf mainTransform = TranslationTransform(state.pivot);
    const gp_Trsf constraintTransform = TranslationTransform(state.constraintPivot.value_or(state.pivot));

    for (const auto &object: m_MainGuideObjects) {
        if (!object.IsNull()) {
            object->SetLocalTransformation(mainTransform);
        }
    }
    for (const auto &object: m_ConstraintGuideObjects) {
        if (!object.IsNull()) {
            object->SetLocalTransformation(constraintTransform);
        }
    }
}

void TransformGuideAdaptor::ClearObjects(bool updateViewer) {
    if (m_Context.IsNull()) {
        m_MainGuideObjects.clear();
        m_ConstraintGuideObjects.clear();
        m_Topology.valid = false;
        return;
    }

    for (const auto &object: m_MainGuideObjects) {
        if (!object.IsNull()) {
            m_Context->Remove(object, Standard_False);
        }
    }
    for (const auto &object: m_ConstraintGuideObjects) {
        if (!object.IsNull()) {
            m_Context->Remove(object, Standard_False);
        }
    }
    m_MainGuideObjects.clear();
    m_ConstraintGuideObjects.clear();
    m_Topology.valid = false;

    if (updateViewer) {
        m_Context->UpdateCurrentViewer();
    }
}

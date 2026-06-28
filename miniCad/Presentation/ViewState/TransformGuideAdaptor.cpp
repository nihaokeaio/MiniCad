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
#include <gp_Vec.hxx>

using namespace TransformElementSpace;

namespace {
    constexpr double GuideLength = 180.0;
    constexpr double ConstraintLineLength = 5000.0;
    constexpr double ArrowLength = 24.0;
    constexpr double ArrowWidth = 10.0;
    constexpr double RotateRadius = 120.0;
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
            case TransformConstraint::Free:
            default:
                return gp::DX();
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

    void AddMoveScaleGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
                           const TransformGuideState &state) {
        AddAxisGuide(objects, state, TransformConstraint::XAxis);
        AddAxisGuide(objects, state, TransformConstraint::YAxis);
        AddAxisGuide(objects, state, TransformConstraint::ZAxis);
    }

    void AddConstraintLine(std::vector<Handle(AIS_InteractiveObject)> &objects,
                           const TransformGuideState &state) {
        const gp_Dir direction = AxisDirection(state.constraint);
        const gp_Vec axisVector(direction);
        const Quantity_Color color = AxisColor(state.constraint, true);
        AddLine(
            objects,
            state.pivot.Translated(axisVector * -ConstraintLineLength),
            state.pivot.Translated(axisVector * ConstraintLineLength),
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

    void AddConstrainedRotateGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
                                   const TransformGuideState &state) {
        AddConstraintLine(objects, state);
        objects.push_back(MakeCircleObject(
            state.pivot,
            AxisDirection(state.constraint),
            AxisColor(state.constraint, true),
            HighlightWidth));
    }

    void AddTransformGuide(std::vector<Handle(AIS_InteractiveObject)> &objects,
                           const TransformGuideState &state) {
        switch (state.mode) {
            case TransformMode::Rotate:
                if (state.constraint == TransformConstraint::Free) {
                    AddRotateGuide(objects, state);
                } else {
                    AddConstrainedRotateGuide(objects, state);
                }
                break;
            case TransformMode::Scale:
            case TransformMode::Move:
            default:
                if (state.constraint == TransformConstraint::Free) {
                    AddMoveScaleGuide(objects, state);
                } else {
                    AddConstraintLine(objects, state);
                }
                break;
        }
    }
}

TransformGuideAdaptor::TransformGuideAdaptor(const opencascade::handle<AIS_InteractiveContext> &context) : m_Context(
    context) {
}

void TransformGuideAdaptor::ShowTransformGuide(const std::shared_ptr<TransformGuideState> &transformGuideState) {
    ClearObjects(false);
    if (m_Context.IsNull() || transformGuideState == nullptr || !transformGuideState->visible) {
        if (!m_Context.IsNull()) {
            m_Context->UpdateCurrentViewer();
        }
        return;
    }

    AddTransformGuide(m_GuideObjects, *transformGuideState);

    for (const auto &object: m_GuideObjects) {
        if (!object.IsNull()) {
            m_Context->Display(object, 0, -1, Standard_False);
        }
    }
    m_Context->UpdateCurrentViewer();
}

void TransformGuideAdaptor::ClearTransformGuide() {
    ClearObjects(true);
}

void TransformGuideAdaptor::ClearObjects(bool updateViewer) {
    if (m_Context.IsNull()) {
        m_GuideObjects.clear();
        return;
    }

    for (const auto &object: m_GuideObjects) {
        if (!object.IsNull()) {
            m_Context->Remove(object, Standard_False);
        }
    }
    m_GuideObjects.clear();

    if (updateViewer) {
        m_Context->UpdateCurrentViewer();
    }
}

//
// Created by ZQD on 2026/6/25.
//

#pragma once

#include <memory>
#include <optional>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <vector>

#include "Controller/Interaction/TransformTypes.h"
#include "Controller/Interaction/Gizmo/GizmoTypes.h"
#include "Geometry/GeometryTypes.h"


struct TransformGuideState {
    bool visible = false;
    gp_Pnt pivot;
    std::optional<gp_Pnt> constraintPivot;
    TransformElementSpace::TransformMode mode{TransformElementSpace::TransformMode::Move};
    TransformElementSpace::TransformConstraint constraint{TransformElementSpace::TransformConstraint::Free};
    std::optional<GizmoHandleId> hoveredGizmoHandleId;
    std::optional<GizmoHandleId> activeGizmoHandleId;
    GeometryTypes::RTransform transform;
};


class TransformGuideAdaptor {
public:
    struct GuideObject {
        Handle(AIS_Shape) object;
        std::optional<GizmoHandleId> handleId;
    };

    explicit TransformGuideAdaptor(const Handle(AIS_InteractiveContext) &context);

    void ShowTransformGuide(const std::shared_ptr<TransformGuideState> &transformGuideState);

    void ClearTransformGuide();

private:
    struct TopologyKey {
        TransformElementSpace::TransformMode mode{TransformElementSpace::TransformMode::Move};
        TransformElementSpace::TransformConstraint constraint{TransformElementSpace::TransformConstraint::Free};
        bool valid{false};

        bool Matches(const TransformGuideState &state) const;
    };

    void RebuildObjects(const TransformGuideState &state);

    void UpdateObjectTransforms(const TransformGuideState &state) const;

    void UpdateObjectStyles(const TransformGuideState &state) const;

    void ClearObjects(bool updateViewer);

    Handle(AIS_InteractiveContext) m_Context;
    std::vector<GuideObject> m_MainGuideObjects;
    std::vector<GuideObject> m_ConstraintGuideObjects;
    TopologyKey m_Topology;
};

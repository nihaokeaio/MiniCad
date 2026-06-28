//
// Created by ZQD on 2026/6/28.
//
#pragma once
#include "Controller/Interaction/TransformTypes.h"

#include <optional>
#include <gp_Pnt.hxx>

enum class GizmoMode {
    Move,
    Rotate,
    Scale
};

enum class GizmoHandleType {
    Axis,
    Plane,
    Ring,
    Center
};

struct GizmoHandleId {
    GizmoMode mode{GizmoMode::Move};
    TransformElementSpace::TransformConstraint constraint{TransformElementSpace::TransformConstraint::Free};
    GizmoHandleType type{GizmoHandleType::Center};

    bool operator==(const GizmoHandleId &rhs) const {
        return mode == rhs.mode && constraint == rhs.constraint && type == rhs.type;
    }
};

struct GizmoState {
    bool visible{false};
    gp_Pnt pivot;
    std::optional<gp_Pnt> constraintPivot;
    GizmoMode mode{GizmoMode::Move};
    TransformElementSpace::TransformConstraint constraint{TransformElementSpace::TransformConstraint::Free};

    std::optional<GizmoHandleId> hovered;
    std::optional<GizmoHandleId> active;
};

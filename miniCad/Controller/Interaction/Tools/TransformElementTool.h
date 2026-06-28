//
// Created by ZQD on 2026/6/17.
//

#pragma once
#include "Controller/Interaction/InteractionHandler.h"
#include "Controller/Command/TransformElementsCommand.h"
#include "Utils/Types.h"

#include <gp_Pln.hxx>
#include <vector>

namespace TransformElementSpace {
    enum class TransformState {
        Idle,
        Dragging
    };

    enum class TransformMode {
        Move,
        Rotate,
        Scale
    };

    enum class TransformConstraint {
        XAxis,
        YAxis,
        ZAxis,
        Free
    };
}

class TransformElementTool : public InteractionHandler {
public:

    struct TransformSession {
        TransformElementSpace::TransformMode mode{TransformElementSpace::TransformMode::Move}; // Move / Rotate / Scale
        TransformElementSpace::TransformConstraint constraint{TransformElementSpace::TransformConstraint::Free};
        // Free / X / Y / Z / XY / YZ / XZ
        TransformElementSpace::TransformState state{TransformElementSpace::TransformState::Idle}; // Idle / Dragging

        gp_Pnt pivot;
        gp_Pln dragPlane;
        gp_Lin constraintAxis;

        gp_Pnt startPoint;
        gp_Pnt currentPoint;

        std::vector<ElementTransformChange> changes;
        bool guideVisible{false};
    };


    explicit TransformElementTool(InteractionContext *context);

    ~TransformElementTool() override;

    bool MousePress(QMouseEvent *event) override;

    bool MouseRelease(QMouseEvent *event) override;

    bool MouseMove(QMouseEvent *event) override;

    bool KeyPress(const QKeyEvent *event) override;

    InteractionPostAction OnMousePressAfter(QMouseEvent *event, bool handled) override;

private:
    [[nodiscard]] bool BeginDrag(QMouseEvent *event);

    [[nodiscard]] bool UpdateDrag(QMouseEvent *event);

    void ApplyDelta(const gp_Vec &delta);

    [[nodiscard]] GeometryTypes::RTransform BuildTransform(const gp_Vec &delta) const;

    [[nodiscard]] GeometryTypes::RTransform BuildMoveTransform(const gp_Vec &delta) const;

    [[nodiscard]] GeometryTypes::RTransform BuildRotateTransform(const gp_Vec &delta) const;

    [[nodiscard]] GeometryTypes::RTransform BuildScaleTransform(const gp_Vec &delta) const;

    gp_Vec ResolveConstrainedDelta(const QMouseEvent *event) const;

    gp_Vec ResolveAxisDelta(const QMouseEvent *event) const;

    gp_Vec ResolvePlaneDelta(const QMouseEvent *event) const;

    void UpdateIntersectionPlane();

    void RestoreOriginalTransforms();

    void ClearDragState();

    void CancelDragState();

    bool RefreshGuideFromSelection();

    void HideTransformGuide();

    void UpdateTransformGuideState() const;

    TransformSession m_Session;
};

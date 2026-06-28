//
// Created by ZQD on 2026/6/28.
//

#pragma once

#include "GizmoTypes.h"
#include "Controller/Interaction/Picking/PickTypes.h"

#include <optional>

struct InteractionContext;

class GizmoManager {
public:
    explicit GizmoManager(InteractionContext *context);

    void Show(const gp_Pnt &pivot,
              GizmoMode mode,
              TransformElementSpace::TransformConstraint constraint =
                  TransformElementSpace::TransformConstraint::Free,
              std::optional<gp_Pnt> constraintPivot = std::nullopt);

    void Hide();

    void SetMode(GizmoMode mode);

    void SetConstraint(TransformElementSpace::TransformConstraint constraint);

    void UpdatePose(const gp_Pnt &pivot, std::optional<gp_Pnt> constraintPivot = std::nullopt);

    [[nodiscard]] std::optional<GizmoPickTarget> Pick(int mouseX, int mouseY) const;

    bool UpdateHover(int mouseX, int mouseY);

    void SetHovered(std::optional<GizmoHandleId> handle);

    void SetActive(std::optional<GizmoHandleId> handle);

    [[nodiscard]] const GizmoState &State() const;

private:
    void SyncView() const;

    void SyncSceneWidgets() const;

    void SyncSceneWidgetTransforms() const;

    void ClearSceneWidgets() const;

    InteractionContext *m_Context = nullptr;
    GizmoState m_State;
};

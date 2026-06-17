//
// Created by ZQD on 2026/6/17.
//

#pragma once
#include "Controller/Interaction/InteractionHandler.h"
#include "Controller/Command/TransformElementsCommand.h"
#include "Utils/Types.h"

#include <gp_Pln.hxx>
#include <vector>

class TransformElementTool : public InteractionHandler {
public:
    enum class TransformState {
        Idle,
        Dragging
    };

    explicit TransformElementTool(InteractionContext *context);

    ~TransformElementTool() override;

    bool MousePress(QMouseEvent *event) override;

    bool MouseRelease(QMouseEvent *event) override;

    bool MouseMove(QMouseEvent *event) override;

    InteractionPostAction OnMousePressAfter(QMouseEvent *event, bool handled) override;

private:
    [[nodiscard]] bool BeginDrag(QMouseEvent *event);

    [[nodiscard]] bool UpdateDrag(QMouseEvent *event);

    void ApplyDelta(const Types::Point3f &currentPoint);

    void RestoreOriginalTransforms();

    void ClearDragState();

    void CancelDragState();

    Types::Point3f m_StartPoint;
    Types::Point3f m_CurrentPoint;
    gp_Pln m_DragPlane;
    std::vector<ElementTransformChange> m_TransformChanges;
    TransformState m_TransformState{TransformState::Idle};
};



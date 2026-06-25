//
// Created by ZQD on 2026/6/25.
//

#pragma once

#include <memory>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <gp_Pnt.hxx>
#include <vector>

#include "Controller/Interaction/Tools/TransformElementTool.h"


struct TransformGuideState {
    bool visible = false;
    gp_Pnt pivot;
    TransformElementSpace::TransformMode mode;
    TransformElementSpace::TransformConstraint constraint;
};


class TransformGuideAdaptor {
public:
    explicit TransformGuideAdaptor(const Handle(AIS_InteractiveContext) &context);

    void ShowTransformGuide(const std::shared_ptr<TransformGuideState> &transformGuideState);

    void ClearTransformGuide();

private:
    void ClearObjects(bool updateViewer);

    Handle(AIS_InteractiveContext) m_Context;
    std::vector<Handle(AIS_InteractiveObject)> m_GuideObjects;
};

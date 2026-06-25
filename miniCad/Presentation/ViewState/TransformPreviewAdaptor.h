//
// Created by ZQD on 2026/6/17.
//

#pragma once

#include <Standard_Handle.hxx>
#include <gp_Trsf.hxx>
#include <vector>

#include "Element/ElementId.h"

class AIS_InteractiveContext;
class ViewObjectRegistry;

struct ElementViewTransform {
    ElementId elementId{ElementId::InvalidId};
    gp_Trsf transform;
};

class TransformPreviewAdaptor {
public:
    TransformPreviewAdaptor(const Handle(AIS_InteractiveContext) &context, ViewObjectRegistry *registry);

    void Apply(const std::vector<ElementViewTransform> &transforms) const;

private:
    Handle(AIS_InteractiveContext) m_Context;
    ViewObjectRegistry *m_Registry = nullptr;
};

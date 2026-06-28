//
// Created by ZQD on 2026/6/10.
//

#pragma once

#include <Bnd_Box.hxx>

#include "../Data/Element/ElementId.h"
#include "../Data/ElementMesh/ElementMesh.h"
#include "../Data/Geometry/GeometryTypes.h"
#include "Controller/Interaction/Picking/PickTypes.h"
#include "Controller/Interaction/Picking/BVH/PrimitiveBvh.h"

struct SceneObject {
    virtual ~SceneObject() = default;

    GeometryTypes::RTransform localTransform;
    GeometryTypes::RTransform worldTransform;
    ElementMesh pickGeometry;
    std::vector<PickPrimitive> pickPrimitives;
    PrimitiveBvh primitiveBvh;
};

struct SceneElement : public SceneObject {
    ElementId elementId{ElementId::InvalidId};
    Bnd_Box boundingBox;
};

struct SceneWidget : public SceneObject {
    GizmoHandleId handle;
};

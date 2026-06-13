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
    ElementId elementId{ElementId::InvalidId};
    GeometryTypes::RTransform localTransform;
    GeometryTypes::RTransform worldTransform;
    Bnd_Box boundingBox;
    ElementMesh pickGeometry;
    std::vector<PickPrimitive> pickPrimitives;
    PrimitiveBvh primitiveBvh;
};

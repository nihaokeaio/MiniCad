//
// Created by ZQD on 2026/6/10.
//

#pragma once

#include <Bnd_Box.hxx>

#include "../Data/Element/ElementId.h"
#include "../Data/Geometry/GeometryTypes.h"

struct SceneObject {
    ElementId elementId{ElementId::InvalidId};
    GeometryTypes::RTransform localTransform;
    GeometryTypes::RTransform worldTransform;
    Bnd_Box boundingBox;
};

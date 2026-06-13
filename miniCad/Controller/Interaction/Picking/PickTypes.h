//
// Created by ZQD on 2026/6/13.
//

#pragma once

#include <Bnd_Box.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>

#include <cstdint>
#include <limits>
#include <optional>

#include "Element/ElementId.h"
#include "ElementMesh/ElementMesh.h"

struct PickSettings {
    double pointTolerance{2.0};
    double segmentTolerance{2.0};
    double broadPhaseTolerance{2.0};
    bool usePrimitiveBvh{true};
};

struct PickQuery {
    gp_Lin ray;
    PickSettings settings;
};

struct PickPrimitive {
    MeshPrimitiveType primitiveType{MeshPrimitiveType::None};
    uint32_t primitiveIndex{InvalidPrimitiveIndex};
    Bnd_Box bounds;
};

struct PickObject {
    ElementId elementId{ElementId::InvalidId};
    Bnd_Box bounds;
};

struct PickResult {
    ElementId elementId{ElementId::InvalidId};
    MeshPrimitiveType primitiveType{MeshPrimitiveType::None};
    uint32_t primitiveIndex{InvalidPrimitiveIndex};
    double distance{std::numeric_limits<double>::max()};
    gp_Pnt hitPoint;
};

namespace Picking {
    [[nodiscard]] int PrimitivePriority(MeshPrimitiveType type);

    [[nodiscard]] bool IsBetterPick(const PickResult &candidate, const PickResult &current);

    void UpdateBestPick(std::optional<PickResult> &best, const PickResult &candidate);
}

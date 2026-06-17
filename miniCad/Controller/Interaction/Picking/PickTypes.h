//
// Created by ZQD on 2026/6/13.
//

#pragma once

#include <Bnd_Box.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>

#include <cstdint>
#include <functional>
#include <limits>
#include <optional>

#include "Element/ElementId.h"
#include "ElementMesh/ElementMesh.h"

enum class PickMask : uint8_t {
    None = 0,
    Object = 1 << 0,
    Triangle = 1 << 1,
    Segment = 1 << 2,
    Point = 1 << 3,
    Any = Object | Triangle | Segment | Point
};

constexpr PickMask operator|(PickMask lhs, PickMask rhs) {
    return static_cast<PickMask>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

constexpr PickMask operator&(PickMask lhs, PickMask rhs) {
    return static_cast<PickMask>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

struct PickSettings {
    double pointTolerance{2.0};
    double segmentTolerance{2.0};
    double broadPhaseTolerance{2.0};
    PickMask pickMask{PickMask::Any};
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

struct PickTarget {
    ElementId elementId{ElementId::InvalidId};
    MeshPrimitiveType primitiveType{MeshPrimitiveType::None};
    uint32_t primitiveIndex{InvalidPrimitiveIndex};

    bool operator==(const PickTarget &rhs) const {
        return elementId == rhs.elementId &&
               primitiveType == rhs.primitiveType &&
               primitiveIndex == rhs.primitiveIndex;
    }
};

struct PickResult {
    PickTarget pickTarget;
    double distance{std::numeric_limits<double>::max()};
    gp_Pnt hitPoint;
};

namespace Picking {
    [[nodiscard]] PickMask MaskForPrimitiveType(MeshPrimitiveType type);

    [[nodiscard]] bool Allows(PickMask mask, MeshPrimitiveType type);

    [[nodiscard]] bool AllowsPrimitive(PickMask mask);

    [[nodiscard]] int PrimitivePriority(MeshPrimitiveType type);

    [[nodiscard]] bool IsBetterPick(const PickResult &candidate, const PickResult &current);

    void UpdateBestPick(std::optional<PickResult> &best, const PickResult &candidate);
}

template<>
struct std::hash<PickTarget> {
    size_t operator()(const PickTarget &target) const noexcept {
        const auto elementHash = std::hash<ElementId>{}(target.elementId);
        const auto typeHash = std::hash<uint8_t>{}(static_cast<uint8_t>(target.primitiveType));
        const auto indexHash = std::hash<uint32_t>{}(target.primitiveIndex);
        return elementHash ^ (typeHash << 1) ^ (indexHash << 2);
    }
};

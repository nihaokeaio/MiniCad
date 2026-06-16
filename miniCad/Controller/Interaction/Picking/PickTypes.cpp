//
// Created by ZQD on 2026/6/13.
//

#include "PickTypes.h"

#include <optional>

PickMask Picking::MaskForPrimitiveType(MeshPrimitiveType type) {
    switch (type) {
        case MeshPrimitiveType::Object:
            return PickMask::Object;
        case MeshPrimitiveType::Triangle:
            return PickMask::Triangle;
        case MeshPrimitiveType::Segment:
            return PickMask::Segment;
        case MeshPrimitiveType::Point:
            return PickMask::Point;
        case MeshPrimitiveType::None:
        default:
            return PickMask::None;
    }
}

bool Picking::Allows(PickMask mask, MeshPrimitiveType type) {
    return static_cast<uint8_t>(mask & MaskForPrimitiveType(type)) != 0;
}

bool Picking::AllowsPrimitive(PickMask mask) {
    return Allows(mask, MeshPrimitiveType::Point) ||
           Allows(mask, MeshPrimitiveType::Segment) ||
           Allows(mask, MeshPrimitiveType::Triangle);
}

int Picking::PrimitivePriority(MeshPrimitiveType type) {
    switch (type) {
        case MeshPrimitiveType::Point:
            return 3;
        case MeshPrimitiveType::Segment:
            return 2;
        case MeshPrimitiveType::Triangle:
            return 1;
        case MeshPrimitiveType::Object:
            return 0;
        case MeshPrimitiveType::None:
        default:
            return -1;
    }
}

bool Picking::IsBetterPick(const PickResult &candidate, const PickResult &current) {
    const int candidatePriority = PrimitivePriority(candidate.pickTarget.primitiveType);
    const int currentPriority = PrimitivePriority(current.pickTarget.primitiveType);
    if (candidatePriority != currentPriority) {
        return candidatePriority > currentPriority;
    }
    return candidate.distance < current.distance;
}

void Picking::UpdateBestPick(std::optional<PickResult> &best, const PickResult &candidate) {
    if (!best.has_value() || IsBetterPick(candidate, *best)) {
        best = candidate;
    }
}

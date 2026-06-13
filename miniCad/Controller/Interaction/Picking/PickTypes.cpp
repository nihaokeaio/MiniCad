//
// Created by ZQD on 2026/6/13.
//

#include "PickTypes.h"

#include <optional>

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
    const int candidatePriority = PrimitivePriority(candidate.primitiveType);
    const int currentPriority = PrimitivePriority(current.primitiveType);
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

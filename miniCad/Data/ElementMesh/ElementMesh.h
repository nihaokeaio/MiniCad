//
// Created by ZQD on 2026/6/10.
//

#pragma once

#include <cstdint>
#include <limits>
#include <vector>

#include "Geometry/GeometryTypes.h"

class TopoDS_Shape;

inline constexpr uint32_t InvalidPrimitiveIndex = std::numeric_limits<uint32_t>::max();

enum class MeshPrimitiveType : uint8_t {
    None,
    Object,
    Triangle,
    Segment,
    Point
};

struct MeshVertex {
    GeometryTypes::Point3D position;
    GeometryTypes::Vector3D normal;
};

struct TriangleIndex {
    uint32_t v0;
    uint32_t v1;
    uint32_t v2;
};

struct SegmentIndex {
    uint32_t v0;
    uint32_t v1;
};

struct PointIndex {
    uint32_t v0;
};

struct ElementMesh {
    std::vector<MeshVertex> vertices;
    std::vector<TriangleIndex> triangles;
    std::vector<SegmentIndex> segments;
    std::vector<PointIndex> points;

    [[nodiscard]] bool Empty() const {
        return vertices.empty() && triangles.empty() && segments.empty() && points.empty();
    }

    void Clear() {
        vertices.clear();
        triangles.clear();
        segments.clear();
        points.clear();
    }
};

struct ElementMeshBuildOptions {
    double deflection{0.5};
    double angleDeflection{0.5};
    bool relative{false};
    bool parallel{true};
    bool cleanBeforeBuild{true};
};

namespace ElementMeshBuilder {
    [[nodiscard]] ElementMesh BuildFromShape(const TopoDS_Shape &shape,
                                             const ElementMeshBuildOptions &options = {});
}

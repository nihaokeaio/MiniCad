//
// Created by ZQD on 2026/6/10.
//

#pragma once
#include "Geometry/GeometryTypes.h"


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

struct ElementMesh {
    std::vector<MeshVertex> vertices;
    std::vector<TriangleIndex> triangles;
    std::vector<SegmentIndex> segments;
};



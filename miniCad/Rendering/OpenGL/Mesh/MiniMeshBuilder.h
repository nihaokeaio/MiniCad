//
// Created by ZQD on 2026/7/19.
//

#pragma once
#include <memory>

#include "MiniMesh.h"

struct ElementMesh;
class MiniMesh;

namespace MiniMeshBuilder {
    MeshInfo BuildRenderMeshData(const ElementMesh *mesh);

    std::unique_ptr<MiniMesh> CreateMeshFromMeshInfo(const MeshInfo &meshInfo);
}

//
// Created by ZQD on 2026/7/19.
//
#include "MiniMeshBuilder.h"
#include "MiniMesh.h"
#include "Common/Types.h"
#include "ElementMesh/ElementMesh.h"

std::unique_ptr<MiniMesh> MiniMeshBuilder::CreateMeshFromMeshInfo(const MeshInfo &meshInfo) {
    return std::make_unique<MiniMesh>(meshInfo.vertices, meshInfo.indices);
}

MeshInfo MiniMeshBuilder::BuildRenderMeshData(const ElementMesh *mesh) {
    MeshInfo meshInfo;
    auto &vertices = meshInfo.vertices;
    auto &indices = meshInfo.indices;
    vertices.resize(mesh->vertices.size());
    indices.resize(3 * mesh->triangles.size());
    for (unsigned int i = 0; i < mesh->vertices.size(); i++) {
        vertices[i].position = Types::ConvertFromOcct3D(mesh->vertices[i].position);
        vertices[i].normal = Types::ConvertFromOcct3D(mesh->vertices[i].normal);
    }
    for (unsigned int i = 0; i < mesh->triangles.size(); i += 1) {
        indices[3 * i] = mesh->triangles[i].v0;
        indices[3 * i + 1] = mesh->triangles[i].v1;
        indices[3 * i + 2] = mesh->triangles[i].v2;
    }
    return meshInfo;
}

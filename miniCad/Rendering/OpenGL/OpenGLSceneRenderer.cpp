//
// Created by ZQD on 2026/7/22.
//

#include "OpenGL/OpenGLSceneRenderer.h"

#include <ranges>

#include "Common/Types.h"
#include "Mesh/MiniMeshBuilder.h"
#include "Renderer/MiniRenderer.h"
#include "RenderShader/MiniRenderShader.h"
#include "Scene/Scene.h"

void OpenGLSceneRenderer::Initialize() {
    m_Shader = std::make_unique<MiniRenderShader>(
        R""(D:/program/MyTinyCad/miniCad/Shader/Base.vert)"",
        R""(D:/program/MyTinyCad/miniCad/Shader/Base.frag)"");
    m_Renderer = std::make_unique<MiniRenderer>();
}

void OpenGLSceneRenderer::Sync(const Scene *scene) {
    for (const auto &[id,element]: scene->GetAllObjects()) {
        if (!m_RenderObjects.contains(id)) {
            RenderObject object;
            object.modelMatrix = Types::ConvertFromOcctMat4x4(element->worldTransform);
            const auto &meshInfo = MiniMeshBuilder::BuildRenderMeshData(&element->pickGeometry);
            object.miniMesh = std::move(MiniMeshBuilder::CreateMeshFromMeshInfo(meshInfo));
            m_RenderObjects.insert({id, std::move(object)});
            continue;
        }
        m_RenderObjects[id].modelMatrix = Types::ConvertFromOcctMat4x4(element->worldTransform);
    }
}

void OpenGLSceneRenderer::Render(const MiniCamera *camera) {
    std::vector<RenderItem> items;
    for (const auto &[miniMesh, modelMatrix]: m_RenderObjects | std::views::values) {
        items.emplace_back(miniMesh.get(), m_Shader.get(), modelMatrix);
    }
    m_Renderer->Render(*camera, items);
}

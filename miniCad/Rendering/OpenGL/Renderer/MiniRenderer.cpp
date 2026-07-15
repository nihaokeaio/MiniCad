//
// Created by ZQD on 2026/7/15.
//

#include "MiniRenderer.h"

#include <QOpenGLShaderProgram>

#include "OpenGL/Camera/MiniCamera.h"
#include "OpenGL/Mesh/MiniMesh.h"
#include "OpenGL/RenderShader/MiniRenderShader.h"

MiniRenderer::~MiniRenderer() {
    m_RenderItems.clear();
    m_Meshes.clear();
    m_Shaders.clear();
}

MiniRenderShader *MiniRenderer::AddShader(std::unique_ptr<MiniRenderShader> shader) {
    if (shader == nullptr) {
        return nullptr;
    }

    auto *rawShader = shader.get();
    m_Shaders.push_back(std::move(shader));
    return rawShader;
}

MiniMesh *MiniRenderer::AddMesh(std::unique_ptr<MiniMesh> mesh) {
    if (mesh == nullptr) {
        return nullptr;
    }

    auto *rawMesh = mesh.get();
    m_Meshes.push_back(std::move(mesh));
    return rawMesh;
}

void MiniRenderer::AddRenderItem(const RenderItem &item) {
    if (item.mesh == nullptr || item.shader == nullptr) {
        return;
    }

    m_RenderItems.push_back(item);
}

void MiniRenderer::ClearRenderItems() {
    m_RenderItems.clear();
}

void MiniRenderer::Render(const MiniCamera &camera) const {
    const QMatrix4x4 view = camera.ViewMatrix();
    const QMatrix4x4 projection = camera.ProjectionMatrix();

    MiniRenderShader *activeShader = nullptr;
    for (const auto &item: m_RenderItems) {
        if (item.mesh == nullptr || item.shader == nullptr) {
            continue;
        }

        if (activeShader != item.shader) {
            if (activeShader != nullptr) {
                activeShader->Release();
            }

            activeShader = item.shader;
            activeShader->Active();
            auto *shaderProgram = activeShader->GetShaderProgram();
            shaderProgram->setUniformValue("uView", view);
            shaderProgram->setUniformValue("uProjection", projection);
        }

        auto *shaderProgram = activeShader->GetShaderProgram();
        shaderProgram->setUniformValue("uModel", item.model);
        item.mesh->Draw();
    }

    if (activeShader != nullptr) {
        activeShader->Release();
    }
}

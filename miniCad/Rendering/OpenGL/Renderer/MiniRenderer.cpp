//
// Created by ZQD on 2026/7/15.
//

#include "MiniRenderer.h"

#include <QOpenGLShaderProgram>

#include "OpenGL/Camera/MiniCamera.h"
#include "OpenGL/Mesh/MiniMesh.h"
#include "OpenGL/RenderShader/MiniRenderShader.h"


void MiniRenderer::Render(const MiniCamera &camera, std::span<const RenderItem> items) {
    const QMatrix4x4 view = camera.ViewMatrix();
    const QMatrix4x4 projection = camera.ProjectionMatrix();

    const MiniRenderShader *activeShader = nullptr;
    for (const auto &[mesh, shader, model]: items) {
        if (mesh == nullptr || shader == nullptr) {
            continue;
        }

        if (activeShader != shader) {
            if (activeShader != nullptr) {
                activeShader->Release();
            }

            activeShader = shader;
            activeShader->Active();
            auto *shaderProgram = activeShader->GetShaderProgram();
            shaderProgram->setUniformValue("uView", view);
            shaderProgram->setUniformValue("uProjection", projection);
            shaderProgram->setUniformValue("uBaseColor", Material::baseColor);
        }

        auto *shaderProgram = activeShader->GetShaderProgram();
        shaderProgram->setUniformValue("uModel", model);
        mesh->Draw();
    }

    if (activeShader != nullptr) {
        activeShader->Release();
    }
}

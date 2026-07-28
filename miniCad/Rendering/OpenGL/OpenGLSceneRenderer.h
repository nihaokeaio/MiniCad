//
// Created by ZQD on 2026/7/22.
//

#pragma once
#include <memory>
#include <QMatrix4x4>

#include "Element/ElementId.h"
#include "OpenGL/Mesh/MiniMesh.h"

class MiniRenderer;
class MiniRenderShader;
class Scene;
class MiniCamera;

struct RenderObject {
    std::unique_ptr<MiniMesh> miniMesh;
    QMatrix4x4 modelMatrix;
};

class OpenGLSceneRenderer {
public:
    void Initialize();

    void Sync(const Scene *scene);

    void Render(const MiniCamera *camera);

private:
    std::unordered_map<ElementId, RenderObject> m_RenderObjects;
    std::unique_ptr<MiniRenderShader> m_Shader;
    std::unique_ptr<MiniRenderer> m_Renderer;
};




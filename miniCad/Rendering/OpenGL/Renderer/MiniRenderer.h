//
// Created by ZQD on 2026/7/15.
//

#pragma once

#include <memory>
#include <vector>

#include <QMatrix4x4>

class MiniCamera;
class MiniMesh;
class MiniRenderShader;

struct RenderItem {
    MiniMesh *mesh = nullptr;
    MiniRenderShader *shader = nullptr;
    QMatrix4x4 model;
};

class MiniRenderer {
public:
    MiniRenderer() = default;
    ~MiniRenderer();

    MiniRenderShader *AddShader(std::unique_ptr<MiniRenderShader> shader);

    MiniMesh *AddMesh(std::unique_ptr<MiniMesh> mesh);

    void AddRenderItem(const RenderItem &item);

    void ClearRenderItems();

    void Render(const MiniCamera &camera) const;

private:
    std::vector<std::unique_ptr<MiniRenderShader>> m_Shaders;
    std::vector<std::unique_ptr<MiniMesh>> m_Meshes;
    std::vector<RenderItem> m_RenderItems;
};

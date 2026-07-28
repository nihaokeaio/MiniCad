//
// Created by ZQD on 2026/7/15.
//

#pragma once

#include <memory>
#include <vector>

#include <QMatrix4x4>
#include <span>

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
    static void Render(const MiniCamera &camera, std::span<const RenderItem> items);
};

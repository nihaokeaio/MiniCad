//
// Created by ZQD on 2026/7/8.
//

#pragma once
#include <memory>
#include <string>

class QOpenGLShaderProgram;

class MiniRenderShader {
public:
    //构造数据
    MiniRenderShader(const char *vertexFilePath, const char *fragmentFilePath);

    //使用（active）着色器
    void Active() const;

    void Release() const;

    static void LoadTexture(const std::string &filename, unsigned int &texture);

    QOpenGLShaderProgram *GetShaderProgram() const;

private:
    std::unique_ptr<QOpenGLShaderProgram> m_ShaderProgram;
};

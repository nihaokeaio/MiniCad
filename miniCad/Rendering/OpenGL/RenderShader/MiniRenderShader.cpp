//
// Created by ZQD on 2026/7/8.
//

#include "MiniRenderShader.h"

#include <QOpenGLShaderProgram>

MiniRenderShader::MiniRenderShader(const char *vertexFilePath, const char *fragmentFilePath) {
    m_ShaderProgram = std::make_unique<QOpenGLShaderProgram>();
    if (!m_ShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexFilePath)) {
        qWarning() << "Vertex shader compile failed:" << m_ShaderProgram->log();
        return;
    }
    if (!m_ShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentFilePath)) {
        qWarning() << "Fragment shader compile failed:" << m_ShaderProgram->log();
        return;
    }
    if (!m_ShaderProgram->link()) {
        qWarning() << "Shader link failed:" << m_ShaderProgram->log();
        return;
    }
}

MiniRenderShader::~MiniRenderShader() = default;

void MiniRenderShader::Active() const {
    m_ShaderProgram->bind();
}

void MiniRenderShader::Release() const {
    m_ShaderProgram->release();
}

void MiniRenderShader::LoadTexture(const std::string &filename, unsigned int &texture) {
}

QOpenGLShaderProgram *MiniRenderShader::GetShaderProgram() const {
    return m_ShaderProgram.get();
}

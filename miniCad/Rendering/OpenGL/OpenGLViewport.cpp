//
// Created by ZQD on 2026/7/6.
//

#include "OpenGLViewport.h"

#include <QDebug>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QVector3D>

#include "MiniRenderShader/MiniRenderShader.h"

namespace {
    struct Vertex {
        float position[3];
        float color[3];
    };

    constexpr Vertex TriangleVertices[] = {
        {{-0.6f, -0.5f, 0.0f}, {1.0f, 0.1f, 0.1f}},
        {{0.6f, -0.5f, 0.0f}, {0.1f, 1.0f, 0.1f}},
        {{0.0f, 0.6f, 0.0f}, {0.1f, 0.35f, 1.0f}},
    };

    constexpr const char *VertexShaderSource = R"(
attribute vec3 aPosition;
attribute vec3 aColor;

varying vec3 vColor;

void main()
{
    gl_Position = vec4(aPosition, 1.0);
    vColor = aColor;
}
)";

    constexpr const char *FragmentShaderSource = R"(
varying vec3 vColor;

void main()
{
    gl_FragColor = vec4(vColor, 1.0);
}
)";
}

OpenGLViewport::OpenGLViewport(QWidget *parent) : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

OpenGLViewport::~OpenGLViewport() {
    makeCurrent();
    ReleaseTriangle();
    doneCurrent();
}

void OpenGLViewport::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    InitTriangle();
    qDebug() << "initializeGL done!";
    m_ElapsedTimer.start();
}

void OpenGLViewport::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void OpenGLViewport::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_MiniRenderShader) {
        return;
    }
    const qint64 elapsedMs = m_ElapsedTimer.elapsed();
    const float elapsedSeconds = static_cast<float>(elapsedMs) / 1000.0f;

    QMatrix4x4 model;
    model.setToIdentity();
    model.rotate(elapsedSeconds * 45.0f, 0.0f, 1.0f, 0.0f);

    QMatrix4x4 view;
    view.lookAt(
        QVector3D(0.0f, 0.0f, 3.0f),
        QVector3D(0.0f, 0.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 projection;
    const float aspectRatio = height() > 0
                                  ? static_cast<float>(width()) / static_cast<float>(height())
                                  : 1.0f;
    projection.perspective(45.0f, aspectRatio, 0.1f, 100.0f);

    m_MiniRenderShader->Active();
    m_VertexArray.bind();
    m_MiniRenderShader->GetShaderProgram()->setUniformValue("uModel", model);
    m_MiniRenderShader->GetShaderProgram()->setUniformValue("uView", view);
    m_MiniRenderShader->GetShaderProgram()->setUniformValue("uProjection", projection);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_VertexArray.release();
    m_MiniRenderShader->Release();
    update();
}

void OpenGLViewport::InitTriangle() {
    m_MiniRenderShader = std::make_unique<MiniRenderShader>(R""(D:/program/MyTinyCad/miniCad/Shader/Base.vert)"",
                                                            R""(D:/program/MyTinyCad/miniCad/Shader/Base.frag)"");

    m_VertexArray.create();
    m_VertexArray.bind();

    m_VertexBuffer.create();
    m_VertexBuffer.bind();
    m_VertexBuffer.allocate(TriangleVertices, sizeof(TriangleVertices));

    m_MiniRenderShader->Active();
    const auto shaderProgram = m_MiniRenderShader->GetShaderProgram();
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));
    shaderProgram->enableAttributeArray(0);

    shaderProgram->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, color), 3, sizeof(Vertex));
    shaderProgram->enableAttributeArray(1);

    m_VertexBuffer.release();
    m_VertexArray.release();
    m_MiniRenderShader->Release();
}

void OpenGLViewport::ReleaseTriangle() {
    m_VertexArray.destroy();
    m_VertexBuffer.destroy();
}

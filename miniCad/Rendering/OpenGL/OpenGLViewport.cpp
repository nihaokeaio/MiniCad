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

    // 立方体的8个顶点
    constexpr Vertex CubeVertices[] = {
        // 前面 (z = 0.5f)
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // 0: 红色
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, // 1: 绿色
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, // 2: 蓝色
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}}, // 3: 黄色

        // 后面 (z = -0.5f)
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}}, // 4: 品红
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}}, // 5: 青色
        {{0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}}, // 6: 灰色
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.5f, 0.0f}}, // 7: 橙色
    };

    // 索引数组：12个三角形，每个三角形3个索引
    constexpr unsigned int CubeIndices[] = {
        // 前面 (z = 0.5f)
        0, 1, 2,
        0, 2, 3,
        // 右面 (x = 0.5f)
        1, 5, 6,
        1, 6, 2,
        // 后面 (z = -0.5f)
        5, 4, 7,
        5, 7, 6,
        // 左面 (x = -0.5f)
        4, 0, 3,
        4, 3, 7,
        // 顶面 (y = 0.5f)
        3, 2, 6,
        3, 6, 7,
        // 底面 (y = -0.5f)
        4, 5, 1,
        4, 1, 0,
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
    ReleaseVertexData();
    doneCurrent();
}

void OpenGLViewport::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    InitVertexData();
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
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, std::size(CubeIndices), GL_UNSIGNED_INT, 0);
    m_VertexArray.release();
    m_MiniRenderShader->Release();
    update();
}

void OpenGLViewport::InitVertexData() {
    m_MiniRenderShader = std::make_unique<MiniRenderShader>(R""(D:/program/MyTinyCad/miniCad/Shader/Base.vert)"",
                                                            R""(D:/program/MyTinyCad/miniCad/Shader/Base.frag)"");

    m_VertexArray.create();
    m_VertexArray.bind();

    m_VertexBuffer.create();
    m_VertexBuffer.bind();
    m_VertexBuffer.allocate(CubeVertices, sizeof(CubeVertices));

    m_IndexBuffer.create();
    m_IndexBuffer.bind();
    m_IndexBuffer.allocate(CubeIndices, sizeof(CubeIndices));

    m_MiniRenderShader->Active();
    const auto shaderProgram = m_MiniRenderShader->GetShaderProgram();
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));
    shaderProgram->enableAttributeArray(0);

    shaderProgram->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, color), 3, sizeof(Vertex));
    shaderProgram->enableAttributeArray(1);

    m_VertexBuffer.release();
    m_VertexArray.release();
    m_IndexBuffer.release();
    m_MiniRenderShader->Release();
}

void OpenGLViewport::ReleaseVertexData() {
    m_VertexArray.destroy();
    m_VertexBuffer.destroy();
    m_IndexBuffer.destroy();
}

//
// Created by ZQD on 2026/7/6.
//

#include "OpenGLViewport.h"
#include <QDebug>
#include <QOpenGLShaderProgram>

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
}

void OpenGLViewport::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void OpenGLViewport::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_ShaderProgram == nullptr) {
        return;
    }

    m_ShaderProgram->bind();
    m_VertexArray.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_VertexArray.release();
    m_ShaderProgram->release();
}

void OpenGLViewport::InitTriangle() {
    m_ShaderProgram = new QOpenGLShaderProgram(this);
    if (!m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, VertexShaderSource)) {
        qWarning() << "Vertex shader compile failed:" << m_ShaderProgram->log();
        return;
    }
    if (!m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, FragmentShaderSource)) {
        qWarning() << "Fragment shader compile failed:" << m_ShaderProgram->log();
        return;
    }
    if (!m_ShaderProgram->link()) {
        qWarning() << "Shader link failed:" << m_ShaderProgram->log();
        return;
    }

    m_VertexArray.create();
    m_VertexArray.bind();

    m_VertexBuffer.create();
    m_VertexBuffer.bind();
    m_VertexBuffer.allocate(TriangleVertices, sizeof(TriangleVertices));

    m_ShaderProgram->bind();
    const int positionLocation = m_ShaderProgram->attributeLocation("aPosition");
    const int colorLocation = m_ShaderProgram->attributeLocation("aColor");

    m_ShaderProgram->enableAttributeArray(positionLocation);
    m_ShaderProgram->setAttributeBuffer(positionLocation, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));

    m_ShaderProgram->enableAttributeArray(colorLocation);
    m_ShaderProgram->setAttributeBuffer(colorLocation, GL_FLOAT, offsetof(Vertex, color), 3, sizeof(Vertex));

    m_VertexBuffer.release();
    m_VertexArray.release();
    m_ShaderProgram->release();
}

void OpenGLViewport::ReleaseTriangle() {
    m_VertexArray.destroy();
    m_VertexBuffer.destroy();
    delete m_ShaderProgram;
    m_ShaderProgram = nullptr;
}

//
// Created by ZQD on 2026/7/6.
//

#include "OpenGLViewport.h"

#include <QDebug>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

#include "Mesh/MiniMesh.h"
#include "Renderer/MiniRenderer.h"
#include "RenderShader/MiniRenderShader.h"

namespace {
    // 立方体的8个顶点
    std::vector<Vertex> CubeVertices = {
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
    std::vector<unsigned int> CubeIndices = {
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
}

OpenGLViewport::OpenGLViewport(QWidget *parent) : QOpenGLWidget(parent), m_Camera(width(), height()) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

OpenGLViewport::~OpenGLViewport() {
    makeCurrent();
    m_Renderer.reset();
    doneCurrent();
}

void OpenGLViewport::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    InitRenderer();
    qDebug() << "initializeGL done!";
    m_ElapsedTimer.start();
}

void OpenGLViewport::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    m_Camera.Resize(w, h);
}

void OpenGLViewport::paintGL() {
    UpdateFrameStats();

    PrepareSceneRenderState();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_Renderer) {
        DrawFrameStats();
        return;
    }

    m_Renderer->Render(m_Camera);
    DrawFrameStats();
    update();
}

void OpenGLViewport::mousePressEvent(QMouseEvent *event) {
    m_LastMousePosition = event->pos();
    QOpenGLWidget::mousePressEvent(event);
}

void OpenGLViewport::mouseMoveEvent(QMouseEvent *event) {
    const QPoint delta = event->pos() - m_LastMousePosition;
    m_LastMousePosition = event->pos();

    if (event->buttons() & Qt::MiddleButton) {
        if (event->modifiers() & Qt::ShiftModifier) {
            m_Camera.Pan(static_cast<float>(delta.x()), static_cast<float>(delta.y()));
        } else {
            m_Camera.Orbit(static_cast<float>(delta.x()), static_cast<float>(delta.y()));
        }
        update();
        return;
    }

    QOpenGLWidget::mouseMoveEvent(event);
}

void OpenGLViewport::wheelEvent(QWheelEvent *event) {
    const float wheelSteps = static_cast<float>(event->angleDelta().y()) / 120.0f;
    if (wheelSteps != 0.0f) {
        m_Camera.Zoom(wheelSteps);
        update();
        return;
    }

    QOpenGLWidget::wheelEvent(event);
}

void OpenGLViewport::InitRenderer() {
    m_Renderer = std::make_unique<MiniRenderer>();

    auto *shader = m_Renderer->AddShader(std::make_unique<MiniRenderShader>(
        R""(D:/program/MyTinyCad/miniCad/Shader/Base.vert)"",
        R""(D:/program/MyTinyCad/miniCad/Shader/Base.frag)""));
    auto *mesh = m_Renderer->AddMesh(std::make_unique<MiniMesh>(CubeVertices, CubeIndices));

    QMatrix4x4 model;
    model.setToIdentity();
    m_Renderer->AddRenderItem(RenderItem{mesh, shader, model});
}

void OpenGLViewport::PrepareSceneRenderState() {
    glViewport(0, 0, width(), height());
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glClearDepthf(1.0f);
}

void OpenGLViewport::UpdateFrameStats() {
    ++m_FrameCount;

    const qint64 nowMs = m_ElapsedTimer.elapsed();
    if (m_LastFpsSampleMs == 0) {
        m_LastFpsSampleMs = nowMs;
        return;
    }

    const qint64 elapsedMs = nowMs - m_LastFpsSampleMs;
    if (elapsedMs < 500) {
        return;
    }

    m_Fps = static_cast<float>(m_FrameCount) * 1000.0f / static_cast<float>(elapsedMs);
    m_FrameTimeMs = static_cast<float>(elapsedMs) / static_cast<float>(m_FrameCount);
    m_FrameCount = 0;
    m_LastFpsSampleMs = nowMs;
}

void OpenGLViewport::DrawFrameStats() {
    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setPen(QColor(230, 230, 230));
    painter.drawText(
        12,
        24,
        QString("FPS: %1  Frame: %2 ms")
        .arg(m_Fps, 0, 'f', 1)
        .arg(m_FrameTimeMs, 0, 'f', 2));
}

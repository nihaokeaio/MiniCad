//
// Created by ZQD on 2026/7/6.
//

#pragma once
#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QPoint>

#include "Camera/MiniCamera.h"

class MiniRenderShader;
class QOpenGLShaderProgram;

class OpenGLViewport : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    explicit OpenGLViewport(QWidget *parent = nullptr);

    ~OpenGLViewport() override;

protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

private:
    void InitVertexData();

    void ReleaseVertexData();

    void PrepareSceneRenderState();

    void UpdateFrameStats();

    void DrawFrameStats();

private:
    std::unique_ptr<MiniRenderShader> m_MiniRenderShader;
    QOpenGLBuffer m_VertexBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_IndexBuffer{QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject m_VertexArray;
    MiniCamera m_Camera;
    QPoint m_LastMousePosition;
    QElapsedTimer m_ElapsedTimer;
    qint64 m_LastFpsSampleMs = 0;
    int m_FrameCount = 0;
    float m_Fps = 0.0f;
    float m_FrameTimeMs = 0.0f;
};

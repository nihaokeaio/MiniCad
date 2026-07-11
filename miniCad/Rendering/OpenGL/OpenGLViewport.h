//
// Created by ZQD on 2026/7/6.
//

#pragma once
#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

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

private:
    void InitTriangle();
    void ReleaseTriangle();

private:
    std::unique_ptr<MiniRenderShader> m_MiniRenderShader;
    QOpenGLBuffer m_VertexBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_VertexArray;
    QElapsedTimer m_ElapsedTimer;
    float m_TimeStamp = 0.0f;
};



//
// Created by ZQD on 2026/7/6.
//

#pragma once
#include <QOpenGLBuffer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

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
    QOpenGLShaderProgram *m_ShaderProgram = nullptr;
    QOpenGLBuffer m_VertexBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_VertexArray;
};



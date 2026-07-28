//
// Created by ZQD on 2026/7/11.
//

#pragma once
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <vector>


struct Vertex {
    QVector3D position;
    QVector3D normal;
};

struct Material {
    inline static auto baseColor = QVector3D(1.0, 0.0, 1.0);
};


struct MeshInfo {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

class MiniMesh : public QOpenGLFunctions {
public:
    MiniMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);


    ~MiniMesh();

    void Draw();

private:
    QOpenGLBuffer m_VBO{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_EBO{QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject m_VAO;
    unsigned int m_indexCount;
};


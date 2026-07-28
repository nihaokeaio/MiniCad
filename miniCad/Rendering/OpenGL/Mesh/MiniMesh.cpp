//
// Created by ZQD on 2026/7/11.
//

#include "MiniMesh.h"

MiniMesh::MiniMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) {
    initializeOpenGLFunctions();
    m_VAO.create();
    m_VAO.bind();

    m_VBO.create();
    m_VBO.bind();
    m_VBO.allocate(vertices.data(), sizeof(Vertex) * vertices.size());

    m_EBO.create();
    m_EBO.bind();
    m_EBO.allocate(indices.data(), sizeof(unsigned int) * indices.size());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);


    m_VBO.release();
    m_VAO.release();
    m_EBO.release();
    m_indexCount = indices.size();
}

MiniMesh::~MiniMesh() {
    m_VAO.destroy();
    m_VBO.destroy();
    m_EBO.destroy();
}

void MiniMesh::Draw() {
    m_VAO.bind();
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    m_VAO.release();
}

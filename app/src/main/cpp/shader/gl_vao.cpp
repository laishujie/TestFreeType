//
// Created by Lai on 2020/12/3.
//

#include "comm_gl.hpp"
#include <GLES3/gl3.h>
#include <logUtil.h>

GLuint GLVAO::AddVertex3D(float *data, int vertexCount, GLuint layout) {
    glBindVertexArray(vao);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), data, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid *) 0);

    glEnableVertexAttribArray(layout);

    vboList.push_back(vbo);

    glBindVertexArray(0);
    return vbo;
}

GLuint GLVAO::AddVertex2D(float *data, int vertexCount, GLuint layout) {
    glBindVertexArray(vao);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);


    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), data, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(layout, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid *) 0);

    glEnableVertexAttribArray(layout);

    vboList.push_back(vbo);

    glBindVertexArray(0);
    return vbo;
}


void GLVAO::updateVertex2D(GLuint updateFbo, float *data, int vertexCount, GLuint layout) {
    glBindBuffer(GL_ARRAY_BUFFER, updateFbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), data, GL_DYNAMIC_DRAW);
    // LOGE("11111", "data %f vertexCount %d updateFbo %d", *data, vertexCount, updateFbo);
}

GLuint GLVAO::setVertex2D(GLuint &fbo, float *data, int vertexCount, GLuint layout) {
    //LOGE("11111","FBO %d",fbo)
    if (fbo == 0) {
        fbo = AddVertex2D(data, vertexCount, layout);
    } else {
        updateVertex2D(fbo, data, vertexCount, layout);
    }
    return fbo;
}


GLVAO::GLVAO() {
    glGenVertexArrays(1, &vao);
}

//绑定ebo
GLuint GLVAO::setIndex(unsigned int *indexData, int indexCount) {
    glBindVertexArray(vao);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData,
                 GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
    return ebo;
}

GLuint GLVAO::updateIndex(unsigned int *indexData, int indexCount) const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData,
                 GL_DYNAMIC_DRAW);
    return 0;
}

int GLVAO::BindVAO() const {
    glBindVertexArray(vao);
    return 0;
}

GLVAO::~GLVAO() {
    for (unsigned int vbo : vboList) {
        glDeleteBuffers(1, &vbo);
    }
    vboList.clear();

    if (ebo != 0) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}

void GLVAO::subDataVertex2D(GLuint updateFbo, float *data, int vertexCount) {
    glBindBuffer(GL_ARRAY_BUFFER, updateFbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * 2 * sizeof(float), data);

    //glBufferSubData(GL_ARRAY_BUFFER, 0,vertexCount * 2 * sizeof(float), data);
}

void GLVAO::subDataIndex2D(unsigned int *indexData, int indexCount) {
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,indexCount * sizeof(unsigned int), indexData);

    glBindVertexArray(0);
}


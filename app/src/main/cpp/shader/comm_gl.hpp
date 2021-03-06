//
// Created by Lai on 2020/12/3.
//

#ifndef OPENGLDEMO_COMMGL_HPP
#define OPENGLDEMO_COMMGL_HPP

#include <GLES3/gl3.h>
#include <vector>

enum GLShaderType {
    SHADER_VERTEX = 1,
    SHADER_FRAGMENT = 2
};


class GLShader {
public:
    GLShader(char *shaderStr, GLShaderType type);

    ~GLShader();

    GLuint shader = 0;
};


class GLProgram {
public:
    GLProgram(char *vertexShaderStr, char *fragmentShaderSrc);

    ~GLProgram();

    int useProgram();

    void OnSurfaceChanged(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

public:
    GLuint program = 0;
};

class GLVAO {
public:
    GLVAO();

    ~GLVAO();

    GLuint AddVertex3D(float *data, int vertexCount, GLuint layout);

    GLuint AddVertex2D(float *data, int vertexCount, GLuint layout);

    GLuint setIndex(unsigned int *indexData, int indexCount);

    GLuint updateIndex(unsigned int *indexData, int indexCount) const;

    int BindVAO() const;

    void updateVertex2D(GLuint updateFbo, float *data, int vertexCount, GLuint layout);

    void subDataVertex2D(GLuint updateFbo, float *data, int vertexCount);

    void subDataIndex2D(unsigned int *indexData, int indexCount);

    GLuint setVertex2D(GLuint &fbo, float *data, int vertexCount, GLuint layout);

private:
    GLuint vao = 0;
    GLuint ebo = 0;
    std::vector<GLuint> vboList;
};

#endif //OPENGLDEMO_COMMGL_HPP

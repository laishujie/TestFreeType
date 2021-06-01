//
// Created by admin on 2021/5/25.
//

#include <logUtil.h>
#include "matrix_shader.h"

void MatrixShader::onDestroy() {

}

void MatrixShader::Init() {
    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "layout(location = 1) in vec3 uvPos;\n"


            "uniform mat4 uMatrix;\n"
            "uniform float width;"
            "uniform float height;"
            "uniform float sc;"

            "out vec3 outUvPos;\n"
            "uniform float r;"

            "mat2 rotate2d(float _angle){\n"
            "    return mat2(cos(_angle),-sin(_angle),\n"
            "                sin(_angle),cos(_angle));\n"
            "}"

            "mat2 scale(vec2 _scale){\n"
            "    return mat2(_scale.x,0.0,\n"
            "                0.0,_scale.y);\n"
            "}"
            "uniform float tx;"
            "uniform float ty;"
            "uniform float cx;"
            "uniform float cy;"
            "void main()                              \n"
            "{                                        \n"

            "vec2 position = vec2(vPosition.x,vPosition.y);"

            //transform
            "position+=vec2(tx,ty);"

            //平移到原点
            "position-=vec2(cx,cy);"

            "float sw = float(width);"
            "float sh = float(height);"
            "position*=sc;"

            "float ratio = float(width)/float(height);"
            "position.x = position.x * ratio;"
            "position = rotate2d(radians(float(-r))) * position;"
            "position.x = position.x / ratio;"
            "position+=vec2(cx,cy);"


            " gl_Position =  vec4(position,0.,1.);              \n"


            " outUvPos = uvPos;              \n"
            "}                                        \n";

    char fboShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "uniform sampler2D textureMap;\n"
            "in vec3 outPos;\n"
            "in vec3 outUvPos;\n"

            "mat2 rotate2d(float _angle){\n"
            "    return mat2(cos(_angle),-sin(_angle),\n"
            "                sin(_angle),cos(_angle));\n"
            "}"

            "mat3 myMat3 = mat3(1.0, 0.0, 0.0, \n"
            "                   0.0, 1.0, 0.0,  \n"
            "                   0.0, 1.0, 1.0);"

            "mat2 scale(vec2 _scale){\n"
            "    return mat2(_scale.x,0.0,\n"
            "                0.0,_scale.y);\n"
            "}"



            "void main()                                  \n"
            "{                                            \n"
            "vec2 uv = vec2(outUvPos.x,1.-outUvPos.y); \n"
            "ivec2 size = textureSize(textureMap, 0);"
            "vec4 textureMap = texture(textureMap,uv); \n"
            "fragColor = textureMap;\n"
            "}                                            \n";


    glProgram = new GLProgram(vShaderStr, fboShaderStr);
    glvao = new GLVAO();

    GLfloat rectangleVertices[] = {
            -1.0f, 1.0f, 0.0f,// 左上角
            1.0f, 1.0f, 0.0f,//右上角
            1.0f, -1.0f, 0.0f,//右下角
            -1.0f, -1.0f, 0.0f};//左下角

    //y纹理颠倒
    GLfloat uv[] = {
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 1.0, 0.0,
            0.0f, 1.0, 0, 0
    };


    unsigned int index[] = {0, 1, 2, 2, 3, 0};


    glvao->AddVertex3D(rectangleVertices, 4, 0);
    glvao->AddVertex3D(uv, 4, 1);
    glvao->setIndex(index, 6);
}

void MatrixShader::OnSurfaceChanged(int width, int height) {
    glProgram->OnSurfaceChanged(0, 0, width, height);
    surfaceWidth = width;
    surfaceHeight = height;
}

void MatrixShader::draw() {

}

void MatrixShader::draw(GLuint textureId) {

}

void MatrixShader::draw(GLuint textureId, float textLeft, float textTop, float textWidth,
                        float textHeight, float cx, float cy, float tx, float ty, float sc,
                        float r) {
    glProgram->useProgram();

    GLint textureIndex = glGetUniformLocation(glProgram->program, "textureMap");

    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, textureId);
    //glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
    glUniform1i(textureIndex, 0);


    offsetWithMatrixValue(textLeft, textTop, textWidth,textHeight,tx, ty, cx, cy);


    GLint widthIndex = glGetUniformLocation(glProgram->program, "width");
    glUniform1f(widthIndex, float(surfaceWidth));
    GLint heightIndex = glGetUniformLocation(glProgram->program, "height");
    glUniform1f(heightIndex, float(surfaceHeight));

    GLint txIndex = glGetUniformLocation(glProgram->program, "tx");
    glUniform1f(txIndex, tx);
    GLint tyIndex = glGetUniformLocation(glProgram->program, "ty");
    glUniform1f(tyIndex, ty);


    GLint cxIndex = glGetUniformLocation(glProgram->program, "cx");
    glUniform1f(cxIndex, cx);
    GLint cyIndex = glGetUniformLocation(glProgram->program, "cy");
    glUniform1f(cyIndex, cy);


    GLint scIndex = glGetUniformLocation(glProgram->program, "sc");
    //float readSc = float(surfaceWidth)/(float(surfaceWidth)*sc);
    glUniform1f(scIndex, sc);

    GLint rIndex = glGetUniformLocation(glProgram->program, "r");
    glUniform1f(rIndex, r);


    glvao->BindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void MatrixShader::offsetWithMatrixValue(float textLeft, float textTop, float textWidth,
                                         float textHeight, float &tx, float &ty, float &cx,
                                         float &cy) {
    float left = -1.f;
    float top = 1.f;

    /* float openglX = (x / float(width)) * 2.f - 1.f;
     float openglY = 1.f - (y / float(height)) * 2.f;*/
    float readTx = tx - textLeft;
    float readDy = ty - textTop;

    //float defaultCanterX =

    tx = (readTx / float(surfaceWidth)) * 2.f;
    ty = -(readDy / float(surfaceHeight)) * 2.f;

    float defaultCanterX = textLeft + textWidth / 2.f;
    float defaultCanterY = textTop + textHeight / 2.f;

    float surfaceCenterX = float(surfaceWidth)/2.f;
    float surfaceCenterY = float(surfaceHeight)/2.f;


    float readCx = defaultCanterX - surfaceCenterX;
    float readCy = defaultCanterY - surfaceCenterY;

    cx = (readCx / float(surfaceWidth)) * 2.f;
    cy = -(readCy / float(surfaceHeight)) * 2.f;
    /*cx = left + (cx / float(surfaceWidth)) * 2.f;
    cy = top - (cy / float(surfaceHeight)) * 2.f;*/

    LOGCATE("11111 tx %f ty %f", tx, ty)
    //tx =left+ ((tx / float(surfaceWidth)) * 2.f - 1.f);
    //tx = top-(1.f - (ty / float(surfaceHeight)) * 2.f);

}

//
// Created by admin on 2021/5/7.
//

#include "out_shader.h"

void OutShader::onDestroy() {

}

void OutShader::Init() {
    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "layout(location = 1) in vec3 uvPos;\n"
            "uniform mat4 uMatrix;\n"
            "out vec3 outUvPos;\n"

            "mat2 rotate2d(float _angle){\n"
            "    return mat2(cos(_angle),-sin(_angle),\n"
            "                sin(_angle),cos(_angle));\n"
            "}"

            "mat2 scale(vec2 _scale){\n"
            "    return mat2(_scale.x,0.0,\n"
            "                0.0,_scale.y);\n"
            "}"

            "void main()                              \n"
            "{                                        \n"

            "vec2 position = vec2(vPosition.x,vPosition.y);"
//            "position+=vec2(0.1851851851851852,-0.1683501683501684);"
//            "float ratio = 0.9090909090909091;"

           // "position -= vec2(0.5);"
//            "position.x = position.x * ratio;"
//            "position = rotate2d(radians(float(45))) * position;"
//            "position.x = position.x / ratio;"
            //"position += vec2(0.5);"

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

            "mat2 scale(vec2 _scale){\n"
            "    return mat2(_scale.x,0.0,\n"
            "                0.0,_scale.y);\n"
            "}"



            "void main()                                  \n"
            "{                                            \n"
            "vec2 uv = vec2(outUvPos.x,1.-outUvPos.y); \n"

            "ivec2 size = textureSize(textureMap, 0);"

           /* "float ratio = float(size.x)/float(size.y);"
            "uv -= vec2(0.5);"
            "uv.x = uv.x * ratio;"
           "uv = rotate2d(radians(float(-45))) * uv;"
            "uv.x = uv.x / ratio;"
            "uv += vec2(0.5);"*/


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

void OutShader::OnSurfaceChanged(int width, int height) {
    glProgram->OnSurfaceChanged(0, 0, width, height);
    surfaceWidth = width;
    surfaceHeight = height;
}

void OutShader::draw() {

}

void OutShader::draw(GLuint textureId) {
    glProgram->useProgram();

    GLint textureIndex = glGetUniformLocation(glProgram->program, "textureMap");

    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, textureId);
    //glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
    glUniform1i(textureIndex, 0);

    /*GLint transformLoc = glGetUniformLocation(glProgram->program, "uMatrix");
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));*/

    glvao->BindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

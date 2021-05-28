//
// Created by admin on 2021/5/28.
//

#include "frame_shader.h"

void FrameShader::Init() {
    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "layout(location = 1) in vec3 uvPos;\n"
            "uniform mat4 uMatrix;\n"
            "out vec3 outUvPos;\n"

            "void main()                              \n"
            "{                                        \n"

            "vec2 position = vec2(vPosition.x,vPosition.y);"
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

            "void main()                                  \n"
            "{                                            \n"
            "vec2 uv = vec2(outUvPos.x,1.-outUvPos.y); \n"
            "vec4 textureMap = texture(textureMap,uv); \n"
            "fragColor = textureMap;\n"
            "}                                            \n";


    glProgram = new GLProgram(vShaderStr, fboShaderStr);
    glvao = new GLVAO();

    vertexVbo = glvao->AddVertex2D(nullptr, 4, 0);
    uvVbo = glvao->AddVertex2D(nullptr, 4, 1);
    glvao->setIndex(nullptr, 6);
}


void FrameShader::OnSurfaceChanged(int width, int height) {

}

void FrameShader::onDestroy() {

}

void FrameShader::draw(GLuint textureId, float second) {


}


void FrameShader::draw() {

}
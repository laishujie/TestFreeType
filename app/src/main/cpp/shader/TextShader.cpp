//
// Created by admin on 2021/4/29.
//

#include "TextShader.h"
#include "logUtil.h"

void TextShader::Init() {
    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec2 vPosition;  \n"
            "layout(location = 1) in vec2 uvPos;\n"
            "uniform mat4 uMatrix;\n"
            "out vec2 outUvPos;\n"
            "void main()                              \n"
            "{                                        \n"
            "   gl_Position =  vec4(vPosition,1.0,1.0);              \n"
            "   outUvPos = uvPos;              \n"
            "}                                        \n";

    char fboShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "uniform sampler2D textureMap;\n"
            "in vec2 outUvPos;\n"
            "void main()                                  \n"
            "{                                            \n"
            "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
            //"vec4 textureMap = texture(textureMap,uv); \n"
            "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textureMap,uv).r);"

            "fragColor = vec4(vec3(1.0,0.0,0.0), sampled.a);"
            // "   fragColor = vec4 ( outPos, 1.0 ); \n"
            //   "   fragColor = textureMap;\n"
            "}                                            \n";


    glProgram = new GLProgram(vShaderStr, fboShaderStr);
    glvao = new GLVAO();

    textVbo = glvao->AddVertex2D(nullptr, 4, 0);
    uvVbo = glvao->AddVertex2D(nullptr, 4, 1);

    ebo = glvao->setIndex(nullptr, 6);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void TextShader::OnSurfaceChanged(int width, int height) {
    glViewport(0, 0, width, height);
    surfaceWidth = width;
    surfaceHeight = height;
}

void TextShader::onDestroy() {

}

struct PointF {
    float x;
    float y;
};

PointF vertexWithPoint(float x, float y, int width, int height) {
    float openglX = (x / float(width)) * 2.f - 1.f;
    float openglY = 1.f - (y / float(height)) * 2.f;


    return {openglX, openglY};
}

PointF normalizePoint(float x, float y, int width, int height) {

    return {x / float(width), y / float(height)};
}

void TextShader::drawText(GLuint areaTextureId, ftgl::texture_font_t *font, const char *text) {
    std::vector<GLfloat> vertex;
    std::vector<GLfloat> uvVertex;
    std::vector<unsigned int> indexVertex;
    float startX = 0;
    float startY = 0 + font->size;
    for (size_t i = 0; i < strlen(text); ++i) {
        //获取字形
        ftgl::texture_glyph_t *pGlyph = texture_font_find_glyph(font, text + i);

        if (pGlyph != nullptr && pGlyph->width != 0) {
            LOGCATE("i %d font->descender= %f\n "
                    "pGlyph->advance_x =%f pGlyph->advance_y =%f \n"
                    "glyph->offset_x =%d glyph->offset_y =%d \n"
                    "glyph->width =%d glyph->height =%d \n "
                    "glyph->s0 =%f glyph->t0 =%f \n "
                    "glyph->s1 =%f  glyph->t1 =%f",
                    i, font->descender,
                    pGlyph->advance_x, pGlyph->advance_y,
                    pGlyph->offset_x, pGlyph->offset_y,
                    pGlyph->width, pGlyph->height,
                    pGlyph->s0, pGlyph->t0,
                    pGlyph->s1, pGlyph->t1)
            //y - (ch.Size.y - ch.Bearing.y)
            float x0 = (startX + float(pGlyph->offset_x));
            float y0 = (startY - (float(pGlyph->offset_y)));

            float x1 = (x0 + float(pGlyph->width));
            float y1 = (y0 + float(pGlyph->height));

            const PointF &leftTop = vertexWithPoint(x0, y0, font->atlas->width,
                                                    font->atlas->height);
            const PointF &rightBottom = vertexWithPoint(x1, y1, font->atlas->width,
                                                        font->atlas->height);

            float s0 = pGlyph->s0;
            float t0 = pGlyph->t0;
            float s1 = pGlyph->s1;
            float t1 = pGlyph->t1;

            LOGCATE("leftTop.x %f leftTop.y %f rightBottom.x %f rightBottom.y %f",
                    leftTop.x, leftTop.y, rightBottom.x, rightBottom.y)

            float rectangleVertices[] = {
                    leftTop.x, leftTop.y,// 左上角
                    rightBottom.x, leftTop.y,//右上角
                    rightBottom.x, rightBottom.y,//右下角
                    leftTop.x, rightBottom.y};//左下角

            int index = vertex.size() * 0.5;
            int indices[] = {index, index + 1, index + 2,
                             index, index + 2, index + 3};
            //glvao->subDataIndex2D(ebo, index, 6);
            indexVertex.push_back(indices[0]);
            indexVertex.push_back(indices[1]);
            indexVertex.push_back(indices[2]);
            indexVertex.push_back(indices[3]);
            indexVertex.push_back(indices[4]);
            indexVertex.push_back(indices[5]);

            vertex.push_back(rectangleVertices[0]);
            vertex.push_back(rectangleVertices[1]);
            vertex.push_back(rectangleVertices[2]);
            vertex.push_back(rectangleVertices[3]);
            vertex.push_back(rectangleVertices[4]);
            vertex.push_back(rectangleVertices[5]);
            vertex.push_back(rectangleVertices[6]);
            vertex.push_back(rectangleVertices[7]);
            // vertex.push_back(rectangleVertices);


            GLfloat uv[] = {
                    s0, t0,
                    s1, t0,
                    s1, t1,
                    s0, t1,
            };
            uvVertex.push_back(uv[0]);
            uvVertex.push_back(uv[1]);
            uvVertex.push_back(uv[2]);
            uvVertex.push_back(uv[3]);
            uvVertex.push_back(uv[4]);
            uvVertex.push_back(uv[5]);
            uvVertex.push_back(uv[6]);
            uvVertex.push_back(uv[7]);
            //uvVertex.push_back(uv);



            startX += pGlyph->advance_x;
        }
    }


    glvao->updateVertex2D(textVbo, &vertex[0], vertex.size() / 2, 0);
    glvao->updateVertex2D(uvVbo, &uvVertex[0], uvVertex.size() / 2, 1);
    glvao->updateIndex(&indexVertex[0], indexVertex.size());

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glProgram->useProgram();

    GLint textureIndex = glGetUniformLocation(glProgram->program, "textureMap");

    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, areaTextureId);
    //glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
    glUniform1i(textureIndex, 0);


    glvao->BindVAO();
    //glDrawArrays(GL_TRIANGLES, 0, vertex.size()*4);
    glDrawElements(GL_TRIANGLES, indexVertex.size(), GL_UNSIGNED_INT, nullptr);
}

void TextShader::draw() {

}

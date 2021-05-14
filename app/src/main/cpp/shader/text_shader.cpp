//
// Created by admin on 2021/4/29.
//

#include "text_shader.h"
#include "logUtil.h"
#include "color_util.h"

void text_shader::Init() {
    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec2 vPosition;  \n"
            "layout(location = 1) in vec3 uvPos;\n"
            "uniform mat4 uMatrix;\n"
            "out vec3 outUvPos;\n"
            "void main()                              \n"
            "{                                        \n"
            "   gl_Position =  vec4(vPosition.x, vPosition.y, 0., 1.0);              \n"
            "   outUvPos = uvPos;              \n"
            "}                                        \n";

    char fboShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "uniform sampler2D textureMap;\n"
            "in vec3 outUvPos;\n"

            "uniform vec4 _OutlineColor;"
            "uniform vec4 _FontColor;"
            "uniform float _OutlineDistanceMark;"
            "uniform float _DistanceMark;"
            "uniform float _ShadowAlpha;"
            "mat2 scale(vec2 _scale){\n"
            "    return mat2(_scale.x,0.0,\n"
            "                0.0,_scale.y);\n"
            "}"

            "void main(){"
            "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
            "vec4 col = texture(textureMap,uv); \n"
            "float dist  = col.r;\n"
            "float width = fwidth(dist);\n"
            "float outline_center = 0.5;"
            "float glyph_center = _OutlineDistanceMark;"
            "float alpha = smoothstep(glyph_center-width, glyph_center+width, dist);"
            "vec3 outline_color = vec3(_OutlineColor.r,_OutlineColor.g,_OutlineColor.b);"
            "vec3 glyph_color = vec3(_FontColor.r,_FontColor.g,_FontColor.b);"
            "float mu = smoothstep(outline_center-width, outline_center+width, dist);"
            "vec3 rgb = mix(outline_color, glyph_color, mu);"
            "float finalA = max(alpha,mu);"
            /* "if(finalA==0.){"
             " fragColor = vec4(0.,0.,1., 1.);"
             "}else{"
             "fragColor = vec4(rgb, finalA);"
             "}"*/
            "fragColor = vec4(rgb, finalA);"
            "}";
    char ShadowShaderStr[] = "#version 300 es                              \n"
                             "precision mediump float;                     \n"
                             "out vec4 fragColor;                          \n"
                             "uniform sampler2D textureMap;\n"
                             "in vec3 outUvPos;\n"
                             "uniform float _ShadowAlpha;"
                             "uniform vec4 _ShadowColor;"

                             "void main(){"
                             "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
                             "vec4 col = texture(textureMap,uv); \n"
                             "float offsetDistance = texture(textureMap, uv).r;"
                             "float shadowAlpha = smoothstep(_ShadowAlpha, 0.5, offsetDistance);"
                             " vec4 shadow = vec4(_ShadowColor.rgb, _ShadowColor.a * shadowAlpha);"
                             "fragColor = shadow;"
                             "}";

    glProgram = new GLProgram(vShaderStr, fboShaderStr);
    shadowProgram = new GLProgram(vShaderStr, ShadowShaderStr);
    glvao = new GLVAO();

    textVbo = glvao->AddVertex2D(nullptr, 4, 0);
    uvVbo = glvao->AddVertex2D(nullptr, 4, 1);
    glvao->setIndex(nullptr, 6);
}

/*
void text_shader::OnSurfaceChanged(int width, int height) {
    glViewport(0, 0, width, height);
}
*/

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


void text_shader::DrawTextInfo(ftgl::texture_font_t *font, TextInfo *&textInfo) {
    DrawShadowText(textInfo, font);
    DrawStrokeNormalText(textInfo, font);
}

text_shader::text_shader() : shadowProgram(nullptr), shadowVao(nullptr), glProgram(nullptr), glvao(
        nullptr) {
}

text_shader::~text_shader() {
    if (shadowVao != nullptr) {
        delete shadowVao;
        shadowVao = nullptr;
    }
    if (shadowProgram != nullptr) {
        delete shadowProgram;
        shadowProgram = nullptr;
    }

    if (glvao != nullptr) {
        delete glvao;
        glvao = nullptr;
    }
    if (glProgram != nullptr) {
        delete glProgram;
        glProgram = nullptr;
    }
}

int text_shader::FillVertex(TextInfo *&textInfo,
                            ftgl::texture_font_t *font) {
    std::vector<GLfloat> vertex;
    std::vector<GLfloat> uvVertex;
    std::vector<unsigned int> indexVertex;
    float initX = textInfo->offset_x;
    float startX = initX;
    float initY = textInfo->offset_y;
    float startY = initY;
    lineSpace = startY;
    bool isHorizontal = textInfo->isHorizontal;
    const char *textChart = textInfo->text.c_str();
    lineSpace = isHorizontal ? startY : startX;

    for (size_t i = 0; i < strlen(textChart); ++i) {
        //获取字形
        ftgl::texture_glyph_t *pGlyph = texture_font_find_glyph(font, textChart + i);

        if (pGlyph != nullptr && pGlyph->width != 0) {
            if (isspace(textChart[i]) && !textInfo->isFromTemplate) {
                isHorizontal ? startX = initX : startY = initY;
                lineSpace += font->size;
            } else {
                if (isHorizontal)
                    startY = lineSpace;
                else {
                    float canterX = font->size / 2.f - pGlyph->width / 2.f;
                    //文字居中
                    startX = lineSpace + canterX;
                }
                float kerning = 0.0f;

                if (i > 0) {
                    kerning = texture_glyph_get_kerning(pGlyph, textChart + i - 1);
                }

                LOGCATE("text %c kerning %f i %d textInfo.x %f textInfo.y %ffont->descender= %f font->height %f\n "
                        "pGlyph->advance_x =%f pGlyph->advance_y =%f \n"
                        "glyph->offset_x =%d glyph->offset_y =%d \n"
                        "glyph->width =%d glyph->height =%d ascender %F\n "
                        "glyph->s0 =%f glyph->t0 =%f \n "
                        "glyph->s1 =%f  glyph->t1 =%f", textChart[i],
                        kerning,
                        i, textInfo->offset_x, textInfo->offset_y, font->descender, font->height,
                        pGlyph->advance_x, pGlyph->advance_y,
                        pGlyph->offset_x, pGlyph->offset_y,
                        pGlyph->width, pGlyph->height, font->ascender,
                        pGlyph->s0, pGlyph->t0,
                        pGlyph->s1, pGlyph->t1)

                startX += kerning;

                float centerX =
                        float(textInfo->surfaceWidth) * 0.5f - float(textInfo->textWidth) * 0.5f;
                float centerY =
                        float(textInfo->surfaceHeight) * 0.5f - float(textInfo->textHeight) * 0.5f;


                /*//取得图片和图片所处空间的比例
                float scaleBitmap = ((float) textInfo->textWidth) / (float) textInfo->textHeight;
                float scaleView =
                        ((float) textInfo->surfaceWidth) / (float) textInfo->surfaceHeight;
                // 空间的大小 / bitmap 的大小 = bitmap 缩放的倍数
                float scaleWidth = ((float) textInfo->surfaceWidth) / (float) textInfo->textWidth;
                float scaleHeight =
                        ((float) textInfo->surfaceHeight) / (float) textInfo->textHeight;
                float scale = 1.f;
                //按照比例选择缩放参照
                if (scaleBitmap < scaleView) {
                    //比例小于视框时，全部按照图片高度和视框高度的比例进行缩放
                    scale = scaleHeight;
                } else {
                    //比例大于视框时，全部按照图片宽度和视框宽度的比例进行缩放
                    scale = scaleWidth;
                }
*/
                float x0 =
                        startX + float(pGlyph->offset_x) - float(font->padding) + centerX ;
                //往下多少
                float y0 =startY + font->ascender - float(pGlyph->offset_y) - float(font->padding) +centerY;
                float x1 = x0 + float(pGlyph->width);
                float y1 = y0 + float(pGlyph->height);


                const PointF &leftTop = vertexWithPoint(x0, y0, textInfo->surfaceWidth,
                                                        textInfo->surfaceHeight);

                const PointF &rightBottom = vertexWithPoint(x1, y1, textInfo->surfaceWidth,
                                                            textInfo->surfaceHeight);


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
                /* GLfloat uv[] = {
                         s0, t1,
                         s1, t1,
                         s1, t0,
                         s0, t0,
                 };*/
                uvVertex.push_back(uv[0]);
                uvVertex.push_back(uv[1]);
                uvVertex.push_back(uv[2]);
                uvVertex.push_back(uv[3]);
                uvVertex.push_back(uv[4]);
                uvVertex.push_back(uv[5]);
                uvVertex.push_back(uv[6]);
                uvVertex.push_back(uv[7]);

                if (isHorizontal) {
                    startX += pGlyph->advance_x + float(textInfo->spacing);
                } else {
                    startY += font->size + float(textInfo->lineSpacing);
                }
            }
        }
    }

    glvao->updateVertex2D(textVbo, &vertex[0], vertex.size() / 2, 0);
    glvao->updateVertex2D(uvVbo, &uvVertex[0], uvVertex.size() / 2, 1);
    glvao->updateIndex(&indexVertex[0], indexVertex.size());
    return indexVertex.size();
}

int text_shader::DrawShadowText(TextInfo *&textInfo,
                                ftgl::texture_font_t *font) {

    //默认的0.5，不绘制
    if (textInfo->shadowDistance == 0.) {
        return 0;
    }
    //幅度值
    float angle = float(textInfo->shadowAngle) * 0.01745329252f;// pi / 180
    float x = textInfo->shadowDistance * cos(angle);
    float y = textInfo->shadowDistance * sin(angle);

    float temp_x = textInfo->offset_x;
    float temp_y = textInfo->offset_y;

    textInfo->offset_x = x;
    textInfo->offset_y = y;

    int indexSize = FillVertex(textInfo, font);

    textInfo->offset_x = temp_x;
    textInfo->offset_y = temp_y;

    if (indexSize == 0)return 0;
    shadowProgram->useProgram();

    GLint textureIndex2 = glGetUniformLocation(shadowProgram->program, "textureMap");
    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, font->atlas->id);
    //glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
    glUniform1i(textureIndex2, 0);

    GLint testShadowOffsetPixie2 = glGetUniformLocation(shadowProgram->program, "_ShadowAlpha");
    glUniform1f(testShadowOffsetPixie2, textInfo->shadowAlpha);
    GLint shadowColorLocation2 = glGetUniformLocation(shadowProgram->program, "_ShadowColor");
    glUniform4f(shadowColorLocation2, color_util::getColorR(textInfo->shadowColor),
                color_util::getColorG(textInfo->shadowColor),
                color_util::getColorB(textInfo->shadowColor), 1.0f);


    glvao->BindVAO();

    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, nullptr);

    return 0;
}

int text_shader::DrawStrokeNormalText(TextInfo *&textInfo, ftgl::texture_font_t *font) {
    glProgram->useProgram();

    int indexVertex = FillVertex(textInfo, font);

    GLint textureIndex = glGetUniformLocation(glProgram->program, "textureMap");

    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, font->atlas->id);
    //glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
    glUniform1i(textureIndex, 0);


    GLint distanceMarkIndex = glGetUniformLocation(glProgram->program, "_DistanceMark");
    glUniform1f(distanceMarkIndex, textInfo->distanceMark);
    GLint outLineMarkIndex = glGetUniformLocation(glProgram->program, "_OutlineDistanceMark");
    glUniform1f(outLineMarkIndex, textInfo->outlineDistanceMark);

    GLint fontColorLocation = glGetUniformLocation(glProgram->program, "_FontColor");
    glUniform4f(fontColorLocation, color_util::getColorR(textInfo->fontColor),
                color_util::getColorG(textInfo->fontColor),
                color_util::getColorB(textInfo->fontColor), 1.0f);

    GLint vertexColorLocation = glGetUniformLocation(glProgram->program, "_OutlineColor");
    glUniform4f(vertexColorLocation, color_util::getColorR(textInfo->outLineColor),
                color_util::getColorG(textInfo->outLineColor),
                color_util::getColorB(textInfo->outLineColor), 1.0f);


    glvao->BindVAO();
    //glDrawArrays(GL_TRIANGLES, 0, vertex.size()*4);
    glDrawElements(GL_TRIANGLES, indexVertex, GL_UNSIGNED_INT, nullptr);

    return 0;
}

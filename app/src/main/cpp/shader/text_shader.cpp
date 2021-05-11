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

            "#define AZUR  vec3(0.0, 0.5, 1.0)\n"
            "#define PURPLE vec3(1.0, 0.0, 0.5)\n"
            "#define MAGENTA vec3(1.0, 0.0, 1.0)\n"


            "uniform vec4 _OutlineColor;"
            "uniform vec4 _FontColor;"
            "uniform vec4 _ShadowColor;"
            "vec4 _GlowColor = vec4(MAGENTA,1. );"
            "uniform float _OutlineDistanceMark;"
            "vec4 _MainColor = vec4(AZUR, 1.0);"
            "uniform float _DistanceMark;"
            "uniform float _ShadowOffsetPixie;"
            "uniform int _ShadowAngleMark;"
            "float outline_center = 0.5;"
            "float glyph_center   = 0.2;"

            "uniform float _ShadowAlpha;"
            "mat2 scale(vec2 _scale){\n"
            "    return mat2(_scale.x,0.0,\n"
            "                0.0,_scale.y);\n"
            "}"

            "void main(){"
            /* "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
             "vec4 col = texture(textureMap,uv); \n"
             "float distance = col.r;"
             //r分量到了多少需要进行描边绘制
             "if(distance<_DistanceMark){"
             //实体描边 _OutlineDistanceMark 分量多少算描边 如果_X 大于或等于_Y，返回 1;否则 0，
             "vec4 stokeColor = _OutlineColor;"
             "stokeColor.a = step(_OutlineDistanceMark, distance);"
                  "if(stokeColor.a!=0.){"
                  //属于描边范围直接绘制
                  "fragColor = stokeColor;"
               "}"
             "}else{"
             //文字颜色
             "fragColor = _FontColor;"
             "}"
 */

            "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
            "vec4 col = texture(textureMap,uv); \n"

            "float dist  = col.r;\n"
            "float width = fwidth(dist);\n"
            "outline_center = _DistanceMark;"
            "glyph_center = _OutlineDistanceMark;"
            "float alpha = smoothstep(glyph_center-width, glyph_center+width, dist);"

            "vec3 outline_color = vec3(_OutlineColor.r,_OutlineColor.g,_OutlineColor.b);"
            "vec3 glyph_color = vec3(_FontColor.r,_FontColor.g,_FontColor.b);"

            "float mu = smoothstep(outline_center-width, outline_center+width, dist);"
            "vec3 rgb = mix(outline_color, glyph_color, mu);"
            "float finalA = max(alpha,mu);"
            "if(finalA==0.){"
            //属于外部范围，进行阴影绘制 这个浮点数是 pi / 180，角度转弧度
            "float rad = radians(float(_ShadowAngleMark));"
            //取得纹理大小
            "ivec2 size = textureSize(textureMap, 0);\n"
            //获取一个像素所占的值//单位坐标
            "vec2 unit = 1.0 / vec2(float(size.x),float(size.y));\n"
            // 根据半径可获取圆点偏移坐标点  //偏移量
            "vec2 offset = vec2(_ShadowOffsetPixie * cos(rad) * unit.x, _ShadowOffsetPixie * sin(rad) * unit.y);"
            //坐标点采集R分量
            "float offsetDistance = texture(textureMap, uv + offset).r;"
            "if(offsetDistance !=0.){"
            //进行线性插值
            "float a = smoothstep(0., _ShadowAlpha, offsetDistance);"
            "vec4 shadowColor = vec4(_ShadowColor.rgb,a*_ShadowColor.a);"
            "fragColor = shadowColor;}"
            "}else{"
               "fragColor = vec4(rgb, finalA);"
            "}"




            "}";





    glProgram = new GLProgram(vShaderStr, fboShaderStr);
    glvao = new GLVAO();

    textVbo = glvao->AddVertex2D(nullptr, 4, 0);
    uvVbo = glvao->AddVertex2D(nullptr, 4, 1);

    ebo = glvao->setIndex(nullptr, 6);

    /* glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    //LOGCATE("glProgram %d", glProgram->program)
}


void text_shader::OnSurfaceChanged(int width, int height) {
    glViewport(0, 0, width, height);
    surfaceWidth = width;
    surfaceHeight = height;
}

void text_shader::onDestroy() {

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

void text_shader::drawText(GLuint areaTextureId, int textWidth, int textHeight,
                           ftgl::texture_font_t *font, const char *text) {
    std::vector<GLfloat> vertex;
    std::vector<GLfloat> uvVertex;
    std::vector<unsigned int> indexVertex;
    float initX = float(textWidth) / 4.f;
    float startX = initX;
    float startY = 128 + 20;
    lineSpace = startY;
    for (size_t i = 0; i < strlen(text); ++i) {
        if (isspace(text[i])) {
            startX = initX;
            lineSpace += font->size;
        } else {
            startY = lineSpace;
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
                float y0 = (startY + (float(pGlyph->offset_y)));

                float x1 = (x0 + float(pGlyph->width));
                float y1 = (y0 - float(pGlyph->height));

                const PointF &leftTop = vertexWithPoint(x0, y0, textWidth,
                                                        textHeight);
                const PointF &rightBottom = vertexWithPoint(x1, y1, textWidth,
                                                            textHeight);

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
                        s0, t1,
                        s1, t1,
                        s1, t0,
                        s0, t0,
                };
                uvVertex.push_back(uv[0]);
                uvVertex.push_back(uv[1]);
                uvVertex.push_back(uv[2]);
                uvVertex.push_back(uv[3]);
                uvVertex.push_back(uv[4]);
                uvVertex.push_back(uv[5]);
                uvVertex.push_back(uv[6]);
                uvVertex.push_back(uv[7]);


                startX += pGlyph->advance_x;
            }
        }
    }
    lineSpace = 0;


    glvao->updateVertex2D(textVbo, &vertex[0], vertex.size() / 2, 0);
    glvao->updateVertex2D(uvVbo, &uvVertex[0], uvVertex.size() / 2, 1);
    glvao->updateIndex(&indexVertex[0], indexVertex.size());


    glProgram->useProgram();

    GLint textureIndex = glGetUniformLocation(glProgram->program, "textureMap");

    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, areaTextureId);
    //glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
    glUniform1i(textureIndex, 0);


    glvao->BindVAO();
    glDrawArrays(GL_TRIANGLES, 0, vertex.size()*4);
    //glDrawElements(GL_TRIANGLES, indexVertex.size(), GL_UNSIGNED_INT, nullptr);
}

void text_shader::draw() {

}

void
text_shader::drawTextInfo(GLuint areaTextureId, ftgl::texture_font_t *font, TextInfo *&textInfo) {
    std::vector<GLfloat> vertex;
    std::vector<GLfloat> uvVertex;
    std::vector<unsigned int> indexVertex;
    float initX = float(textInfo->textWidth) / 4.f;
    float initY = 128 + 20;
    float startX = initX;
    float startY = initY;
    bool isHorizontal = textInfo->isHorizontal;
    const char *textChart = textInfo->text.c_str();
    lineSpace = isHorizontal ? startY : startX;

    for (size_t i = 0; i < strlen(textChart); ++i) {
        //获取字形
        ftgl::texture_glyph_t *pGlyph = texture_font_find_glyph(font, textChart + i);

        if (pGlyph != nullptr && pGlyph->width != 0) {
            float kerning = 0.0f;

            if (i > 0) {
                kerning = texture_glyph_get_kerning(pGlyph, textChart+ i - 1);
            }

            if (isspace(textChart[i])) {
                isHorizontal ? startX = initX : startY = initY;
                lineSpace += font->size;
            } else {
                if (isHorizontal)
                    startY = lineSpace;
                else {
                    float canterX = font->size / 2.f-pGlyph->width/2.f;
                    //文字居中
                    startX = lineSpace+canterX;
                }

                LOGCATE("kerning %f i %d font->descender= %f font->size %f\n "
                        "pGlyph->advance_x =%f pGlyph->advance_y =%f \n"
                        "glyph->offset_x =%d glyph->offset_y =%d \n"
                        "glyph->width =%d glyph->height =%d \n "
                        "glyph->s0 =%f glyph->t0 =%f \n "
                        "glyph->s1 =%f  glyph->t1 =%f",
                        kerning,
                        i, font->descender, font->size,
                        pGlyph->advance_x, pGlyph->advance_y,
                        pGlyph->offset_x, pGlyph->offset_y,
                        pGlyph->width, pGlyph->height,
                        pGlyph->s0, pGlyph->t0,
                        pGlyph->s1, pGlyph->t1)
                //y - (ch.Size.y - ch.Bearing.y)
                float x0 = (startX + float(pGlyph->offset_x));
                float y0 = (startY + (float(-pGlyph->offset_y)));

                float x1 = (x0 + float(pGlyph->width));
                float y1 = (y0 + float(pGlyph->height));

                const PointF &leftTop = vertexWithPoint(x0, y0, textInfo->textWidth,
                                                        textInfo->textHeight);
                const PointF &rightBottom = vertexWithPoint(x1, y1, textInfo->textWidth,
                                                            textInfo->textHeight);

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


    glProgram->useProgram();

    GLint textureIndex = glGetUniformLocation(glProgram->program, "textureMap");

    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, areaTextureId);
    //glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
    glUniform1i(textureIndex, 0);


    GLint distanceMarkIndex = glGetUniformLocation(glProgram->program, "_DistanceMark");
    glUniform1f(distanceMarkIndex, textInfo->distanceMark);
    GLint outLineMarkIndex = glGetUniformLocation(glProgram->program, "_OutlineDistanceMark");
    glUniform1f(outLineMarkIndex, textInfo->outlineDistanceMark);

    GLint offsetMarkIndex = glGetUniformLocation(glProgram->program, "_ShadowOffsetPixie");
    glUniform1f(offsetMarkIndex, textInfo->shadowDistance);

    GLint shadowAngleMarkIndex = glGetUniformLocation(glProgram->program, "_ShadowAngleMark");
    glUniform1i(shadowAngleMarkIndex, _ShadowAngleMark);


    GLint testShadowOffsetPixie1 = glGetUniformLocation(glProgram->program, "_ShadowAlpha");
    glUniform1f(testShadowOffsetPixie1, textInfo->shadowAlpha);

    LOGCATE("textInfo->shadowAlpha %f",textInfo->shadowAlpha)
    GLint shadowColorLocation = glGetUniformLocation(glProgram->program, "_ShadowColor");
    glUniform4f(shadowColorLocation, color_util::getColorR(_ShadowColor), color_util::getColorG(_ShadowColor), color_util::getColorB(_ShadowColor), 1.0f);


    GLint fontColorLocation = glGetUniformLocation(glProgram->program, "_FontColor");
    glUniform4f(fontColorLocation, color_util::getColorR(textInfo->fontColor), color_util::getColorG(textInfo->fontColor), color_util::getColorB(textInfo->fontColor), 1.0f);

    GLint vertexColorLocation = glGetUniformLocation(glProgram->program, "_OutlineColor");
    glUniform4f(vertexColorLocation, color_util::getColorR(textInfo->outLineColor), color_util::getColorG(textInfo->outLineColor), color_util::getColorB(textInfo->outLineColor), 1.0f);


    glvao->BindVAO();
    //glDrawArrays(GL_TRIANGLES, 0, vertex.size()*4);
    glDrawElements(GL_TRIANGLES, indexVertex.size(), GL_UNSIGNED_INT, nullptr);
}

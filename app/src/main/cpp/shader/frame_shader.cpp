//
// Created by admin on 2021/5/28.
//

#include <logUtil.h>
#include "frame_shader.h"

void ImageFrameShader::Init() {
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
            "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
            "vec4 textureMap = texture(textureMap,uv); \n"
            "fragColor = textureMap;\n"
            "}                                            \n";

    /* "if(textureMap.a==0.0){"
               "fragColor = vec4(1.0,0.,0.,1.);\n"
               "}else{"
               "fragColor = textureMap;\n"
               "}"*/

    glProgram = new GLProgram(vShaderStr, fboShaderStr);
    glvao = new GLVAO();

    vertexVbo = glvao->AddVertex2D(nullptr, 4, 0);
    uvVbo = glvao->AddVertex2D(nullptr, 4, 1);
    glvao->setIndex(nullptr, 6);
}


void ImageFrameShader::OnSurfaceChanged(int width, int height) {
    surfaceWidth = width;
    surfaceHeight = height;
}

void ImageFrameShader::onDestroy() {

}

void ImageFrameShader::draw(GLuint textureId, float second) {
    glProgram->useProgram();

    GLint textureIndex = glGetUniformLocation(glProgram->program, "textureMap");

    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, textureId);
    //glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
    glUniform1i(textureIndex, 0);

    glvao->BindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}


void ImageFrameShader::draw() {

}


int ImageFrameShader::DrawFrame(int frame, TextInfo *&textInfo) {
    std::deque<TextImage> &deque = textInfo->textImages;
    unsigned int allSize = 0;
    unsigned int lastSize = 0;
    for (auto &img:deque) {
        allSize += img.frameCoordinates.size();
        if (allSize >= frame) {
            unsigned int readIndex = frame - lastSize - 1;

            try {
                FrameCoordinates &coordinates = img.frameCoordinates.at(readIndex);
                LOGCATE("frame %d index %d coordinates x %d ", frame, readIndex, coordinates.x)
                float width = img.size.x;
                float height = img.size.y;

                PointF leftTop = {float(coordinates.x) / width, float(coordinates.y) / height};
                //PointF leftTop = {0.0, 0.0};
                PointF rightBottom = {float(coordinates.x + coordinates.w) / width,
                                      float(coordinates.y + coordinates.h) / height};

                GLfloat uv[] = {
                        leftTop.x, leftTop.y,// 左上角
                        rightBottom.x, leftTop.y,//右上角
                        rightBottom.x, rightBottom.y,//右下角
                        leftTop.x, rightBottom.y};//左下角
                /* GLfloat uv[] = {
                         0.0, 0.0,
                         1.0, 0.0,
                         1.0, 1.0,
                         0.0f, 1.0,
                 };
     */
                glvao->updateVertex2D(uvVbo, uv, 4, 1);

                draw(img.textureId, 0.f);
            }
            catch (const std::out_of_range& oor) {
            }

            break;
        } else {
            lastSize = allSize;
        }
    }


    return 0;
}

int ImageFrameShader::InitFrame(int frame, TextInfo *&textInfo) {
    std::deque<TextImage> &deque = textInfo->textImages;
    unsigned int allSize = -1;

    for (auto &img:deque) {
        if (img.textureId == 0) {
            glGenTextures(1, &img.textureId);
            glBindTexture(GL_TEXTURE_2D, img.textureId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int iw, ih, n;

            // 加载图片获取宽、高、颜色通道信息
            unsigned char *idata = ImageLoad::load(img.frameImg.c_str(), &iw, &ih, &n, 0);

            //LOGCATE("===== textureId %d", textureId);
            //像素偏移1
            //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iw, ih, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         idata);
            glBindTexture(GL_TEXTURE_2D, 0);

            ImageLoad::imageFree(idata);
            /*allFrameCoordinates.insert(allFrameCoordinates.end(),
                                       std::make_move_iterator(img.frameCoordinates.begin()),
                                       std::make_move_iterator(img.frameCoordinates.end()));*/
        }
    }
    if (allSize >= frame) {
        FrameCoordinates &coordinates = deque.at(0).frameCoordinates.at(0);
        auto imgW = (float) coordinates.w;
        auto imgH = (float) coordinates.h;

        float templateRatioW = float(surfaceWidth) / 1920.f;
        //float templateRatioH = float(surfaceHeight) / 1920.f;

        float readWidth = imgW * templateRatioW;
        float readHeight = imgH * templateRatioW;

        //移动到中间
        float centerX = float(surfaceWidth) * 0.5f - readWidth * 0.5f;
        float centerY = float(surfaceHeight) * 0.5f - readHeight * 0.5f;

        PointF leftTop{centerX, centerY};
        PointF rightBottom{leftTop.x + readWidth, leftTop.y + readHeight};

        float offsetX = textInfo->offset_x * templateRatioW;
        float offsetY = textInfo->offset_y * templateRatioW;

        //模板偏移量
        leftTop.x += offsetX;
        rightBottom.x += offsetX;

        rightBottom.y += offsetY;
        leftTop.y += offsetY;

        leftTop = vertexWithPoint(leftTop.x, leftTop.y, surfaceWidth,
                                  surfaceHeight);
        rightBottom = vertexWithPoint(rightBottom.x, rightBottom.y, surfaceWidth,
                                      surfaceHeight);

        GLfloat rectangleVertices[] = {
                leftTop.x, leftTop.y,// 左上角
                rightBottom.x, leftTop.y,//右上角
                rightBottom.x, rightBottom.y,//右下角
                leftTop.x, rightBottom.y};//左下角

        unsigned int index[] = {0, 1, 2, 2, 3, 0};

        glvao->updateVertex2D(vertexVbo, rectangleVertices, 4, 0);
        glvao->updateIndex(index, 6);
        return 0;
    }
    return -1;
}

PointF ImageFrameShader::vertexWithPoint(float x, float y, int width, int height) {
    float openglX = (x / float(width)) * 2.f - 1.f;
    float openglY = 1.f - (y / float(height)) * 2.f;

    return {openglX, openglY};
}

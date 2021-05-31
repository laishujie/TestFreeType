//
// Created by admin on 2021/5/7.
//

#include "shader_manager.h"
#include "../util/logUtil.h"

/*
#include "ImageLoad.h"
#include "string"
*/

ShaderManager::ShaderManager() : textShader_(nullptr),
                                 fontManager_(
                                         nullptr), freeTypeShader(nullptr), outShader_(nullptr),
                                 matrixShader_(nullptr), imageFrameShader(nullptr) {
    textShader_ = new TextShader();
    matrixShader_ = new MatrixShader();
    outShader_ = new OutShader();
    imageFrameShader = new ImageFrameShader();
    //freeTypeShader = new FreeTypeShader();
}

ShaderManager::~ShaderManager() {
    LOGCATI("enter  %s", __func__)
    if (textShader_ != nullptr) {
        delete textShader_;
        textShader_ = nullptr;
    }
    if (fontManager_ != nullptr) {
        glDeleteTextures(1, &fontManager_->atlas->id);
        fontManager_->atlas->id = 0;
        font_manager_delete(fontManager_);
    }
    if (freeTypeShader != nullptr) {
        delete freeTypeShader;
        freeTypeShader = nullptr;
    }
    if (matrixShader_ != nullptr) {
        delete matrixShader_;
        matrixShader_ = nullptr;
    }
    if (outShader_ != nullptr) {
        delete outShader_;
        outShader_ = nullptr;
    }
    if (imageFrameShader != nullptr) {
        delete imageFrameShader;
        imageFrameShader = nullptr;
    }
    LOGCATI("leave: %s", __func__)
}

void ShaderManager::InitShader(int width, int height) {
    LOGCATI("enter %s", __func__)

    matrixShader_->Init();
    matrixShader_->OnSurfaceChanged(width, height);

    outShader_->Init();
    outShader_->OnSurfaceChanged(width, height);

    textShader_->Init();
    imageFrameShader->Init();
    imageFrameShader->OnSurfaceChanged(width, height);
    fontManager_ = ftgl::font_manager_new(width, height, 1);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    if (fontManager_->atlas->id == 0) {
        glGenTextures(1, &fontManager_->atlas->id);
        glBindTexture(GL_TEXTURE_2D, fontManager_->atlas->id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontManager_->atlas->width,
                     fontManager_->atlas->height,
                     0, GL_RED, GL_UNSIGNED_BYTE, fontManager_->atlas->data);
        // 设置纹理选项
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        LOGCATI("create %d ", fontManager_->atlas->id)
    }

    //TODO 这里重新设计
    ftgl::texture_font_t *pFont = ftgl::font_manager_get_from_filename(fontManager_,
                                                                       "/data/user/0/com.example.testfreetype/files/fonts/DroidSansFallback.ttf",
                                                                       float(72));
    pFont->rendermode = ftgl::RENDER_SIGNED_DISTANCE_FIELD;
    pFont->padding = 10;
    //获取对应得文本

    ftgl::texture_font_load_glyphs(pFont, "输入文字");

    LOGCATI("leave: %s", __func__)
}


ftgl::texture_font_t *
ShaderManager::InsetTextAndCalculate(TextInfo *&textInfo) const {
    LOGCATI("enter %s", __func__)

    const char *path = textInfo->ttf_file.c_str();
    const char *text = textInfo->text.c_str();


    //返回字体表
    ftgl::texture_font_t *pFont = ftgl::font_manager_get_from_filename(fontManager_, path,
                                                                       float(textInfo->fontSize));

    pFont->rendermode = ftgl::RENDER_SIGNED_DISTANCE_FIELD;
    pFont->padding = 10;
    //获取对应得文本

    int i = ftgl::texture_font_load_glyphs(pFont, text);

    if (i != 0) {
        //TODO 这里暂时不处理
        LOGCATI("纹理丢失的数量 %s %d ", text, i)
    } else {
        LOGCATI("纹理成功 %s %d ", text, i)
        size_t len = strlen(text);
        float xPointer = 0.f;
        float startX = -1.f;
        float endX = 0.0f;
        //计算文本宽高
        for (size_t j = 0; j < len; ++j) {
            //获取字形
            ftgl::texture_glyph_t *pGlyph = texture_font_find_glyph(pFont, text + j);
            if (pGlyph != nullptr && pGlyph->width != 0) {
                float x0 = xPointer + float(pGlyph->offset_x) - float(pFont->padding);
                float x1 = x0 + float(pGlyph->width);
                if (startX == -1.f) {
                    startX = x0;
                }
                endX = x1;

                xPointer += pGlyph->advance_x + float(textInfo->spacing);
            }
        }
        textInfo->textHeight = abs(pFont->ascender) + abs(pFont->descender);
        textInfo->textWidth = endX - startX;
    }

    LOGCATI("leave %s", __func__)
    return pFont;
}


int ShaderManager::DrawTextLayer(TextLayer *textLayer) {
    LOGCATI("enter ShaderManager %s", __func__)
    if (textLayer == nullptr) return -1;

    //层id为空时新建纹理
    if (textLayer->textureId == 0) {
        FboInfo fboInfo = fbo_util::CreateFbo(outShader_->getSurfaceWidth(),
                                              outShader_->getSurfaceHeight(), GL_RGBA);
        textLayer->textureId = fboInfo.textureId;
        textLayer->frameBuffer = fboInfo.frameBuffer;
    }


    if (textLayer->isDraw) {
        fbo_util::BindFbo(textLayer->frameBuffer);
        //前景色合成要用到
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        //区域重置
        textLayer->textArea.reset();
        //绘制文字层下的文字组
        for (auto &textInfo:textLayer->text_deque) {
            if (!textInfo->isInit) {
                InitTextInfo(textLayer, textInfo);
                textInfo->isInit = true;
            }

            if (textInfo->isTextImage) {
                DrawTextImage(textLayer, textInfo);
            } else {
                DrawTextInfo(textLayer, textInfo);
            }
        }

        fbo_util::UnBindFbo();

        textLayer->isDraw = false;
    }


    if (textLayer->applyMatrix) {
        matrixShader_->draw(textLayer->textureId, textLayer->tx, textLayer->ty, textLayer->sc,
                            textLayer->r);
    } else {
        outShader_->draw(textLayer->textureId);
    }


    /*long long int i = GetSysCurrentTime();
    int width = outShader_->getSurfaceWidth();
    int height = outShader_->getSurfaceHeight();
    unsigned char *buffer = new unsigned char[outShader_->getSurfaceWidth() *
                                              outShader_->getSurfaceHeight() * 4];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    std::string path = "/storage/emulated/0/DCIM/Camera/" + std::to_string(i) + ".png";

    LOGE("11111", "save %s ", path.c_str())

    if (!ImageLoad::savePng(path.c_str(), width, height, 4, buffer, 0)) {
        LOGE("11111", "ERROR: could not write image")
    }*/

    LOGCATI("leave ShaderManager %s", __func__)
    return 0;
}

int ShaderManager::DrawTextInfo(TextLayer *&textLayer, TextInfo *&textInfo) {
    const char *textChart = textInfo->text.c_str();
    const char *ttf_file = textInfo->ttf_file.c_str();

    LOGCATI("Draw info ttf %s textInfo->text %s textInfo->fontSize %d", ttf_file,
            textChart, textInfo->fontSize)

    if (textInfo->text.empty()) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        return 0;
    }

    textInfo->surfaceWidth = outShader_->getSurfaceWidth();
    textInfo->surfaceHeight = outShader_->getSurfaceHeight();

    //需要重新绘制阴影部分
    if (textInfo->shadowDistance != 0.) {
        LOGCATI("阴影绘制，重新生成点顶点数据")
        //幅度值
        float angle = float(textInfo->shadowAngle) * 0.01745329252f;// pi / 180
        float x = textInfo->shadowDistance * cos(angle);
        float y = textInfo->shadowDistance * sin(angle);

        float temp_x = textInfo->offset_x;
        float temp_y = textInfo->offset_y;

        textInfo->offset_x = x;
        textInfo->offset_y = y;

        //需要重新生成阴影偏移点
        ftgl::texture_font_t *pFont = InsetTextAndCalculate(textInfo);
        textInfo->indexVertex = textShader_->FillVertex(textInfo, pFont);

        textShader_->DrawShadowText(textInfo, fontManager_->atlas->id);

        textInfo->offset_x = temp_x;
        textInfo->offset_y = temp_y;
    }

    //需要重新填充顶点数据，调整了阴影的话需要
    if (textInfo->isCreateVertexAndSet() || textInfo->shadowDistance != 0.) {
        LOGCATI("重新生成顶点，并赋值边框")
        ftgl::texture_font_t *pFont = InsetTextAndCalculate(textInfo);
        textInfo->indexVertex = textShader_->FillVertex(textInfo, pFont);

        //计算最大文字边框
        if (textLayer->textArea.getWidth() == 0.f) {
            textLayer->textArea = textInfo->area;
        } else {
            textLayer->textArea.left = std::min(textLayer->textArea.left,
                                                textInfo->area.left);
            textLayer->textArea.right = std::max(textLayer->textArea.right,
                                                 textInfo->area.right);
            textLayer->textArea.top = std::min(textLayer->textArea.top, textInfo->area.top);
            textLayer->textArea.bottom = std::max(textLayer->textArea.bottom,
                                                  textInfo->area.bottom);
        }
        textLayer->isChangeTextArea = true;
    } else {
        //TODo 应该时一组文字对应一个vao这里要改
        ftgl::texture_font_t *pFont = InsetTextAndCalculate(textInfo);
        textInfo->indexVertex = textShader_->FillVertex(textInfo, pFont);
        textLayer->isChangeTextArea = false;
        LOGCATI("不需要重新生成顶点")
    }

    //绘制普通不带阴影的值
    textShader_->DrawStrokeNormalText(textInfo, fontManager_->atlas->id);
    return 0;
}



int ShaderManager::DrawTextImage(TextLayer *&textLayer, TextInfo *&textInfo) {
    DrawFrame(textLayer->frameIndex, textInfo);
    return 0;
}

void ShaderManager::DrawFrame(int frame, TextInfo *&pInfo) {
    imageFrameShader->DrawFrame(frame, pInfo);
    //outShader_->draw(pInfo->textImages[1].textureId);
}

void ShaderManager::InitTextInfo(TextLayer *pLayer, TextInfo *&pInfo) {
    LOGCATI("enter ShaderManager %s", __func__)
    if (pInfo->isTextImage) {
        imageFrameShader->InitFrame(pLayer->frameIndex, pInfo);
    }
    LOGCATI("leave ShaderManager %s", __func__)
}


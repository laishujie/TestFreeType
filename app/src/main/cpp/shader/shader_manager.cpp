//
// Created by admin on 2021/5/7.
//

#include "shader_manager.h"
#include "logUtil.h"

void shader_manager::DrawTextInfo(TextInfo *textInfo) {
    LOGCATI("enter shader_manager %s", __func__)
    if (textLayer_ == nullptr) {
        return;
    }

    const char *textChart = textInfo->text.c_str();
    const char *ttf_file = textInfo->ttf_file.c_str();

    LOGCATI("info ttf %s textInfo->text %s textInfo->fontSize %d", ttf_file,
            textChart, textInfo->fontSize)

    if (textInfo->text.empty()) {
        fbo_util::UnBindFbo();
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    fbo_util::BindFbo(textLayer_->frameBuffer);

    //glViewport(0,0,200,200);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    textInfo->textWidth = outShader_->getSurfaceWidth();
    textInfo->textHeight = outShader_->getSurfaceHeight();

    ftgl::texture_font_t *pFont = InsetText(ttf_file, textChart,
                                            textInfo->fontSize);

    textShader_->DrawTextInfo(pFont, textInfo);

    fbo_util::UnBindFbo();

    outShader_->draw(textLayer_->textureId);


    /*  InsetText(ttf_file, textChart,textInfo->fontSize);
     freeTypeShader->draw(fontManager_->atlas->id);*/
    // LOGCATE("fboTexture %d fbo %d", textLayer_->textureId, textLayer_->frameBuffer)
    LOGCATI("leave: shader_manager %s", __func__)
}

shader_manager::shader_manager() : textShader_(nullptr), outShader_(nullptr), textLayer_(nullptr),
                                   fontManager_(
                                           nullptr), freeTypeShader(nullptr) {
    textShader_ = new text_shader();
    outShader_ = new out_shader();
    freeTypeShader = new FreeTypeShader();
}

shader_manager::~shader_manager() {
    LOGCATI("enter  %s", __func__)
    if (textShader_ != nullptr) {
        delete textShader_;
        textShader_ = nullptr;
    }
    if (outShader_ != nullptr) {
        delete outShader_;
        outShader_ = nullptr;
    }
    if (fontManager_ != nullptr) {
        glDeleteTextures(1, &fontManager_->atlas->id);
        fontManager_->atlas->id = 0;
        font_manager_delete(fontManager_);
    }
    if (textLayer_ != nullptr) {
        delete textLayer_;
        textLayer_ = nullptr;
    }
    if (freeTypeShader != nullptr) {
        delete freeTypeShader;
        freeTypeShader = nullptr;
    }
    LOGCATI("leave: %s", __func__)
}

void shader_manager::InitShader(int width, int height) {
    LOGCATI("enter %s", __func__)

    outShader_->Init();
    outShader_->OnSurfaceChanged(width, height);
    textShader_->Init();
    freeTypeShader->Init();
    fontManager_ = ftgl::font_manager_new(width, height, 1);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendEquation(GL_MAX);


    if (textLayer_ == nullptr) {
        FboInfo fboInfo = fbo_util::CreateFbo(outShader_->getSurfaceWidth(),
                                              outShader_->getSurfaceHeight(), GL_RGBA);
        textLayer_ = new TextLayer(1, fboInfo);
    }
    LOGCATI("leave: %s", __func__)
}


ftgl::texture_font_t *
shader_manager::InsetText(const char *path, const char *text, int fontSize) const {
    LOGCATI("enter %s", __func__)

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

    //返回字体表
    ftgl::texture_font_t *pFont = ftgl::font_manager_get_from_filename(fontManager_, path,
                                                                       float(fontSize));
    pFont->rendermode = ftgl::RENDER_SIGNED_DISTANCE_FIELD;
    pFont->padding = 10;
    //获取对应得文本

    int i = ftgl::texture_font_load_glyphs(pFont, text);

    if (i != 0) {
        //TODO 这里暂时不处理
        LOGCATI("纹理丢失的数量 %s %d ", text, i)
    } else {
        LOGCATI("纹理成功 %s %d ", text, i)
    }

    LOGCATI("leave %s", __func__)
    return pFont;
}


int shader_manager::DrawTextLayer(TextLayer *textLayer) {
    if (textLayer == nullptr) return -1;

    if (textLayer->textureId == 0) {
        FboInfo fboInfo = fbo_util::CreateFbo(outShader_->getSurfaceWidth(),
                                              outShader_->getSurfaceHeight(), GL_RGBA);
        textLayer->textureId = fboInfo.textureId;
        textLayer->frameBuffer = fboInfo.frameBuffer;
    }

    fbo_util::BindFbo(textLayer->frameBuffer);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto textInfo:textLayer->text_deque) {
        const char *textChart = textInfo->text.c_str();
        const char *ttf_file = textInfo->ttf_file.c_str();

        LOGCATI("info ttf %s textInfo->text %s textInfo->fontSize %d", ttf_file,
                textChart, textInfo->fontSize)

        if (textInfo->text.empty()) {
            fbo_util::UnBindFbo();
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            return 0;
        }

        //glViewport(0,0,200,200);
        /*glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);*/

        textInfo->textWidth = outShader_->getSurfaceWidth();
        textInfo->textHeight = outShader_->getSurfaceHeight();

        ftgl::texture_font_t *pFont = InsetText(ttf_file, textChart,
                                                textInfo->fontSize);

        textShader_->DrawTextInfo(pFont, textInfo);
    }
    fbo_util::UnBindFbo();

    outShader_->draw(textLayer->textureId);
    return 0;
}


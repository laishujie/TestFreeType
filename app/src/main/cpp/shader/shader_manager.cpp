//
// Created by admin on 2021/5/7.
//

#include "shader_manager.h"
#include "logUtil.h"

void ShaderManager::DrawPreViewTextInfo(TextInfo *textInfo) {
    LOGCATI("enter ShaderManager %s", __func__)
    if (previewLayer_ == nullptr) {
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

    fbo_util::BindFbo(previewLayer_->frameBuffer);

    //glViewport(0,0,200,200);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    textInfo->surfaceWidth = outShader_->getSurfaceWidth();
    textInfo->surfaceHeight = outShader_->getSurfaceHeight();

    ftgl::texture_font_t *pFont = InsetText(textInfo);

    textShader_->DrawTextInfo(pFont, textInfo);

    fbo_util::UnBindFbo();

    outShader_->draw(previewLayer_->textureId);


    /*  InsetText(ttf_file, textChart,textInfo->fontSize);
     freeTypeShader->draw(fontManager_->atlas->id);*/
    // LOGCATE("fboTexture %d fbo %d", previewLayer_->textureId, previewLayer_->frameBuffer)
    LOGCATI("leave: ShaderManager %s", __func__)
}

ShaderManager::ShaderManager() : textShader_(nullptr), outShader_(nullptr), previewLayer_(nullptr),
                                 fontManager_(
                                         nullptr), freeTypeShader(nullptr) {
    textShader_ = new TextShader();
    outShader_ = new OutShader();
    freeTypeShader = new FreeTypeShader();
}

ShaderManager::~ShaderManager() {
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
    if (previewLayer_ != nullptr) {
        delete previewLayer_;
        previewLayer_ = nullptr;
    }
    if (freeTypeShader != nullptr) {
        delete freeTypeShader;
        freeTypeShader = nullptr;
    }
    LOGCATI("leave: %s", __func__)
}

void ShaderManager::InitShader(int width, int height) {
    LOGCATI("enter %s", __func__)

    outShader_->Init();
    outShader_->OnSurfaceChanged(width, height);
    textShader_->Init();
    freeTypeShader->Init();
    fontManager_ = ftgl::font_manager_new(width, height, 1);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    if (previewLayer_ == nullptr) {
        FboInfo fboInfo = fbo_util::CreateFbo(outShader_->getSurfaceWidth(),
                                              outShader_->getSurfaceHeight(), GL_RGBA);
        previewLayer_ = new TextLayer(1, fboInfo);
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    LOGCATI("leave: %s", __func__)
}


ftgl::texture_font_t *
ShaderManager::InsetText(TextInfo *&textInfo) const {
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
        float textWidth = 0;
        float textHeight;

        size_t len = strlen(text);
        //计算文本宽高
        for (size_t j = 0; j < len; ++j) {
            //获取字形
            ftgl::texture_glyph_t *pGlyph = texture_font_find_glyph(pFont, text + j);
            if (pGlyph != nullptr && pGlyph->width != 0) {
                textWidth += pGlyph->advance_x;
            }
        }
        textHeight = pFont->height;
        textInfo->textHeight = textHeight;
        textInfo->textWidth = textWidth;
        LOGCATE("textInfo->textWidth %f ", textWidth)
    }

    LOGCATI("leave %s", __func__)
    return pFont;
}


int ShaderManager::DrawTextLayer(TextLayer *textLayer) {
    if (textLayer == nullptr) return -1;

    if (textLayer->textureId == 0) {
        FboInfo fboInfo = fbo_util::CreateFbo(outShader_->getSurfaceWidth(),
                                              outShader_->getSurfaceHeight(), GL_RGBA);
        textLayer->textureId = fboInfo.textureId;
        textLayer->frameBuffer = fboInfo.frameBuffer;
    }

    fbo_util::BindFbo(textLayer->frameBuffer);
    //前景色合成要用到
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto textInfo:textLayer->text_deque) {

        const char *textChart = textInfo->text.c_str();
        const char *ttf_file = textInfo->ttf_file.c_str();

        LOGCATI("info ttf %s textInfo->text %s textInfo->fontSize %d", ttf_file,
                textChart, textInfo->fontSize)

        if (textInfo->text.empty()) {
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            return 0;
        }

        textInfo->surfaceWidth = outShader_->getSurfaceWidth();
        textInfo->surfaceHeight = outShader_->getSurfaceHeight();

        ftgl::texture_font_t *pFont = InsetText(textInfo);

        //绘制文字
        textShader_->DrawTextInfo(pFont, textInfo);
    }
    fbo_util::UnBindFbo();

    outShader_->draw(textLayer->textureId);
    return 0;
}


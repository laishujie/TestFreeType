//
// Created by admin on 2021/5/6.
//

#include <text_shader.h>
#include "text_control.h"


enum RenderMessage {
    kEGLCreate = 0,
    kEGLDestroy,
    kEGLWindowCreate,
    kEGLWindowDestroy,
    kDRAW
};


text_control::text_control() : Handler(),
                               window_(nullptr),
                               buffer_pool_(nullptr),
                               core_(nullptr), render_surface_(nullptr),
                               surface_height_(0),
                               surface_width_(0), message_queue_(), message_queue_thread_(),
                               fontManager_(nullptr), textShader_(nullptr), current_text_(nullptr),freeTypeShader(
                nullptr) {
    buffer_pool_ = new BufferPool(sizeof(Message));
    message_queue_ = new MessageQueue("text_control Message Queue");
    InitMessageQueue(message_queue_);

    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kEGLCreate;
    PostMessage(message);

    //默认插入字符表
    fontManager_ = ftgl::font_manager_new(512, 512, 1);
    textShader_ = new text_shader();
    freeTypeShader= new FreeTypeShader();
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&message_queue_thread_, &attr, MessageQueueThread, this);
}

text_control::~text_control() {
    LOGCATI("enter %s", __func__);

    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kEGLDestroy;
    PostMessage(message);

    auto msg = buffer_pool_->GetBuffer<Message>();
    msg->what = MESSAGE_QUEUE_LOOP_QUIT_FLAG;
    PostMessage(msg);
    pthread_join(message_queue_thread_, nullptr);

    if (nullptr != message_queue_) {
        message_queue_->Abort();
        delete message_queue_;
        message_queue_ = nullptr;
    }
    if (current_text_ != nullptr) {
        delete current_text_;
        current_text_ = nullptr;
    }
    if (nullptr != buffer_pool_) {
        delete buffer_pool_;
        buffer_pool_ = nullptr;
    }
    LOGCATI("leave: %s", __func__);
}

int text_control::Init() {
    return 0;
}

void text_control::OnSurfaceCreated(ANativeWindow *window, int width, int height) {
    LOGCATI("enter: %s", __func__);
    window_ = window;
    surface_width_ = width;
    surface_height_ = height;
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kEGLWindowCreate;
    PostMessage(message);
    LOGCATI("leave: %s", __func__);
}

void text_control::OnSurfaceChanged(int width, int height) {
    LOGCATI("enter: %s width: %d height: %d", __func__, width, height);
    surface_width_ = width;
    surface_height_ = height;
    LOGCATI("leave: %s", __func__);
}

void text_control::OnSurfaceDestroy() {
    LOGCATI("enter: %s", __func__);
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kEGLWindowDestroy;
    PostMessage(message);
    LOGCATI("leave: %s", __func__);
}


void text_control::HandleMessage(Message *msg) {
    LOGCATE("HandleMessage-----------------")

    int what = msg->GetWhat();
    void *obj = msg->GetObj();
    switch (what) {
        case kEGLCreate:
            OnGLCreate();
            break;
        case kEGLWindowCreate:
            OnGLWindowCreate();
            break;
        case kEGLDestroy:
            OnGlFontDestroy();
            OnGLDestroy();
            break;
        case kEGLWindowDestroy:
            OnGLWindowDestroy();
            break;
        case kDRAW: {
            auto *textInfo = reinterpret_cast<TextInfo *>(obj);
            ftgl::texture_font_t *pFont = inset_text(textInfo->ttf_file, textInfo->text);

           // textShader_->drawText(fontManager_->atlas->id, pFont, textInfo->text);
            freeTypeShader->draw(fontManager_->atlas->id);
            if (!core_->SwapBuffers(render_surface_)) {
                LOGCATE("eglSwapBuffers error: %d", eglGetError())
            }
        }
            break;
        default:
            break;
    }
}

void text_control::OnGLWindowCreate() {
    render_surface_ = core_->CreateWindowSurface(window_);
    if (nullptr != render_surface_ && EGL_NO_SURFACE != render_surface_) {
        auto result = core_->MakeCurrent(render_surface_);
        if (!result) {
            LOGCATE("MakeCurrent error");
            return;
        }
    }

    if (textShader_ == nullptr) {
        textShader_ = new text_shader();
    }
    textShader_->Init();
    textShader_->OnSurfaceChanged(surface_width_, surface_height_);

    freeTypeShader->Init();
    freeTypeShader->OnSurfaceChanged(surface_width_, surface_height_);
}

void text_control::OnGLCreate() {
    LOGCATI("enter %s", __func__);
    core_ = new EGLCore();
    auto result = core_->Init();
    if (!result) {
        LOGCATE("create EGLContext failed");
        return;
    }
    LOGCATI("leave %s", __func__);
}

void text_control::OnGLWindowDestroy() {
    LOGCATE("enter %s", __func__);
    if (nullptr != core_ && EGL_NO_SURFACE != render_surface_) {
        core_->ReleaseSurface(render_surface_);
        render_surface_ = EGL_NO_SURFACE;
    }
    LOGCATE("leave %s", __func__);
}

void text_control::OnGLDestroy() {
    LOGCATI("enter %s", __func__);
    if (nullptr != core_) {
        if (EGL_NO_SURFACE != render_surface_) {
            LOGE("%s MakeCurrent: %p", __func__, render_surface_);
            core_->MakeCurrent(render_surface_);
        }
        core_->Release();
        delete core_;
        core_ = nullptr;
    }
    LOGCATI("leave %s", __func__);

}

void *text_control::MessageQueueThread(void *args) {
    auto *player = reinterpret_cast<text_control *>(args);
    player->ProcessMessage();
    pthread_exit(nullptr);
}

void text_control::ProcessMessage() {
    LOGCATI("enter %s", __func__);
    bool rendering = true;
    while (rendering) {
        Message *msg = nullptr;
        if (message_queue_->DequeueMessage(&msg, true) > 0) {
            if (nullptr != msg) {
                if (MESSAGE_QUEUE_LOOP_QUIT_FLAG == msg->Execute()) {
                    LOGCATE("MESSAGE_QUEUE_LOOP_QUIT_FLAG");
                    rendering = false;
                }
                if (nullptr != buffer_pool_) {
                    buffer_pool_->ReleaseBuffer(msg);
                }
            }
        }
    }
    LOGCATI("leave %s", __func__);
}

void text_control::OnGlFontDestroy() {
    LOGCATI("enter %s", __func__)

    if (fontManager_ != nullptr) {
        glDeleteTextures(1, &fontManager_->atlas->id);
        fontManager_->atlas->id = 0;
        font_manager_delete(fontManager_);
        delete fontManager_;
        fontManager_ = nullptr;
    }
    if (textShader_ != nullptr) {
        delete textShader_;
        textShader_ = nullptr;
    }
    if (freeTypeShader != nullptr) {
        delete freeTypeShader;
        freeTypeShader = nullptr;
    }

    LOGCATI("leave %s", __func__)
}

ftgl::texture_font_t *text_control::inset_text(const char *path, const char *text) const {

    if (fontManager_->atlas->id == 0){
        glGenTextures(1, &fontManager_->atlas->id);
        glBindTexture(GL_TEXTURE_2D, fontManager_->atlas->id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontManager_->atlas->width, fontManager_->atlas->height,
                     0, GL_RED, GL_UNSIGNED_BYTE, fontManager_->atlas->data);
        // 设置纹理选项
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    LOGCATI("enter %s", __func__)
    //返回字体表
    ftgl::texture_font_t *pFont = ftgl::font_manager_get_from_filename(fontManager_, path, 32);
    pFont->rendermode = ftgl::RENDER_NORMAL;
    //获取对应得文本

    ftgl::texture_font_load_glyphs(pFont, text);

    glBindTexture(GL_TEXTURE_2D, fontManager_->atlas->id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fontManager_->atlas->width,
                    fontManager_->atlas->height,
                    GL_RED, GL_UNSIGNED_BYTE, fontManager_->atlas->data);

    /*int i = ftgl::texture_font_load_glyphs_isOk(pFont, text);

    if (i == 1) {
        glBindTexture(GL_TEXTURE_2D, fontManager_->atlas->id);

        *//*glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fontManager_->atlas->width,
                        fontManager_->atlas->height,
                        GL_R8, GL_UNSIGNED_BYTE, fontManager_->atlas->data);*//*
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontManager_->atlas->width, fontManager_->atlas->height,
                     0, GL_RED, GL_UNSIGNED_BYTE, fontManager_->atlas->data);
        
        LOGCATI("更新字符表 %s ", text)
    } else {
        LOGCATI("无需更新字符表")
    }*/
    LOGCATI("leave %s", __func__)
    return pFont;
}

void text_control::Draw(char *ttfPath, char *text) {

    if (current_text_ == nullptr) {
        current_text_ = new TextInfo();
    }
    if (current_text_->text != nullptr) {
        delete current_text_->text;
        delete current_text_->ttf_file;
        current_text_->text = nullptr;
        current_text_->ttf_file = nullptr;
    }

    current_text_->text = text;
    current_text_->ttf_file = ttfPath;
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kDRAW;
    message->obj = current_text_;
    PostMessage(message);
}




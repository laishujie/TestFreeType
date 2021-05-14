//
// Created by admin on 2021/5/6.
//

#include <ImageLoad.h>
#include <shader_manager.h>
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
                               surface_width_(0), message_queue_(nullptr), message_queue_thread_(),
                               shaderManager_(nullptr),
                               current_text_(nullptr) {
    buffer_pool_ = new BufferPool(sizeof(Message));
    message_queue_ = new MessageQueue("text_control Message Queue");
    InitMessageQueue(message_queue_);

    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kEGLCreate;
    PostMessage(message);
    shaderManager_ = new shader_manager();

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&message_queue_thread_, &attr, MessageQueueThread, this);
}

text_control::~text_control() {
    LOGCATI("enter %s", __func__)

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
    LOGCATI("leave: %s", __func__)
}

int text_control::Init() {
    return 0;
}

void text_control::OnSurfaceCreated(ANativeWindow *window, int width, int height) {
    LOGCATI("enter: %s", __func__)
    window_ = window;
    surface_width_ = width;
    surface_height_ = height;
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kEGLWindowCreate;
    PostMessage(message);
    LOGCATI("leave: %s", __func__)
}

void text_control::OnSurfaceChanged(int width, int height) {
    LOGCATI("enter: %s width: %d height: %d", __func__, width, height)
    surface_width_ = width;
    surface_height_ = height;
    LOGCATI("leave: %s", __func__)
}

void text_control::OnSurfaceDestroy() {
    LOGCATI("enter: %s", __func__)
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kEGLWindowDestroy;
    PostMessage(message);
    LOGCATI("leave: %s", __func__)
}


void text_control::HandleMessage(Message *msg) {
    int what = msg->GetWhat();
    void *obj = msg->GetObj();
    LOGCATE("HandleMessage----------%d ", what)

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
            LOGCATI("enter kDRAW %s", __func__)


            if(obj!= nullptr){
                auto *textLayer = reinterpret_cast<TextLayer *>(obj);
                shaderManager_->DrawTextLayer(textLayer);
            }else{
                shaderManager_->DrawTextInfo(current_text_);
            }


            /*if (!ImageLoad::savePng(textInfo->outPath, fontManager_->atlas->width,
                                    fontManager_->atlas->height, 1,
                                    fontManager_->atlas->data, 0)) {
                LOGE("11111", "ERROR: could not write image");
            }*/

            //textShader_->drawText(fontManager_->atlas->id, pFont, textInfo->text);
            //freeTypeShader->draw(fontManager_->atlas->id);

            if (!core_->SwapBuffers(render_surface_)) {
                LOGCATE("eglSwapBuffers error: %d", eglGetError())
            }
            LOGCATI(" leave %s", __func__)
        }
            break;
        default:
            break;
    }
}

void text_control::OnGLWindowCreate() {
    LOGCATI("enter %s", __func__)

    render_surface_ = core_->CreateWindowSurface(window_);

    if (nullptr != render_surface_ && EGL_NO_SURFACE != render_surface_) {
        auto result = core_->MakeCurrent(render_surface_);
        if (!result) {
            LOGCATE("MakeCurrent error")
            return;
        }
    }

    shaderManager_->InitShader(surface_width_, surface_height_);

    LOGCATI("leave %s", __func__)
}

void text_control::OnGLCreate() {
    LOGCATI("enter %s", __func__)
    core_ = new EGLCore();
    auto result = core_->Init();
    if (!result) {
        LOGCATE("create EGLContext failed")
        return;
    }
    LOGCATI("leave %s", __func__)
}

void text_control::OnGLWindowDestroy() {
    LOGCATE("enter %s", __func__)
    if (nullptr != core_ && EGL_NO_SURFACE != render_surface_) {
        core_->ReleaseSurface(render_surface_);
        render_surface_ = EGL_NO_SURFACE;
    }
    LOGCATE("leave %s", __func__)
}

void text_control::OnGLDestroy() {
    LOGCATI("enter %s", __func__)
    if (nullptr != core_) {
        if (EGL_NO_SURFACE != render_surface_) {
            LOGE("%s MakeCurrent: %p", __func__, render_surface_)
            core_->MakeCurrent(render_surface_);
        }
        core_->Release();
        delete core_;
        core_ = nullptr;
    }
    LOGCATI("leave %s", __func__)

}

void *text_control::MessageQueueThread(void *args) {
    auto *player = reinterpret_cast<text_control *>(args);
    player->ProcessMessage();
    pthread_exit(nullptr);
}

void text_control::ProcessMessage() {
    LOGCATI("enter %s", __func__)
    bool rendering = true;
    while (rendering) {
        Message *msg = nullptr;
        if (message_queue_->DequeueMessage(&msg, true) > 0) {
            if (nullptr != msg) {
                if (MESSAGE_QUEUE_LOOP_QUIT_FLAG == msg->Execute()) {
                    LOGCATE("MESSAGE_QUEUE_LOOP_QUIT_FLAG")
                    rendering = false;
                }
                if (nullptr != buffer_pool_) {
                    buffer_pool_->ReleaseBuffer(msg);
                }
            }
        }
    }
    LOGCATI("leave %s", __func__)
}

void text_control::OnGlFontDestroy() {
    LOGCATI("enter %s", __func__)

    if (shaderManager_ != nullptr) {
        delete shaderManager_;
        shaderManager_ = nullptr;
    }

    LOGCATI("leave %s", __func__)
}

void text_control::ConfigTextInfo(const char *ttfPath, const char *text, char *outPath,
                                  bool isHorizontal, int spacing,
                                  int lineSpacing, int fontSize, int fontColor, float distanceMark,
                                  float outLineDistanceMark, int outLineColor, float shadowDistance,
                                  float shadowAlpha,
                                  int shadowColor, int shadowAngle) {
    LOGCATI("enter %s", __func__)

    if (current_text_ == nullptr) {
        current_text_ = new TextInfo();
    }
    if (ttfPath != nullptr) {
        /*if (current_text_->ttf_file != nullptr) {
            delete []current_text_->ttf_file;
            current_text_->ttf_file = nullptr;
        }*/
        current_text_->ttf_file = ttfPath;
    }
    if (text != nullptr) {
        /*if (current_text_->text != nullptr) {
            delete []current_text_->text;
            current_text_->text = nullptr;
        }*/
        current_text_->text = text;
    }

    if (outPath != nullptr) {
        if (current_text_->outPath != nullptr) {
            delete[]current_text_->outPath;
            current_text_->outPath = nullptr;
        }
        current_text_->outPath = outPath;
    }

    current_text_->surfaceWidth = 100;
    current_text_->surfaceHeight = 100;
    current_text_->isHorizontal = isHorizontal;
    current_text_->spacing = spacing;
    current_text_->lineSpacing = lineSpacing;
    current_text_->fontSize = fontSize;
    current_text_->fontColor = fontColor;
    current_text_->distanceMark = distanceMark;
    current_text_->outlineDistanceMark = outLineDistanceMark;
    current_text_->outLineColor = outLineColor;
    current_text_->shadowDistance = shadowDistance;
    current_text_->shadowAlpha = shadowAlpha;
    current_text_->shadowColor = shadowColor;
    current_text_->shadowAngle = shadowAngle;

    LOGCATI("leave %s", __func__)
}

void text_control::PostDraw() {
    if (current_text_ == nullptr) return;
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kDRAW;
    message->obj = nullptr;
    PostMessage(message);
}

void text_control::DrawLayer(TextLayer *textLayer) {
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kDRAW;
    message->obj = textLayer;
    PostMessage(message);
}




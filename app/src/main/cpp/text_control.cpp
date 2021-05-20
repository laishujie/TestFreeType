//
// Created by admin on 2021/5/6.
//

#include "util/ImageLoad.h"
#include <shader_manager.h>
#include "text_control.h"


enum RenderMessage {
    kEGLCreate = 0,
    kEGLDestroy,
    kEGLWindowCreate,
    kEGLWindowDestroy,
    kDRAW,
    kPreviewClean
};


text_control::text_control() : Handler(),
                               window_(nullptr),
                               buffer_pool_(nullptr),
                               core_(nullptr), render_surface_(nullptr),
                               surface_height_(0),
                               surface_width_(0), message_queue_(nullptr), message_queue_thread_(),
                               shaderManager_(nullptr),
                               previewLayer(nullptr), previewTemplateLayer(nullptr), layerMaps(),
                               selfIncreasingId(0), javaCallHelper(nullptr) {
    buffer_pool_ = new BufferPool(sizeof(Message));
    message_queue_ = new MessageQueue("text_control Message Queue");
    InitMessageQueue(message_queue_);

    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kEGLCreate;
    PostMessage(message);
    shaderManager_ = new ShaderManager();

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


    if (javaCallHelper != nullptr) {
        delete javaCallHelper;
        javaCallHelper = nullptr;
    }

    if (nullptr != buffer_pool_) {
        delete buffer_pool_;
        buffer_pool_ = nullptr;
    }
    LOGCATI("leave: %s", __func__)
}

int text_control::Init(JavaCallHelper *callHelper) {
    text_control::javaCallHelper = callHelper;
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
            OnGlResourcesDestroy();
            OnGLDestroy();
            break;
        case kEGLWindowDestroy:
            OnGLWindowDestroy();
            break;
        case kDRAW: {
            LOGCATI("enter kDRAW %s", __func__)
            //清理数据
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //绘制层
            if (!layerMaps.empty()) {
                for (auto &textInfo : layerMaps) {
                    TextLayer *pLayer = textInfo.second;
                    shaderManager_->DrawTextLayer(pLayer);
                }
            }

            //绘制内置临时预览层数据
            if (previewLayer != nullptr) {
                shaderManager_->DrawTextLayer(previewLayer);
                //获取信息
                if (javaCallHelper != nullptr) {

                    javaCallHelper->onTextAreaChanged(previewLayer->textArea.left,
                                                      previewLayer->textArea.top,
                                                      previewLayer->textArea.right,
                                                      previewLayer->textArea.bottom);
                }
            }

            if (!core_->SwapBuffers(render_surface_)) {
                LOGCATE("eglSwapBuffers error: %d", eglGetError())
            }

            LOGCATI(" leave kDRAW %s", __func__)
        }
            break;
        case kPreviewClean: {
            delete previewLayer;
            previewLayer = nullptr;
            Display();
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

    if (!core_->SwapBuffers(render_surface_)) {
        LOGCATE("eglSwapBuffers error: %d", eglGetError())
    }
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

void text_control::OnGlResourcesDestroy() {
    LOGCATI("enter %s", __func__)

    if (previewLayer != nullptr) {
        delete previewLayer;
        previewLayer = nullptr;
    }

    for (auto &textInfo : layerMaps) {
        delete textInfo.second;
        textInfo.second = nullptr;
    }

    layerMaps.clear();

    /*pthread_mutex_lock(&queue_mutex_);
     for (auto &textInfo : text_layers_) {
         delete textInfo.second;
         textInfo.second = nullptr;
     }
     text_layers_.clear();
     pthread_mutex_unlock(&queue_mutex_);*/

    if (shaderManager_ != nullptr) {
        delete shaderManager_;
        shaderManager_ = nullptr;
    }

    LOGCATI("leave %s", __func__)
}

void text_control::UpdatePreViewTextInfo(const char *ttfPath, const char *text,
                                         bool isHorizontal, int spacing,
                                         int lineSpacing, int fontSize, int fontColor,
                                         float distanceMark,
                                         float outLineDistanceMark, int outLineColor,
                                         float shadowDistance,
                                         float shadowAlpha,
                                         int shadowColor, int shadowAngle) {
    LOGCATI("enter %s", __func__)
    TextInfo *current_text_;
    //重置层信息
    RestoreTmpLayer(false);
    //TODO 有风险
    current_text_ = previewLayer->text_deque[0];

    if (ttfPath != nullptr) {
        current_text_->ttf_file = ttfPath;
    }
    if (text != nullptr) {
        current_text_->text = text;
    }

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

int text_control::UpdatePreViewByJson(const char *cLayerJson, const char *cFontFolder) {
    std::string layerJson(cLayerJson);
    std::string fontFolder(cFontFolder);

    char *config_buffer = nullptr;

    int ret = ReadFile(layerJson, &config_buffer);
    if (ret != 0 || config_buffer == nullptr) {
        LOGCATE("read info sticker config error: %d", ret);
        return -2;
    }

    cJSON *pJson = cJSON_Parse(config_buffer);
    delete config_buffer;

    if (nullptr == pJson) {
        LOGCATE("parse fail: %s", cJSON_GetErrorPtr())
        return -3;
    }

    cJSON *layers = cJSON_GetObjectItem(pJson, "ts");

    if (nullptr != layers) {
        //重置预览的层信息
        RestoreTmpLayer(true);

        int filter_size = cJSON_GetArraySize(layers);
        for (int i = 0; i < filter_size; i++) {
            cJSON *filter_child = cJSON_GetArrayItem(layers, i);
            cJSON *font_json = cJSON_GetObjectItem(filter_child, "font");
            cJSON *size_id_json = cJSON_GetObjectItem(filter_child, "size");
            cJSON *offset_x_json = cJSON_GetObjectItem(filter_child, "offset_x");
            cJSON *offset_y_json = cJSON_GetObjectItem(filter_child, "offset_y");
            cJSON *font_color_json = cJSON_GetObjectItem(filter_child, "color");

            char *ptr;
            auto *textInfo = new TextInfo();
            textInfo->ttf_file = fontFolder + "/" + font_json->valuestring;
            textInfo->fontSize = strtol(size_id_json->valuestring, &ptr, 10);
            textInfo->isFromTemplate = true;
            textInfo->offset_x = (float) strtol(offset_x_json->valuestring, &ptr, 10);
            textInfo->offset_y = (float) strtol(offset_y_json->valuestring, &ptr, 10);

            // unsigned long i1 = strtoul(font_color_json->valuestring, &ptr, 16);
            textInfo->fontColor = (int) strtoul(font_color_json->valuestring, &ptr, 16);


            cJSON *text_child = cJSON_GetObjectItem(filter_child, "wenan");
            if (text_child != nullptr) {
                cJSON *text_ = cJSON_GetArrayItem(text_child, 0);
                textInfo->text = text_->valuestring;
            }


            previewLayer->text_deque.push_back(textInfo);
        }

        cJSON_Delete(pJson);
    }
    return 0;
}

void text_control::Display() {
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kDRAW;
    PostMessage(message);
}

int text_control::AddThePreviewLayer2Map() {
    if (previewLayer == nullptr || previewLayer->textureId == 0 ||
        previewLayer->text_deque.empty()) {
        return -1;
    }
    TextInfo *&pInfo = previewLayer->text_deque[0];
    int layerID = AddTextLayer(pInfo->ttf_file.c_str(), pInfo->text.c_str(), pInfo->isHorizontal,
                               pInfo->spacing,
                               pInfo->lineSpacing, pInfo->fontSize,
                               pInfo->fontColor, pInfo->distanceMark, pInfo->outlineDistanceMark,
                               pInfo->outLineColor, pInfo->shadowDistance, pInfo->shadowAlpha,
                               pInfo->shadowColor,
                               pInfo->shadowAngle);

    layerMaps[layerID]->textureId = previewLayer->textureId;
    layerMaps[layerID]->frameBuffer = previewLayer->frameBuffer;

    previewLayer->frameBuffer = 0;
    previewLayer->textureId = 0;

    return layerID;
}

int text_control::AddTextLayerByJson(const char *cLayerJson, const char *cFontFolder) {
    std::string layerJson(cLayerJson);
    std::string fontFolder(cFontFolder);


    char *config_buffer = nullptr;

    int ret = ReadFile(layerJson, &config_buffer);
    if (ret != 0 || config_buffer == nullptr) {
        LOGCATE("read info sticker config error: %d", ret);
        return -2;
    }

    cJSON *pJson = cJSON_Parse(config_buffer);
    delete config_buffer;

    if (nullptr == pJson) {
        LOGCATE("parse fail: %s", cJSON_GetErrorPtr())
        return -3;
    }


    cJSON *layers = cJSON_GetObjectItem(pJson, "ts");

    if (nullptr != layers) {
        selfIncreasingId++;
        auto *textLayer = new TextLayer();
        textLayer->id = selfIncreasingId;

        int filter_size = cJSON_GetArraySize(layers);
        for (int i = 0; i < filter_size; i++) {
            cJSON *filter_child = cJSON_GetArrayItem(layers, i);
            cJSON *font_json = cJSON_GetObjectItem(filter_child, "font");
            cJSON *size_id_json = cJSON_GetObjectItem(filter_child, "size");
            cJSON *offset_x_json = cJSON_GetObjectItem(filter_child, "offset_x");
            cJSON *offset_y_json = cJSON_GetObjectItem(filter_child, "offset_y");

            char *ptr;
            auto *textInfo = new TextInfo();
            textInfo->ttf_file = fontFolder + "/" + font_json->valuestring;
            textInfo->fontSize = strtol(size_id_json->valuestring, &ptr, 10);
            textInfo->isFromTemplate = true;
            textInfo->offset_x = strtol(offset_x_json->valuestring, &ptr, 10);
            textInfo->offset_y = strtol(offset_y_json->valuestring, &ptr, 10);

            cJSON *text_child = cJSON_GetObjectItem(filter_child, "wenan");
            if (text_child != nullptr) {
                cJSON *text_ = cJSON_GetArrayItem(text_child, 0);
                textInfo->text = text_->valuestring;
            }


            textLayer->text_deque.push_back(textInfo);
        }

        std::pair<int, TextLayer *> stl = {selfIncreasingId, textLayer};

        layerMaps.insert(stl);

        cJSON_Delete(pJson);
        return selfIncreasingId;
    }
    return -1;
}

int text_control::ReadFile(const std::string &path, char **buffer) {
    FILE *file = fopen(path.c_str(), "r");
    printf("path: %s\n", path.c_str());
    if (file == nullptr) {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    char *data = new char[sizeof(char) * file_size + 1];
    /* if (nullptr == data) {
         fclose(file);
         return -2;
     }*/
    memset(data, 0, sizeof(char) * file_size);
    data[file_size] = '\0';
    size_t read_size = fread(data, 1, file_size, file);
    if (read_size != file_size) {
        fclose(file);
        delete[] data;
        return -3;
    }
    fclose(file);
    printf("%s\n", data);
    *buffer = data;
    return 0;
}

int
text_control::AddTextLayer(const char *ttfPath, const char *text, bool isHorizontal, int spacing,
                           int lineSpacing, int fontSize, int fontColor, float distanceMark,
                           float outLineDistanceMark, int outLineColor, float shadowDistance,
                           float shadowAlpha, int shadowColor, int shadowAngle) {
    selfIncreasingId++;
    auto *textLayer = new TextLayer();
    textLayer->id = selfIncreasingId;
    auto *textInfo = new TextInfo();
    textInfo->ttf_file = ttfPath;
    textInfo->text = text;
    textInfo->isHorizontal = isHorizontal;
    textInfo->spacing = spacing;
    textInfo->lineSpacing = lineSpacing;
    textInfo->fontSize = fontSize;
    textInfo->fontColor = fontColor;
    textInfo->distanceMark = distanceMark;
    textInfo->outlineDistanceMark = outLineDistanceMark;
    textInfo->outLineColor = outLineColor;
    textInfo->shadowDistance = shadowDistance;
    textInfo->shadowAlpha = shadowAlpha;
    textInfo->shadowColor = shadowColor;
    textInfo->shadowAngle = shadowAngle;

    //层添加关系
    textLayer->text_deque.push_back(textInfo);

    //添加对应关系
    std::pair<int, TextLayer *> stl = {selfIncreasingId, textLayer};
    layerMaps.insert(stl);
    return selfIncreasingId;
}

int text_control::AddThePreviewLayer2MapByJson() {
    if (previewLayer == nullptr || previewLayer->textureId == 0 ||
        previewLayer->text_deque.empty()) {
        return -1;
    }

    selfIncreasingId++;

    auto *textLayer = new TextLayer();
    textLayer->id = selfIncreasingId;
    textLayer->textureId = previewLayer->textureId;
    textLayer->frameBuffer = previewLayer->frameBuffer;
    textLayer->text_deque.swap(previewLayer->text_deque);


    std::pair<int, TextLayer *> stl = {selfIncreasingId, textLayer};
    layerMaps.insert(stl);
    previewLayer->id = -1;
    previewLayer->frameBuffer = 0;
    previewLayer->textureId = 0;
    /* for (auto text:previewLayer->text_deque) {
         delete text;
     }*/
    //previewLayer->text_deque.clear();

    return selfIncreasingId;
}


int text_control::RestoreTmpLayer(bool isFromTemplate) {
    if (isFromTemplate) {
        if (previewLayer == nullptr) {
            previewLayer = new TextLayer();
        } else {
            if (!previewLayer->text_deque.empty()) {
                for (auto textInfo:previewLayer->text_deque) {
                    delete textInfo;
                }
                previewLayer->text_deque.clear();
            }
        }
        previewLayer->isTemplate = true;
    } else {
        if (previewLayer == nullptr) {
            previewLayer = new TextLayer();
            previewLayer->text_deque.push_back(new TextInfo());
        } else {
            //如果上次是模板预览
            if (previewLayer->isTemplate) {
                //清空上次选择的数据
                if (!previewLayer->text_deque.empty()) {
                    for (auto textInfo:previewLayer->text_deque) {
                        delete textInfo;
                    }
                }
                //清空
                previewLayer->text_deque.clear();
                //添加模板信息
                previewLayer->text_deque.push_back(new TextInfo());
            }
            previewLayer->isTemplate = false;
        }
    }
    return 0;
}

void text_control::CleanPreview() {
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kPreviewClean;
    PostMessage(message);
}






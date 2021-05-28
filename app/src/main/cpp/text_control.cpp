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
    kPreviewClean,
    kAddTextLayer,
    kRemoveTextLayer
};

text_control::text_control() : Handler(),
                               window_(nullptr),
                               buffer_pool_(nullptr),
                               core_(nullptr), render_surface_(nullptr),
                               surface_height_(0),
                               surface_width_(0), message_queue_(nullptr), message_queue_thread_(),
                               shaderManager_(nullptr),
                               previewLayer(nullptr), previewTemplateLayer(nullptr), layerMaps(),
                               LayerSelfIdIncreasing(0), javaCallHelper(nullptr) {
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
                    TextLayer *&pLayer = textInfo.second;

                    shaderManager_->DrawTextLayer(pLayer);

                    /*if (pLayer->isFristCreate) {
                        //TODO 先文字一层看看效果
                        javaCallHelper->onTextLevelChange(true, pLayer->id,
                                                          pLayer->text_deque[0]->id);
                        pLayer->isFristCreate = false;
                    }*/
                    if (javaCallHelper != nullptr && pLayer->isChangeTextArea) {
                        javaCallHelper->onTextAreaChanged(pLayer->id, pLayer->textArea.left,
                                                          pLayer->textArea.top,
                                                          pLayer->textArea.right,
                                                          pLayer->textArea.bottom);
                        pLayer->isChangeTextArea = false;
                    }
                }
            }

            //绘制内置临时预览层数据
            /* if (previewLayer != nullptr) {
                 shaderManager_->DrawTextLayer(previewLayer);

                 //获取信息
                 if (javaCallHelper != nullptr && previewLayer->isChangeTextArea) {
                     javaCallHelper->onTextAreaChanged(previewLayer->id, previewLayer->textArea.left,
                                                       previewLayer->textArea.top,
                                                       previewLayer->textArea.right,
                                                       previewLayer->textArea.bottom);
                 }
             }*/

            if (!core_->SwapBuffers(render_surface_)) {
                LOGCATE("eglSwapBuffers error: %d", eglGetError())
            }


            LOGCATI(" leave kDRAW %s", __func__)
        }
            break;
        case kPreviewClean: {
            delete previewLayer;
            previewLayer = nullptr;
            PostDisplay();
        }
            break;
        case kAddTextLayer: {
            auto &textLayer = reinterpret_cast<TextLayer *&>(obj);
            AddTextLayer(textLayer);
        }
            break;
        case kRemoveTextLayer: {
            int layerId = msg->arg1;
            RemoveLayer(layerId);
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
    //重置层信息
    RestoreTmpLayer(false);
    //TODO 有风险
    TextInfo *&current_text_ = previewLayer->text_deque[0];

    if (current_text_ == nullptr) return;

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
        cJSON_Delete(pJson);
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

void text_control::PostDisplay() {
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kDRAW;
    message->obj = nullptr;
    PostMessage(message);
}

int text_control::AddThePreviewLayer2Map() {
    if (previewLayer == nullptr || previewLayer->textureId == 0 ||
        previewLayer->text_deque.empty()) {
        return -1;
    }
    /*TextInfo *&pInfo = previewLayer->text_deque[0];
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
    previewLayer->textureId = 0;*/

    LayerSelfIdIncreasing++;

    //直接把预览的指针赋值,地址添加到集合
    auto *textLayer = previewLayer;
    textLayer->id = LayerSelfIdIncreasing;
    std::pair<int, TextLayer *> stl = {LayerSelfIdIncreasing, textLayer};
    layerMaps.insert(stl);

    //置空，下次自动赋值
    previewLayer = nullptr;


    return LayerSelfIdIncreasing;
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

    delete config_buffer;
    cJSON *pJson = cJSON_Parse(config_buffer);

    if (nullptr == pJson) {
        LOGCATE("parse fail: %s", cJSON_GetErrorPtr())
        return -3;
    }


    cJSON *layers = cJSON_GetObjectItem(pJson, "ts");

    if (nullptr != layers) {
        LayerSelfIdIncreasing++;
        auto *textLayer = new TextLayer();
        textLayer->id = LayerSelfIdIncreasing;

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

        std::pair<int, TextLayer *> stl = {LayerSelfIdIncreasing, textLayer};

        layerMaps.insert(stl);

        cJSON_Delete(pJson);
        return LayerSelfIdIncreasing;
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
    LayerSelfIdIncreasing++;
    auto *textLayer = new TextLayer();
    textLayer->id = LayerSelfIdIncreasing;
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
    std::pair<int, TextLayer *> stl = {LayerSelfIdIncreasing, textLayer};
    layerMaps.insert(stl);
    return LayerSelfIdIncreasing;
}

int text_control::AddThePreviewLayer2MapByJson() {
    if (previewLayer == nullptr || previewLayer->textureId == 0 ||
        previewLayer->text_deque.empty()) {
        return -1;
    }

    LayerSelfIdIncreasing++;

    auto *textLayer = new TextLayer();
    textLayer->id = LayerSelfIdIncreasing;
    textLayer->textureId = previewLayer->textureId;
    textLayer->frameBuffer = previewLayer->frameBuffer;
    textLayer->text_deque.swap(previewLayer->text_deque);


    std::pair<int, TextLayer *> stl = {LayerSelfIdIncreasing, textLayer};
    layerMaps.insert(stl);
    previewLayer->id = -1;
    previewLayer->frameBuffer = 0;
    previewLayer->textureId = 0;
    /* for (auto text:previewLayer->text_deque) {
         delete text;
     }*/
    //previewLayer->text_deque.clear();

    return LayerSelfIdIncreasing;
}


int text_control::RestoreTmpLayer(bool isFromTemplate) {
    if (isFromTemplate) {
        if (previewLayer == nullptr) {
            previewLayer = new TextLayer();
            previewLayer->id = PREVIEW_ID;
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
            previewLayer->id = PREVIEW_ID;
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

void text_control::TextLayerTransform(int layerId, float tx, float ty, float sc, float r) {

    const std::map<int, TextLayer *>::iterator &iterator = layerMaps.find(layerId);

    if (iterator != layerMaps.end()) {
        TextLayer *pLayer = iterator->second;
        pLayer->applyMatrix = true;
        pLayer->tx = tx;
        pLayer->ty = ty;
        pLayer->sc = sc;
        pLayer->r = r;
        PostDisplay();
    }
}


int
text_control::AddSimpleSubtext(int layerId, int subTextId, const char *ttfPath, const char *text,
                               int fonSize,
                               int fontColor) {

    auto *textLayer = new TextLayer();
    textLayer->isFristCreate = true;
    textLayer->id = layerId;
    auto *textInfo = new TextInfo();
    textInfo->id = subTextId;
    textInfo->ttf_file = ttfPath;
    textInfo->text = text;
    textInfo->fontSize = fonSize;
    textInfo->fontColor = fontColor;

    //层添加关系
    textLayer->text_deque.push_back(textInfo);

    //添加对应关系
    std::pair<int, TextLayer *> stl = {layerId, textLayer};
    layerMaps.insert(stl);

    PostDisplay();
    return 0;
}


int text_control::RemoveLayer(int layerId) {
    const std::map<int, TextLayer *>::iterator &iterator = layerMaps.find(layerId);

    if (iterator != layerMaps.end()) {
        // 释放指针
        delete iterator->second;
        iterator->second = nullptr;
        layerMaps.erase(iterator);

        PostDisplay();

        /* if (javaCallHelper != nullptr) {
             javaCallHelper->onTextLevelChange(false, layerId, 0, 1);
         }*/
    } else {
        return -1;
    }
    return 0;
}


void
text_control::SetBasicTextAttributes(int layerId, int subId, const char *text, const char *ttfPath,
                                     int fonSize, int fontColor) {
    TextInfo *pInfo = nullptr;
    TextLayer *pLayer = nullptr;

    int i = findTextInfo(layerId, subId, pLayer, pInfo);
    if (i == 0) {
        pLayer->isDraw = true;
        pInfo->text = text;
        pInfo->ttf_file = ttfPath;
        pInfo->fontSize = fonSize;
        pInfo->fontColor = fontColor;

        PostDisplay();
    }
}

int text_control::FindTextLayer(int layerId, TextLayer *&textLayer) {
    const std::map<int, TextLayer *>::iterator &iterator = layerMaps.find(layerId);
    if (iterator != layerMaps.end()) {
        TextLayer *pLayer = iterator->second;
        if (pLayer->id == layerId) {
            textLayer = pLayer;
            return 0;
        }
    }
    return -1;
}


int
text_control::findTextInfo(int layerId, int subId, TextLayer *&textLayer, TextInfo *&pTextInfo) {
    int i = FindTextLayer(layerId, textLayer);
    if (i == 0) {
        std::deque<TextInfo *> &deque = textLayer->text_deque;
        for (auto &pInfo : deque) {
            if (pInfo->id == subId) {
                pTextInfo = pInfo;
                return 0;
            }
        }
    }

    return -1;
}

void text_control::printAll() {
    const std::map<int, TextLayer *>::iterator &iterator = layerMaps.begin();
    for (auto &it : layerMaps) {
        for (auto textInfo:it.second->text_deque) {
            LOGCATE("JNI layerId %d subId %d text %s", it.first, textInfo->id,
                    textInfo->text.c_str())
        }
    }
}

void split(const std::string &s, std::vector<std::string> &tokens, const std::string &delimiters = " ") {
    std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    std::string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (lastPos != std::string::npos || pos != std::string::npos) {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
}

int text_control::FillFrame(std::string &templateFolder, TextInfo *&info) {
    if (info->isTextImage && !info->file.empty()) {
        std::vector<std::string> str;
        split(info->file, str, ",");

        for (auto &filePath:str) {
            char *config_buffer = nullptr;

            int ret = ReadFile(templateFolder.append("/") + filePath + ".json", &config_buffer);
            if (ret != 0 || config_buffer == nullptr) {
                LOGCATE("读取背景序列帧图片失败 error: %d", ret)
                return -2;
            }
            cJSON *pJson = cJSON_Parse(config_buffer);

            if (nullptr == pJson) {
                cJSON_Delete(pJson);
                LOGCATE("parse fail: %s", cJSON_GetErrorPtr())
                continue;
            }

            cJSON *frames = cJSON_GetObjectItem(pJson, "frames");
            cJSON *meta = cJSON_GetObjectItem(pJson, "meta");
            cJSON *image_id_json = cJSON_GetObjectItem(meta, "image");
            TextImage textImage;
            textImage.frameImg = image_id_json->string;

            if (nullptr != frames) {
                int frames_size = cJSON_GetArraySize(frames);

                for (int i = 0; i < frames_size; i++) {

                    cJSON *filter_child = cJSON_GetArrayItem(frames, i);

                    cJSON *text_child = cJSON_GetObjectItem(filter_child, "frame");
                    cJSON *x_id_json = cJSON_GetObjectItem(text_child, "x");
                    cJSON *y_id_json = cJSON_GetObjectItem(text_child, "y");
                    cJSON *w_id_json = cJSON_GetObjectItem(text_child, "w");
                    cJSON *h_id_json = cJSON_GetObjectItem(text_child, "h");

                    textImage.frameCoordinates.push_back(
                            {x_id_json->valueint, y_id_json->valueint, w_id_json->valueint,
                             h_id_json->valueint});

                    info->textImages.push_back(textImage);
                }
            }

            cJSON_Delete(pJson);
        }
    }
    return 0;
}

int text_control::AddTextLayer(TextLayer *&textLayer) {
    std::pair<int, TextLayer *> stl = {textLayer->id, textLayer};

    for (auto &textInfo:textLayer->text_deque) {
        FillFrame(textLayer->templateFolder, textInfo);
    }

    layerMaps.insert(stl);

    return 0;
}

void text_control::setStrokeAttributes(int layerId, int subTextId, float lineDistance,
                                       float outLineDistance, int outLineColor) {
    TextInfo *pInfo = nullptr;
    TextLayer *pLayer = nullptr;

    int i = findTextInfo(layerId, subTextId, pLayer, pInfo);
    if (i == 0) {
        pLayer->isDraw = true;
        pInfo->distanceMark = lineDistance;
        pInfo->outLineColor = outLineColor;
        pInfo->outlineDistanceMark = outLineDistance;
        PostDisplay();
    }
}

void text_control::PostAddTextLayer(TextLayer *&textLayer) {
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kAddTextLayer;
    message->obj = textLayer;
    PostMessage(message);
}

void text_control::PostRemoveLayer(int layerId) {
    auto message = buffer_pool_->GetBuffer<Message>();
    message->what = kRemoveTextLayer;
    message->arg1 = layerId;
    PostMessage(message);
}








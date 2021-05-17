//
// Created by admin on 2021/5/6.
//

#include "text_engine.h"

int text_engine::Init() {
    int result = pthread_mutex_init(&queue_mutex_, nullptr);
    if (result != 0) {
        LOGCATE("init text queue mutex error");
        return result;
    }
    result = pthread_mutex_init(&text_mutex_, nullptr);
    if (result != 0) {
        LOGCATE("init text text_mutex_ error");
        return result;
    }
    return 0;
}

void text_engine::OnSurfaceCreated(jobject surface, int width, int height) {
    if (nullptr != window_) {
        ANativeWindow_release(window_);
        window_ = nullptr;
    }

    JNIEnv *env = nullptr;
    if (vm_ != nullptr &&
        (vm_)->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

    window_ = ANativeWindow_fromSurface(env, surface);

    if (nullptr != player_) {

        player_->OnSurfaceCreated(window_, width, height);
    }
}

void text_engine::OnSurfaceChanged(int width, int height) {
    if (nullptr != player_) {
        player_->OnSurfaceChanged(width, height);
    }
}

void text_engine::OnSurfaceDestroyed() {
    if (nullptr != player_) {
        player_->OnSurfaceDestroy();
    }
    if (nullptr != window_) {
        ANativeWindow_release(window_);
        window_ = nullptr;
    }

}

text_engine::text_engine(JNIEnv *env) : window_(nullptr), vm_(nullptr), queue_mutex_(),
                                        text_mutex_(), selfIncreasingId(0) {
    env->GetJavaVM(&vm_);
    player_ = new text_control();
}

text_engine::~text_engine() {
    LOGCATI("enter %s", __func__)

    if (nullptr != player_) {
        delete player_;
        player_ = nullptr;
    }
    if (window_ != nullptr) {
        ANativeWindow_release(window_);
        window_ = nullptr;
    }

    pthread_mutex_lock(&queue_mutex_);
    for (auto &textInfo : text_layers_) {
        delete textInfo.second;
        textInfo.second = nullptr;
    }
    text_layers_.clear();
    pthread_mutex_unlock(&queue_mutex_);
    pthread_mutex_destroy(&queue_mutex_);

    pthread_mutex_destroy(&text_mutex_);


    LOGCATI("leave %s", __func__)
    // JNIEnv *env = nullptr;
//    if ((vm_)->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK) {
//        env->DeleteGlobalRef(video_editor_object_);
//    }
}

void
text_engine::DrawPreView(const char *ttfPath, const char *text, char *outPath, bool isHorizontal,
                         int spacing, int lineSpacing,
                         int fontSize, int fontColor, float distanceMark,
                         float outLineDistanceMark,
                         int outlineColor,
                         float shadowDistance, float shadowAlpha, int shadowColor,
                         int shadowAngle) {

    //避免出现竞争
    pthread_mutex_lock(&queue_mutex_);
    player_->ConfigTextInfo(ttfPath, text, nullptr, isHorizontal, spacing, lineSpacing, fontSize,
                            fontColor,
                            distanceMark, outLineDistanceMark, outlineColor, shadowDistance,
                            shadowAlpha, shadowColor, shadowAngle);
    player_->DrawPreView();
    pthread_mutex_unlock(&queue_mutex_);
}

int text_engine::AddTextLayer(const char *c_layerJson, const char *c_fontFolder) {

    if (c_layerJson == nullptr || c_fontFolder == nullptr) return -1;


    pthread_mutex_lock(&text_mutex_);

    std::string layerJson(c_layerJson);
    std::string fontFolder(c_fontFolder);


    char *config_buffer = nullptr;

    int ret = ReadFile(layerJson, &config_buffer);
    if (ret != 0 || config_buffer == nullptr) {
        LOGCATE("read info sticker config error: %d", ret);
        pthread_mutex_unlock(&text_mutex_);
        return -2;
    }

    cJSON *pJson = cJSON_Parse(config_buffer);
    delete config_buffer;

    if (nullptr == pJson) {
        LOGCATE("parse fail: %s", cJSON_GetErrorPtr())
        pthread_mutex_unlock(&text_mutex_);
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

        text_layers_.insert(stl);

        player_->DrawLayer(text_layers_.at(selfIncreasingId));

        cJSON_Delete(pJson);

        pthread_mutex_unlock(&text_mutex_);

        return selfIncreasingId;
    }
    pthread_mutex_unlock(&text_mutex_);


    return -1;
}

int text_engine::ReadFile(const std::string &path, char **buffer) {
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

int text_engine::AddTextLayer(const char *ttfPath, const char *text, char *outPath,
                              bool isHorizontal, int spacing,
                              int lineSpacing, int fontSize, int fontColor, float distanceMark,
                              float outLineDistanceMark, int outLineColor, float shadowDistance,
                              float shadowAlpha,
                              int shadowColor, int shadowAngle) {
    pthread_mutex_lock(&text_mutex_);
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
    text_layers_.insert(stl);


    player_->DrawLayer(text_layers_.at(selfIncreasingId));


    pthread_mutex_unlock(&text_mutex_);


    return selfIncreasingId;
}

int text_engine::UpdateTextInfo(int layerId, const char *ttfPath, const char *text, char *outPath,
                                bool isHorizontal, int spacing, int lineSpacing, int fontSize,
                                int fontColor, float distanceMark, float outLineDistanceMark,
                                int outLineColor, float shadowDistance, float shadowAlpha,
                                int shadowColor, int shadowAngle) {
    pthread_mutex_lock(&queue_mutex_);
    TextLayer *&pLayer = text_layers_.at(layerId);

    if (!pLayer->text_deque.empty()) {
        TextInfo *&textInfo = pLayer->text_deque[0];
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

        player_->DrawLayer(pLayer);
    }

    pthread_mutex_unlock(&queue_mutex_);

    return 0;
}

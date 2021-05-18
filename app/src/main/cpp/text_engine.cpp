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
                                        text_mutex_() {
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

    /*pthread_mutex_lock(&queue_mutex_);
    for (auto &textInfo : text_layers_) {
        delete textInfo.second;
        textInfo.second = nullptr;
    }
    text_layers_.clear();
    pthread_mutex_unlock(&queue_mutex_);*/
    pthread_mutex_destroy(&queue_mutex_);

    pthread_mutex_destroy(&text_mutex_);


    LOGCATI("leave %s", __func__)
    // JNIEnv *env = nullptr;
//    if ((vm_)->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK) {
//        env->DeleteGlobalRef(video_editor_object_);
//    }
}

void
text_engine::DrawPreView(const char *ttfPath, const char *text, bool isHorizontal,
                         int spacing, int lineSpacing,
                         int fontSize, int fontColor, float distanceMark,
                         float outLineDistanceMark,
                         int outlineColor,
                         float shadowDistance, float shadowAlpha, int shadowColor,
                         int shadowAngle) {
    //避免出现竞争
    pthread_mutex_lock(&queue_mutex_);
    player_->UpdatePreViewTextInfo(ttfPath, text, isHorizontal, spacing, lineSpacing, fontSize,
                                   fontColor,
                                   distanceMark, outLineDistanceMark, outlineColor, shadowDistance,
                                   shadowAlpha, shadowColor, shadowAngle);
    player_->Display();
    pthread_mutex_unlock(&queue_mutex_);
}

int text_engine::AddTextLayer(const char *c_layerJson, const char *c_fontFolder) {

    if (c_layerJson == nullptr || c_fontFolder == nullptr) return -1;

    pthread_mutex_lock(&text_mutex_);
    int id = player_->AddTextLayerByJson(c_layerJson, c_fontFolder);
    pthread_mutex_unlock(&text_mutex_);

    return id;
}


int text_engine::AddTextLayer(const char *ttfPath, const char *text,
                              bool isHorizontal, int spacing,
                              int lineSpacing, int fontSize, int fontColor, float distanceMark,
                              float outLineDistanceMark, int outLineColor, float shadowDistance,
                              float shadowAlpha,
                              int shadowColor, int shadowAngle) {

    assert(ttfPath);
    assert(text);
    assert(fontSize);

    pthread_mutex_lock(&text_mutex_);

    int layerId = player_->AddTextLayer(ttfPath, text, isHorizontal, spacing, lineSpacing, fontSize,
                                        fontColor,
                                        distanceMark, outLineDistanceMark, outLineColor,
                                        shadowDistance,
                                        shadowAlpha, shadowColor, shadowAngle);
    pthread_mutex_unlock(&text_mutex_);


    return layerId;
}

int text_engine::UpdateTextInfo(int layerId, const char *ttfPath, const char *text, char *outPath,
                                bool isHorizontal, int spacing, int lineSpacing, int fontSize,
                                int fontColor, float distanceMark, float outLineDistanceMark,
                                int outLineColor, float shadowDistance, float shadowAlpha,
                                int shadowColor, int shadowAngle) {
    /*  pthread_mutex_lock(&queue_mutex_);
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

      pthread_mutex_unlock(&queue_mutex_);*/

    return 0;
}

void text_engine::Display() {
    player_->Display();
}

int text_engine::AddThePreviewLayer2Map() {
    pthread_mutex_lock(&text_mutex_);
    int layerId = player_->AddThePreviewLayer2Map();
    pthread_mutex_unlock(&text_mutex_);
    return layerId;
}

void text_engine::DrawPreViewByJson(const char *layerJson, const char *fontFolder) {
    pthread_mutex_lock(&queue_mutex_);
    player_->UpdatePreViewByJson(layerJson, fontFolder);
    player_->Display();
    pthread_mutex_unlock(&queue_mutex_);
}

int text_engine::AddThePreviewLayer2MapByJson() {
    pthread_mutex_lock(&text_mutex_);
    int layerId = player_->AddThePreviewLayer2MapByJson();
    pthread_mutex_unlock(&text_mutex_);
    return layerId;
}

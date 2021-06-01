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
        (vm_)->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK && surface ==
                                                                                     nullptr) {
        LOGCATE("text_engine::OnSurfaceCreated 失败")
        return;
    }

    window_ = ANativeWindow_fromSurface(env, surface);

    if (nullptr != textControl) {
        textControl->OnSurfaceCreated(window_, width, height);
    }
}

void text_engine::OnSurfaceChanged(int width, int height) {
    if (nullptr != textControl) {
        textControl->OnSurfaceChanged(width, height);
    }
}

void text_engine::OnSurfaceDestroyed() {
    if (nullptr != textControl) {
        textControl->OnSurfaceDestroy();
    }
    if (nullptr != window_) {
        ANativeWindow_release(window_);
        window_ = nullptr;
    }

}

text_engine::text_engine(JNIEnv *env, JavaCallHelper *javaCallHelper) : window_(nullptr),
                                                                        vm_(nullptr),
                                                                        queue_mutex_(),
                                                                        text_mutex_() {
    env->GetJavaVM(&vm_);
    textControl = new text_control();
    textControl->Init(javaCallHelper);
}

text_engine::~text_engine() {
    LOGCATI("enter %s", __func__)

    if (nullptr != textControl) {
        delete textControl;
        textControl = nullptr;
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
    LOGCATI("enter %s", __func__)
    //避免出现竞争
    pthread_mutex_lock(&queue_mutex_);
    textControl->UpdatePreViewTextInfo(ttfPath, text, isHorizontal, spacing, lineSpacing, fontSize,
                                       fontColor,
                                       distanceMark, outLineDistanceMark, outlineColor,
                                       shadowDistance,
                                       shadowAlpha, shadowColor, shadowAngle);
    pthread_mutex_unlock(&queue_mutex_);
    textControl->PostDisplay();
    LOGCATI("leave %s", __func__)
}

int text_engine::AddTextLayer(const char *c_layerJson, const char *c_fontFolder) {

    if (c_layerJson == nullptr || c_fontFolder == nullptr) return -1;

    pthread_mutex_lock(&text_mutex_);
    int id = textControl->AddTextLayerByJson(c_layerJson, c_fontFolder);
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

    int layerId = textControl->AddTextLayer(ttfPath, text, isHorizontal, spacing, lineSpacing,
                                            fontSize,
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

          textControl->DrawLayer(pLayer);
      }

      pthread_mutex_unlock(&queue_mutex_);*/

    return 0;
}

void text_engine::Display() {
    textControl->PostDisplay();
}

int text_engine::AddThePreviewLayer2Map() {
    pthread_mutex_lock(&text_mutex_);
    int layerId = textControl->AddThePreviewLayer2Map();
    pthread_mutex_unlock(&text_mutex_);
    return layerId;
}

void text_engine::DrawPreViewByJson(const char *layerJson, const char *fontFolder) {
    pthread_mutex_lock(&queue_mutex_);
    textControl->UpdatePreViewByJson(layerJson, fontFolder);
    textControl->PostDisplay();
    pthread_mutex_unlock(&queue_mutex_);
}

int text_engine::AddThePreviewLayer2MapByJson() {
    pthread_mutex_lock(&text_mutex_);
    int layerId = textControl->AddThePreviewLayer2MapByJson();
    pthread_mutex_unlock(&text_mutex_);
    return layerId;
}

void text_engine::CleanPreview() {
    pthread_mutex_lock(&queue_mutex_);
    textControl->CleanPreview();
    pthread_mutex_unlock(&queue_mutex_);
}

void text_engine::TextLayerTransform(int layerId, float tx, float ty, float canterX, float canterY,float sc, float r) {
    pthread_mutex_lock(&queue_mutex_);
    textControl->TextLayerTransform(layerId, tx, ty,canterX,  canterY, sc, r);
    pthread_mutex_unlock(&queue_mutex_);
}


int
text_engine::AddSimpleSubtext(int layerId, int subTextId, const char *ttfPath, const char *text,
                              int fonSize,
                              int fontColor) {
    pthread_mutex_lock(&queue_mutex_);
    textControl->AddSimpleSubtext(layerId, subTextId, ttfPath, text, fonSize, fontColor);
    pthread_mutex_unlock(&queue_mutex_);
    return 0;
}

void text_engine::RemoveTextLayer(int layerId) {
    pthread_mutex_lock(&queue_mutex_);
    textControl->PostRemoveLayer(layerId);
    pthread_mutex_unlock(&queue_mutex_);
}

void
text_engine::SetBasicTextAttributes(int layerId, int subId, const char *text, const char *ttfPath,
                                    int fonSize, int fontColor) {
    pthread_mutex_lock(&queue_mutex_);
    textControl->SetBasicTextAttributes(layerId, subId, text, ttfPath, fonSize, fontColor);
    pthread_mutex_unlock(&queue_mutex_);
}

void text_engine::printAll() {
    textControl->printAll();
}

int text_engine::AddTextLayer(TextLayer *&textLayer) {
    pthread_mutex_lock(&queue_mutex_);
    textControl->PostAddTextLayer(textLayer);
    Display();
    pthread_mutex_unlock(&queue_mutex_);
    return 0;
}

void text_engine::SetStrokeAttributes(int layerId, int subTextId, float lineDistance,
                                      float outLineDistance, int outLineColor) {
    pthread_mutex_lock(&queue_mutex_);
    textControl->setStrokeAttributes(layerId, subTextId, lineDistance, outLineDistance,
                                     outLineColor);
    Display();
    pthread_mutex_unlock(&queue_mutex_);
}

void text_engine::UpdateTextLayerFrame(int layerId, int frameIndex) {
    pthread_mutex_lock(&queue_mutex_);
    textControl->PostUpdateTextLayerFrame(layerId, frameIndex);
    pthread_mutex_unlock(&queue_mutex_);
}


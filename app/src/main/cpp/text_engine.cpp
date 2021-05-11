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

text_engine::text_engine(JNIEnv *env) : window_(nullptr), vm_(nullptr), queue_mutex_() {
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
    for (auto clip : text_deque_) {
        delete clip;
    }
    text_deque_.clear();
    pthread_mutex_unlock(&queue_mutex_);
    pthread_mutex_destroy(&queue_mutex_);
    LOGCATI("leave %s", __func__)
    // JNIEnv *env = nullptr;
//    if ((vm_)->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK) {
//        env->DeleteGlobalRef(video_editor_object_);
//    }
}

void text_engine::OnDraw(const char *ttfPath, const char *text, char *outPath, bool isHorizontal,
                         int spacing, jint i,
                         jint i1, jint fontColor, jfloat distanceMark, jfloat outLineDistanceMark,
                         jint outlineColor,
                         jfloat shadowDistance, jfloat shadowAlpha) {


    /* pthread_mutex_lock(&queue_mutex_);
     text_deque_.push_back(clip);
     pthread_mutex_unlock(&queue_mutex_);*/

    //避免出现竞争
    pthread_mutex_lock(&queue_mutex_);
    player_->ConfigTextInfo(ttfPath, text, nullptr, isHorizontal, spacing, i, i1, fontColor,
                            distanceMark, outLineDistanceMark, outlineColor, shadowDistance,
                            shadowAlpha);
    player_->PostDraw();
    pthread_mutex_unlock(&queue_mutex_);
}

//
// Created by admin on 2021/5/6.
//

#include "text_engine.h"

int text_engine::Init() {
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

text_engine::text_engine(JNIEnv *env) : window_(nullptr), vm_(nullptr) {
    env->GetJavaVM(&vm_);
    player_ = new text_control();
}

text_engine::~text_engine() {
    if (nullptr != player_) {
        delete player_;
        player_ = nullptr;
    }
    // JNIEnv *env = nullptr;
//    if ((vm_)->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK) {
//        env->DeleteGlobalRef(video_editor_object_);
//    }

}

void text_engine::OnDraw(char *ttfPath, char *text) {
    player_->Draw(ttfPath, text);
}

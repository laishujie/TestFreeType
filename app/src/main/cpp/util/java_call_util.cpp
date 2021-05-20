//
// Created by admin on 2021/5/20.
//

#include "java_call_util.h"

JavaCallHelper::JavaCallHelper(JavaVM *_javaVm, JNIEnv *_env, jobject &_jobj) : javaVM(_javaVm),
                                                                               env(_env),
                                                                               jmid_error(nullptr),
                                                                               jmid_text_changed(
                                                                                       nullptr) {
    jobj = env->NewGlobalRef(_jobj);
    jclass jclazz = env->GetObjectClass(jobj);

    jmid_error = env->GetMethodID(jclazz, "onError", "(I)V");
    jmid_text_changed = env->GetMethodID(jclazz, "onTextAreaChanged", "(FFFF)V");

}

JavaCallHelper::~JavaCallHelper() {
    env->DeleteGlobalRef(jobj);
    jobj = nullptr;
}

void JavaCallHelper::onError(int code, int thread) {
    if (thread == THREAD_CHILD) {
        //子线程
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, nullptr) != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_error, code);
        javaVM->DetachCurrentThread();
    } else {
        env->CallVoidMethod(jobj, jmid_error, code);
    }
}

void JavaCallHelper::onTextAreaChanged(float left, float top, float right, float bottom, int thread) const {
    if (thread == THREAD_CHILD) {
        //子线程
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, nullptr) != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_text_changed, left, top, right, bottom);
        javaVM->DetachCurrentThread();
    } else {
        env->CallVoidMethod(jobj, jmid_text_changed, left, top, right, bottom);
    }
}


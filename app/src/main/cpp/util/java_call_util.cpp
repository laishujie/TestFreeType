//
// Created by admin on 2021/5/20.
//

#include "java_call_util.h"

JavaCallHelper::JavaCallHelper(JavaVM *_javaVm, JNIEnv *_env, jobject &_jobj) : javaVM(_javaVm),
                                                                                env(_env),
                                                                                jmid_error(nullptr),
                                                                                jmid_text_changed(
                                                                                        nullptr),
                                                                                jmid_previewInit(
                                                                                        nullptr),
                                                                                jmid_initSuccess(
                                                                                        nullptr),jmid_textLevelChange(
                nullptr) {
    jobj = env->NewGlobalRef(_jobj);
    jclass jclazz = env->GetObjectClass(jobj);

    jmid_error = env->GetMethodID(jclazz, "onError", "(I)V");

    jmid_text_changed = env->GetMethodID(jclazz, "onTextLayerAreaChange", "(IFFFF)V");
    jmid_previewInit = env->GetMethodID(jclazz, "onPreviewInit", "(I)V");

    jmid_textLevelChange= env->GetMethodID(jclazz, "onTextLevelChange", "(ZII)V");
   // jmid_initSuccess = env->GetMethodID(jclazz, "onInitSuccess", "()V");

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

void
JavaCallHelper::onTextAreaChanged(int layerId, float left, float top, float right, float bottom,
                                  int thread) const {
    if (thread == THREAD_CHILD) {
        //子线程
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, nullptr) != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_text_changed, layerId, left, top, right, bottom);
        javaVM->DetachCurrentThread();
    } else {
        env->CallVoidMethod(jobj, jmid_text_changed, layerId, left, top, right, bottom);
    }
}

void JavaCallHelper::onPreviewLayerInitSuccess(int layerId, int thread) {
    if (thread == THREAD_CHILD) {
        //子线程
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, nullptr) != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_previewInit, layerId);
        javaVM->DetachCurrentThread();
    } else {
        env->CallVoidMethod(jobj, jmid_previewInit, layerId);
    }
}

void JavaCallHelper::onInitSuccess(int thread) {
   /* if (thread == THREAD_CHILD) {
        //子线程
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, nullptr) != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_initSuccess);
        javaVM->DetachCurrentThread();
    } else {
        env->CallVoidMethod(jobj, jmid_initSuccess);
    }*/

}

void JavaCallHelper::onTextLevelChange(bool isAdd, int layerId, int subTextId, int thread) {
    if (thread == THREAD_CHILD) {
        //子线程
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, nullptr) != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_textLevelChange, isAdd,layerId,subTextId);
        javaVM->DetachCurrentThread();
    } else {
        env->CallVoidMethod(jobj, jmid_textLevelChange, isAdd,layerId,subTextId);
    }
}


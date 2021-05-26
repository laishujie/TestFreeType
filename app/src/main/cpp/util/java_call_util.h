//
// Created by admin on 2021/5/20.
//

#ifndef TESTFREETYPE_JAVA_CALL_UTIL_H
#define TESTFREETYPE_JAVA_CALL_UTIL_H

#include <jni.h>
//标记线程 因为子线程需要attach
#define THREAD_MAIN 1
#define THREAD_CHILD 2

class JavaCallHelper {
public:
    JavaCallHelper(JavaVM *_javaVm, JNIEnv *_env, jobject &_jobj);

    ~JavaCallHelper();

    void onError(int code, int thread = THREAD_MAIN);

    void onTextAreaChanged(int layerId,float left, float top, float right, float bottom, int thread = THREAD_CHILD) const;
    void onPreviewLayerInitSuccess(int layerId, int thread = THREAD_MAIN);
    void onTextLevelChange(bool isAdd,int layerId,int subTextId,int thread = THREAD_CHILD);
    void onInitSuccess(int thread = THREAD_MAIN);

public:
    JavaVM *javaVM;
    JNIEnv *env;
    jobject jobj;


    jmethodID jmid_error;
    jmethodID jmid_text_changed;
    jmethodID jmid_previewInit;
    jmethodID jmid_initSuccess;
    jmethodID jmid_textLevelChange;

};


#endif //TESTFREETYPE_JAVA_CALL_UTIL_H

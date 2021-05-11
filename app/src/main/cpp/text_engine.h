//
// Created by admin on 2021/5/6.
//

#ifndef TESTFREETYPE_TEXT_ENGINE_H
#define TESTFREETYPE_TEXT_ENGINE_H

#include <jni.h>
#include <android/native_window_jni.h>
#include "text_control.h"
#include <deque>

class text_engine {

    void OnSurfaceChanged(int width, int height);

public:
    text_engine(JNIEnv* env);

    void OnSurfaceCreated(jobject surface,int width, int height);

    void OnSurfaceDestroyed();

    void
    OnDraw(const char *ttfPath, const char *text, char *outPath, bool isHorizontal, int spacing,
           jint i,
           jint i1, jint fontColor, jfloat distanceMark, jfloat outLineDistanceMark,
           jint outlineColor,
           jfloat shadowDistance, jfloat d);


    ~text_engine();

    int Init();

private:
    ANativeWindow* window_;
    JavaVM* vm_;
    //文本控制器
    text_control* player_;
    //文本队列互斥量
    pthread_mutex_t queue_mutex_;
    //信息队列
    std::deque<TextInfo*> text_deque_;

};


#endif //TESTFREETYPE_TEXT_ENGINE_H

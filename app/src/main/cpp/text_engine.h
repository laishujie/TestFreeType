//
// Created by admin on 2021/5/6.
//

#ifndef TESTFREETYPE_TEXT_ENGINE_H
#define TESTFREETYPE_TEXT_ENGINE_H

#include <jni.h>
#include <android/native_window_jni.h>
#include "text_control.h"
#include <map>
#include "cJSON.h"

class text_engine {

    void OnSurfaceChanged(int width, int height);

public:
    text_engine(JNIEnv *env);

    void OnSurfaceCreated(jobject surface, int width, int height);

    void OnSurfaceDestroyed();

    int AddTextLayer(const char *layerJson, const char *fontFolder);

    static int ReadFile(const std::string &path, char **buffer);

    void
    OnDraw(const char *ttfPath, const char *text, char *outPath, bool isHorizontal, int spacing,
           jint lineSpacing,
           jint fontSize, jint fontColor, jfloat distanceMark, jfloat outLineDistanceMark,
           jint outlineColor,
           jfloat shadowDistance, jfloat shadowAlpha, jint shadowColor, jint shadowAngle);


    ~text_engine();

    int Init();

private:
    ANativeWindow *window_;
    JavaVM *vm_;
    //文本控制器
    text_control *player_;
    //文本队列互斥量
    pthread_mutex_t queue_mutex_;
    pthread_mutex_t text_mutex_;
    //信息队列
    std::map<int, TextLayer *> text_layers_;
    //json提取器
    int selfIncreasingId;
};


#endif //TESTFREETYPE_TEXT_ENGINE_H

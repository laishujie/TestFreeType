//
// Created by admin on 2021/5/6.
//

#ifndef TESTFREETYPE_TEXT_ENGINE_H
#define TESTFREETYPE_TEXT_ENGINE_H

#include <jni.h>
#include <android/native_window_jni.h>
#include "text_control.h"

class text_engine {
    int Init();

    void OnSurfaceChanged(int width, int height);

public:
    text_engine(JNIEnv* env);

    void OnSurfaceCreated(jobject surface,int width, int height);

    void OnSurfaceDestroyed();

    void OnDraw(char * ttfPath,char *text);

    ~text_engine();

private:
    ANativeWindow* window_;
    JavaVM* vm_;
    text_control* player_;
};


#endif //TESTFREETYPE_TEXT_ENGINE_H

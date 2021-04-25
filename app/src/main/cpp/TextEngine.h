//
// Created by admin on 2021/4/22.
//

#ifndef TESTFREETYPE_TEXTENGINE_H
#define TESTFREETYPE_TEXTENGINE_H
//

#include <TinySDF.h>
#include "TextureImageDemo.h"
#include <android/bitmap.h>

class TextEngine {
public:
    TextureImageDemo *textureImageDemo;

    TextEngine():textureImageDemo(nullptr){

    }
    void sdfChange(float distanceMark);
    void initSdfShaderByPath(char * textureImagePath,int surfaceWidth,int surfaceHeight);
    void initSdfShaderByBitmap(JNIEnv* env,jobject bitmap,int surfaceWidth,int surfaceHeight);
    void createSdfTexture1(JNIEnv *env,jstring image_file,
                           jstring image_out_file, jfloat radius,
                           jfloat image_aspect);
    void createSdfTexture2(JNIEnv *env,jstring image_file,
                           jstring image_out_file, jfloat radius,
                           jfloat image_aspect);
    void onDraw();
};


#endif //TESTFREETYPE_TEXTENGINE_H

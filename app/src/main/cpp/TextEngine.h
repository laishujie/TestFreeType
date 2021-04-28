//
// Created by admin on 2021/4/22.
//

#ifndef TESTFREETYPE_TEXTENGINE_H
#define TESTFREETYPE_TEXTENGINE_H
//

#include <TinySDF.h>
#include "TextureImageDemo.h"
#include <android/bitmap.h>
#include "FreeTypeShader.h"
#include "ImageLoad.h"
#include "font-manager.h"

class TextEngine {
public:
    GLuint freeTextureId, freeActiveTextureIndex;
    TextureImageDemo *textureImageDemo;
    FreeTypeShader *freeTypeShader;
    ftgl::font_manager_t *fontManager;

    TextEngine() : textureImageDemo(nullptr),
                   freeTypeShader(nullptr),
                   freeTextureId(0),
                   freeActiveTextureIndex(0), fontManager(nullptr) {

    }

    ~TextEngine();

    void sdfChange(float distanceMark);

    void initSdfShaderByPath(char *textureImagePath, int surfaceWidth, int surfaceHeight);

    void initSdfShaderByBitmap(JNIEnv *env, jobject bitmap, int surfaceWidth, int surfaceHeight);

    void createSdfTexture1(JNIEnv *env, jstring image_file,
                           jstring image_out_file, jfloat radius,
                           jfloat image_aspect);

    void createSdfTexture2(JNIEnv *env, jstring image_file,
                           jstring image_out_file, jfloat radius,
                           jfloat image_aspect);

    void onDraw();

    void initFreeTypeShader(const char *paht, int surfaceWidth, int surfaceHeight);

    void insetText(const char *path,const char *text) const;

    void freeTypeDraw() const;
};


#endif //TESTFREETYPE_TEXTENGINE_H

//
// Created by admin on 2021/4/22.
//

#ifndef TESTFREETYPE_TEXTENGINE_H
#define TESTFREETYPE_TEXTENGINE_H
//

#include <tiny_sdf.h>
#include "texture_image_demo.h"
#include <android/bitmap.h>
#include <handler.h>
#include "FreeTypeShader.h"
#include "ImageLoad.h"
#include "font-manager.h"
#include "text_shader.h"

class TextEngine {
public:
    texture_image_demo *textureImageDemo;
    FreeTypeShader *freeTypeShader;
    ftgl::font_manager_t *fontManager;
    text_shader *textShader;

    TextEngine() : textureImageDemo(nullptr),
                   freeTypeShader(nullptr),
                   fontManager(ftgl::font_manager_new(512, 512, 1)), textShader(nullptr){

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

    ftgl::texture_font_t *insetText(const char *path, const char *text) const;

    const char *getTextInfo(const char *path, const char *text) const;

    void freeTypeDraw() const;

    void glFontManagerInit() const;

    void glInitTextShader(int surfaceWidth, int surfaceHeight);

    void glRenderText(const char *path, const char *text) const;
};


#endif //TESTFREETYPE_TEXTENGINE_H

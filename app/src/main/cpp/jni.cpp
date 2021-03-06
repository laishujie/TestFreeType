#include <jni.h>
#include "TextEngine.h"
#include "util/logUtil.h"
#include <texture-atlas.h>

#include "texture-atlas.h"
#include "texture-font.h"
#include "util/ImageLoad.h"
#include "text_engine.h"

#ifdef __cplusplus
#ifndef NOT_USING_FT_GL_NAMESPACE
using namespace ftgl;
#endif /* NOT_USING_FT_GL_NAMESPACE */
#endif /* __cplusplus */


//
// Created by admin on 2021/4/21.
//

JavaVM *javaVm = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if ((vm)->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    javaVm = vm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_test(JNIEnv
                                                 *env,
                                                 jobject clazz,
                                                 jstring path,
                                                 jstring outPath
) {
    texture_font_t *font;
    texture_atlas_t *atlas = texture_atlas_new(512, 512, 1);
    const char *filename = "./Vera.ttf";
    const char *cache = "1234567789ABCDEFGHIJKLMB  huashdaisdhaisdhias";

    const char *src = env->GetStringUTFChars(path, nullptr);
    font = texture_font_new_from_file(atlas, 72, src);

    font->rendermode = RENDER_SIGNED_DISTANCE_FIELD;

    texture_font_load_glyphs(font, cache);

    const char *savePath = env->GetStringUTFChars(outPath, nullptr);


    if (!ImageLoad::savePng(savePath, atlas->width, atlas->height, 1, atlas->data, 0)) {
        LOGE("11111", "ERROR: could not write image");
    }

    texture_font_delete(font);
    texture_atlas_delete(atlas);
    env->ReleaseStringUTFChars(outPath, savePath);
    env->ReleaseStringUTFChars(path, src);
    LOGE("11111", "atlas id %d width %d height %d", atlas->id, atlas->width, atlas->height)
}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_testSDF(JNIEnv *env, jobject clazz, jstring image_file,
                                                    jstring image_out_file,
                                                    jfloat radius, jfloat image_aspect) {
    //tx->createSdfTexture2(env, image_file, image_out_file, radius, image_aspect);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_testOpengGlSDF(JNIEnv *env, jobject clazz,
                                                           jstring image_file,
                                                           jstring image_out_file, jfloat radius,
                                                           jfloat image_aspect) {
    //auto *text = reinterpret_cast<TextEngine *>(na);
    //tx->createSdfTexture1(env, image_file, image_out_file, radius, image_aspect);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_testSdfShaderOnDraw(JNIEnv *env, jobject clazz,
                                                                jlong na, jint stokeColor,
                                                                jint shadowColor,
                                                                jfloat distanceMark,
                                                                jfloat outLineDistanceMark,
                                                                jfloat shadowDistance,
                                                                jint shadowAngle,
                                                                jfloat shadowAlpha) {

    auto *text = reinterpret_cast<TextEngine *>(na);

    text->textureImageDemo->_StokeLineColor = stokeColor;
    text->textureImageDemo->_ShadowColor = shadowColor;
    text->textureImageDemo->_DistanceMark = distanceMark;
    text->textureImageDemo->_OutlineMark = outLineDistanceMark;
    text->textureImageDemo->_ShadowDistanceMark = shadowDistance;
    text->textureImageDemo->_ShadowAngleMark = shadowAngle;
    text->textureImageDemo->_ShadowAlpha = shadowAlpha;


    text->onDraw();

    LOGE("1111", "distanceMark %f  outLineDistanceMark %f shadowDistance %f shadowAngle  %d",
         distanceMark, outLineDistanceMark, shadowDistance, shadowAngle)
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_testSdfShaderInit(JNIEnv *env, jobject clazz,
                                                              jlong text_engine,
                                                              jstring sdf_path, jint s_width,
                                                              jint s_height, jobject bitmap) {

    auto *textEngine = reinterpret_cast<TextEngine *>(text_engine);

    if (sdf_path != nullptr) {
        const char *src = env->GetStringUTFChars(sdf_path, nullptr);
        char *dest = new char[env->GetStringUTFLength(sdf_path)];
        strcpy(dest, src);

        textEngine->initSdfShaderByPath(dest, s_width, s_height);
        env->ReleaseStringUTFChars(sdf_path, src);
    } else if (bitmap != nullptr) {
        textEngine->initSdfShaderByBitmap(env, bitmap, s_width, s_height);
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TypeJni_testSdfChange(JNIEnv *env, jobject thiz, jlong na,
                                                    jfloat distance_mark) {
    auto *text = reinterpret_cast<TextEngine *>(na);
    float d = abs(sin(distance_mark));
    text->textureImageDemo->change(d, 0., 0.);
}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_testFreeTypeInit(JNIEnv *env, jobject thiz,
                                                             jlong text_engine, jstring ttf_path,
                                                             jint surface_width,
                                                             jint surface_height) {
    auto *textEngine = reinterpret_cast<TextEngine *>(text_engine);

    if (ttf_path != nullptr) {
        const char *src = env->GetStringUTFChars(ttf_path, nullptr);
        textEngine->initFreeTypeShader(src, surface_width, surface_height);
        env->ReleaseStringUTFChars(ttf_path, src);
    }



//    if (ttf_path != nullptr) {
//        texture_font_t *font;
//        texture_atlas_t *atlas = texture_atlas_new(512, 512, 1);
//        //const char *filename = "./Vera.ttf";
//        const char *cache = "????????????";
//        const char *src = env->GetStringUTFChars(ttf_path, nullptr);
//        //?????????????????????
//        font = texture_font_new_from_file(atlas, 72, src);
//
//        font->rendermode = RENDER_SIGNED_DISTANCE_FIELD;
//        texture_font_load_glyphs(font, cache);
//
//
//        textEngine->initFreeTypeShader(src, surface_width, surface_height);
//
//        texture_font_delete(font);
//        texture_atlas_delete(atlas);
//
//        env->ReleaseStringUTFChars(ttf_path, src);
//    } else {
//        const char *src = env->GetStringUTFChars(sdf_filePath, nullptr);
//        int iw, ih, n;
//        // ????????????????????????????????????????????????
//        unsigned char *idata = ImageLoad::load(src, &iw, &ih, &n, 1);
//        textEngine->initFreeTypeShader(src, surface_width, surface_height);
//        //LOGE("11111", "iw %d ih %d n %d data %d", iw, ih, n, idata != nullptr)
//        ImageLoad::imageFree(idata);
//        env->ReleaseStringUTFChars(sdf_filePath, src);
//    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_freeDraw(JNIEnv *env, jobject thiz,
                                                     jlong native_handle) {
    auto *text = reinterpret_cast<TextEngine *>(native_handle);

    text->freeTypeDraw();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_insetText(JNIEnv *env, jobject thiz,
                                                      jlong native_handle,
                                                      jstring ttf_path, jstring text) {
    const char *src = env->GetStringUTFChars(ttf_path, nullptr);
    const char *insetText = env->GetStringUTFChars(text, nullptr);

    auto *textEngine = reinterpret_cast<TextEngine *>(native_handle);
    textEngine->insetText(src, insetText);

    env->ReleaseStringUTFChars(ttf_path, src);
    env->ReleaseStringUTFChars(text, insetText);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_testfreetype_TextEngineJni_initTextEngine(JNIEnv *env, jobject thiz) {
    return reinterpret_cast<jlong>(new TextEngine());

}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_destroy(JNIEnv *env, jobject thiz,
                                                    jlong native_handle) {

    auto *textEngine = reinterpret_cast<TextEngine *>(native_handle);
    delete textEngine;
}

jstring chartoJstring(JNIEnv *env, const char *pat) {
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *) pat);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_testfreetype_TextEngineJni_printTextInfo(JNIEnv *env, jobject thiz,
                                                          jlong native_handle, jstring ttf,
                                                          jstring str) {

    auto *textEngine = reinterpret_cast<TextEngine *>(native_handle);
    const char *src = env->GetStringUTFChars(str, nullptr);
    const char *ttfPath = env->GetStringUTFChars(ttf, nullptr);

    const char *strings = textEngine->getTextInfo(ttfPath, src);

    env->ReleaseStringUTFChars(str, src);
    env->ReleaseStringUTFChars(ttf, ttfPath);
    return chartoJstring(env, strings);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_glInitTextShader(JNIEnv *env, jobject thiz,
                                                             jlong native_handle,
                                                             jint surface_width,
                                                             jint suface_height) {
    auto *textEngine = reinterpret_cast<TextEngine *>(native_handle);

    textEngine->glInitTextShader(surface_width, suface_height);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_glRenderText(JNIEnv *env, jobject thiz,
                                                         jlong native_handle, jstring ttf_path,
                                                         jstring text) {
    auto *textEngine = reinterpret_cast<TextEngine *>(native_handle);
    const char *src = env->GetStringUTFChars(ttf_path, nullptr);
    const char *insetText = env->GetStringUTFChars(text, nullptr);

    textEngine->glRenderText(src, insetText);

    env->ReleaseStringUTFChars(ttf_path, src);
    env->ReleaseStringUTFChars(text, insetText);
}extern "C"


JNIEXPORT jlong JNICALL
Java_com_example_testfreetype_TextEngineJni_textEngineCreate(JNIEnv *env, jobject thiz) {
    auto textEngine = new text_engine(env, new JavaCallHelper(javaVm, env, thiz));
    textEngine->Init();
    return reinterpret_cast<jlong>(textEngine);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_textEngineSurfaceCreated(JNIEnv *env, jobject thiz,
                                                                     jlong handler, jobject surface,
                                                                     jint width, jint height) {
    if (handler <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handler);
    editor->OnSurfaceCreated(surface, width, height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_textEngineSurfaceDestroyed(JNIEnv *env, jobject thiz,
                                                                       jlong handler) {
    if (handler <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handler);
    editor->OnSurfaceDestroyed();
    delete editor;
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_textEngineRelease(JNIEnv *env, jobject thiz,
                                                              jlong handler) {
//    if (handler <= 0) {
//        return;
//    }
//    auto* editor = reinterpret_cast<text_engine*>(handler);
//    delete editor;
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_textEngineDraw(JNIEnv *env, jobject thiz,
                                                           jlong native_handle,
                                                           jstring ttf_path, jstring text,
                                                           jstring outPath) {
    auto *editor = reinterpret_cast<text_engine *>(native_handle);

    char *ttf_File_copy = nullptr;
    char *text_file_copy = nullptr;
    char *out_File_copy = nullptr;

    if (ttf_path != nullptr) {
        const char *src = env->GetStringUTFChars(ttf_path, nullptr);
        ttf_File_copy = new char[strlen(src) + 1];
        sprintf(ttf_File_copy, "%s%c", src, 0);

        env->ReleaseStringUTFChars(ttf_path, src);
    }

    if (text != nullptr) {
        const char *insetText = env->GetStringUTFChars(text, nullptr);
        text_file_copy = new char[strlen(insetText) + 1];
        sprintf(text_file_copy, "%s%c", insetText, 0);
        env->ReleaseStringUTFChars(text, insetText);
    }

    if (outPath != nullptr) {
        const char *out_Path = env->GetStringUTFChars(outPath, nullptr);
        out_File_copy = new char[strlen(out_Path) + 1];
        sprintf(out_File_copy, "%s%c", out_Path, 0);
        env->ReleaseStringUTFChars(outPath, out_Path);
    }

    editor->DrawPreView(ttf_File_copy, text_file_copy, true, 0, 0, 0, 0, 0, 0, 0, 0,
                        0,
                        0, 0);
}


void getTextInfo(JNIEnv *env, jclass textInfoClazz, jobject textInfoJobject, TextInfo *&textInfo) {
    jfieldID text_file_id = env->GetFieldID(textInfoClazz, "char", "Ljava/lang/String;");
    jfieldID ttf_file_id = env->GetFieldID(textInfoClazz, "ttfPath", "Ljava/lang/String;");

    auto j_text = (jstring) (env->GetObjectField(textInfoClazz, text_file_id));
    auto j_ttf = (jstring) (env->GetObjectField(textInfoClazz, ttf_file_id));

    jfieldID horizontal_file_id = env->GetFieldID(textInfoClazz, "horizontal", "Z");
    jfieldID spacing_file_id = env->GetFieldID(textInfoClazz, "spacing", "I");
    jfieldID lineSpacing_file_id = env->GetFieldID(textInfoClazz, "lineSpacing", "I");
    jfieldID size_file_id = env->GetFieldID(textInfoClazz, "size", "I");
    jfieldID color_file_id = env->GetFieldID(textInfoClazz, "fontColor", "I");
    jfieldID distanceMark_file_id = env->GetFieldID(textInfoClazz, "distanceMark", "F");
    jfieldID outLineDistanceMark_file_id = env->GetFieldID(textInfoClazz,
                                                           "outLineDistanceMark", "F");
    jfieldID outLineColor_file_id = env->GetFieldID(textInfoClazz, "outLineColor", "I");

    jfieldID shadowDistance_file_id = env->GetFieldID(textInfoClazz, "shadowDistance", "F");
    jfieldID shadowAlpha_file_id = env->GetFieldID(textInfoClazz, "shadowAlpha", "F");
    jfieldID shadowColor_file_id = env->GetFieldID(textInfoClazz, "shadowColor", "I");
    jfieldID shadowAngle_file_id = env->GetFieldID(textInfoClazz, "shadowAngle", "I");

    jboolean isHorizontal = env->GetBooleanField(textInfoJobject, horizontal_file_id);
    jint spacing = env->GetIntField(textInfoJobject, spacing_file_id);
    jint lineSpacing = env->GetIntField(textInfoJobject, lineSpacing_file_id);
    jint fontSize = env->GetIntField(textInfoJobject, size_file_id);
    jint fontColor = env->GetIntField(textInfoJobject, color_file_id);
    jfloat distanceMark = env->GetFloatField(textInfoJobject, distanceMark_file_id);
    jfloat outLineDistanceMark = env->GetFloatField(textInfoJobject,
                                                    outLineDistanceMark_file_id);

    jint outlineColor = env->GetIntField(textInfoJobject, outLineColor_file_id);

    jfloat shadowDistance = env->GetFloatField(textInfoJobject, shadowDistance_file_id);
    jfloat shadowAlpha = env->GetFloatField(textInfoJobject, shadowAlpha_file_id);

    jint shadowColor = env->GetIntField(textInfoJobject, shadowColor_file_id);
    jint shadowAngle = env->GetIntField(textInfoJobject, shadowAngle_file_id);


    const char *text = env->GetStringUTFChars(j_text, nullptr);
    const char *ttf_path = env->GetStringUTFChars(j_ttf, nullptr);

    textInfo->ttf_file = ttf_path;
    textInfo->isHorizontal = isHorizontal;
    textInfo->spacing = spacing;
    textInfo->lineSpacing = lineSpacing;
    textInfo->fontSize = fontSize;
    textInfo->fontColor = fontColor;
    textInfo->distanceMark = distanceMark;
    textInfo->outlineDistanceMark = outLineDistanceMark;
    textInfo->outLineColor = outlineColor;
    textInfo->shadowDistance = shadowDistance;
    textInfo->shadowAlpha = shadowAlpha;
    textInfo->shadowColor = shadowColor;
    textInfo->shadowAngle = shadowAngle;

    env->ReleaseStringUTFChars(j_ttf, ttf_path);
    env->ReleaseStringUTFChars(j_text, text);

    env->DeleteLocalRef(textInfoClazz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_textEngineDrawPreView(JNIEnv *env, jobject thiz,
                                                                  jlong handler, jobject layer) {
    if (handler <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handler);

    jclass clip_clazz = env->GetObjectClass(layer);

    jfieldID text_file_id = env->GetFieldID(clip_clazz, "char", "Ljava/lang/String;");
    jfieldID ttf_file_id = env->GetFieldID(clip_clazz, "ttfPath", "Ljava/lang/String;");

    auto j_text = static_cast<jstring>(env->GetObjectField(layer, text_file_id));
    auto j_ttf = static_cast<jstring>(env->GetObjectField(layer, ttf_file_id));

    jfieldID horizontal_file_id = env->GetFieldID(clip_clazz, "horizontal", "Z");
    jfieldID spacing_file_id = env->GetFieldID(clip_clazz, "spacing", "I");
    jfieldID lineSpacing_file_id = env->GetFieldID(clip_clazz, "lineSpacing", "I");
    jfieldID size_file_id = env->GetFieldID(clip_clazz, "size", "I");
    jfieldID color_file_id = env->GetFieldID(clip_clazz, "fontColor", "I");
    jfieldID distanceMark_file_id = env->GetFieldID(clip_clazz, "distanceMark", "F");
    jfieldID outLineDistanceMark_file_id = env->GetFieldID(clip_clazz, "outLineDistanceMark", "F");
    jfieldID outLineColor_file_id = env->GetFieldID(clip_clazz, "outLineColor", "I");

    jfieldID shadowDistance_file_id = env->GetFieldID(clip_clazz, "shadowDistance", "F");
    jfieldID shadowAlpha_file_id = env->GetFieldID(clip_clazz, "shadowAlpha", "F");
    jfieldID shadowColor_file_id = env->GetFieldID(clip_clazz, "shadowColor", "I");
    jfieldID shadowAngle_file_id = env->GetFieldID(clip_clazz, "shadowAngle", "I");

    jboolean isHorizontal = env->GetBooleanField(layer, horizontal_file_id);
    jint spacing = env->GetIntField(layer, spacing_file_id);
    jint lineSpacing = env->GetIntField(layer, lineSpacing_file_id);
    jint fontSize = env->GetIntField(layer, size_file_id);
    jint fontColor = env->GetIntField(layer, color_file_id);
    jfloat distanceMark = env->GetFloatField(layer, distanceMark_file_id);
    jfloat outLineDistanceMark = env->GetFloatField(layer, outLineDistanceMark_file_id);

    jint outlineColor = env->GetIntField(layer, outLineColor_file_id);

    jfloat shadowDistance = env->GetFloatField(layer, shadowDistance_file_id);
    jfloat shadowAlpha = env->GetFloatField(layer, shadowAlpha_file_id);

    jint shadowColor = env->GetIntField(layer, shadowColor_file_id);
    jint shadowAngle = env->GetIntField(layer, shadowAngle_file_id);

    const char *text = j_text == nullptr ? "" : env->GetStringUTFChars(j_text, nullptr);
    const char *ttf_path = j_ttf == nullptr ? "" : env->GetStringUTFChars(j_ttf, nullptr);

    editor->DrawPreView(ttf_path, text, isHorizontal, spacing, lineSpacing, fontSize,
                        fontColor, distanceMark, outLineDistanceMark, outlineColor, shadowDistance,
                        shadowAlpha, shadowColor, shadowAngle);

    env->ReleaseStringUTFChars(j_ttf, ttf_path);
    env->ReleaseStringUTFChars(j_text, text);

    env->DeleteLocalRef(clip_clazz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_testLayer(JNIEnv *env, jobject thiz, jlong handler,
                                                      jstring json,
                                                      jstring font_folder) {
    if (handler <= 0) {
        return;
    }

    auto *editor = reinterpret_cast<text_engine *>(handler);

    const char *json_file = env->GetStringUTFChars(json, nullptr);
    const char *font_folder_ = env->GetStringUTFChars(font_folder, nullptr);


    editor->AddTextLayer(json_file, font_folder_);

    env->ReleaseStringUTFChars(json, json_file);
    env->ReleaseStringUTFChars(font_folder, font_folder_);
}


extern "C"
JNIEXPORT int JNICALL
Java_com_example_testfreetype_TextEngineJni_addTextLayer(JNIEnv *env, jobject thiz, jlong handler,
                                                         jobject text_layer) {

    if (handler <= 0) {
        return -1;
    }

    int layerId = -1;

    auto *editor = reinterpret_cast<text_engine *>(handler);

    jclass textLayerClazz = env->GetObjectClass(text_layer);
    jfieldID textList_fileId = env->GetFieldID(textLayerClazz, "textList", "Ljava/util/ArrayList;");
    jfieldID templateFolder_file_id = env->GetFieldID(textLayerClazz, "templateFolder",
                                                      "Ljava/lang/String;");

    jint textLayerId = env->GetIntField(text_layer,
                                        env->GetFieldID(textLayerClazz, "layerId", "I"));
    auto *textLayer = new TextLayer();

    textLayer->id = textLayerId;
    textLayer->isDraw = true;
    textLayer->isFristCreate = true;
    textLayer->isTemplate = true;

    //??????textList??????
    jobject textListObj = env->GetObjectField(text_layer, textList_fileId);

    auto template_jst = (jstring) env->GetObjectField(text_layer, templateFolder_file_id);
    const char *templateFolder = env->GetStringUTFChars(template_jst, nullptr);
    textLayer->templateFolder = templateFolder;

    if (textListObj != nullptr) {
        int i;
        jclass cls_arraylist = env->GetObjectClass(textListObj);

        jmethodID arraylist_get = env->GetMethodID(cls_arraylist, "get", "(I)Ljava/lang/Object;");
        jmethodID arraylist_size = env->GetMethodID(cls_arraylist, "size", "()I");
        jint len = env->CallIntMethod(textListObj, arraylist_size);


        for (i = 0; i < len; i++) {
            jobject obj_textInfo = env->CallObjectMethod(textListObj, arraylist_get, i);
            jclass clip_clazz = env->GetObjectClass(obj_textInfo);
            auto *info = new TextInfo();

            jfieldID id_file_id = env->GetFieldID(clip_clazz, "id", "I");
            jint id = env->GetIntField(obj_textInfo, id_file_id);


            jfieldID text_file_id = env->GetFieldID(clip_clazz, "char", "Ljava/lang/String;");
            auto pJobject = (jstring) env->GetObjectField(obj_textInfo, text_file_id);
            const char *text = env->GetStringUTFChars(pJobject, nullptr);

            jfieldID ttf_file_id = env->GetFieldID(clip_clazz, "ttfPath", "Ljava/lang/String;");
            auto tJobject = (jstring) env->GetObjectField(obj_textInfo, ttf_file_id);
            const char *ttfPath = env->GetStringUTFChars(tJobject, nullptr);


            jfieldID size_file_id = env->GetFieldID(clip_clazz, "size", "I");
            jint fontSize = env->GetIntField(obj_textInfo, size_file_id);

            jfieldID color_file_id = env->GetFieldID(clip_clazz, "fontColor", "I");
            jint fontColor = env->GetIntField(obj_textInfo, color_file_id);


            jfieldID offsetX_file_id = env->GetFieldID(clip_clazz, "offsetX", "F");
            jint offsetX = env->GetFloatField(obj_textInfo, offsetX_file_id);

            jfieldID offsetY_file_id = env->GetFieldID(clip_clazz, "offsetY", "F");
            jint offsetY = env->GetFloatField(obj_textInfo, offsetY_file_id);

            jfieldID isTextImage_file_id = env->GetFieldID(clip_clazz, "isTextImage", "Z");
            jboolean isTextImage = env->GetBooleanField(obj_textInfo, isTextImage_file_id);


            jfieldID img_file_id = env->GetFieldID(clip_clazz, "file", "Ljava/lang/String;");
            auto imgJstring = (jstring) env->GetObjectField(obj_textInfo, img_file_id);
            const char *imgPath = env->GetStringUTFChars(imgJstring, nullptr);

            info->id = id;
            info->text = text;
            info->ttf_file = ttfPath;
            info->fontColor = fontColor;
            info->fontSize = fontSize;
            info->offset_x = offsetX;
            info->offset_y = offsetY;
            info->isFromTemplate = true;
            info->isTextImage = isTextImage;
            info->file = imgPath;
            textLayer->text_deque.push_back(info);

            env->DeleteLocalRef(obj_textInfo);
            env->ReleaseStringUTFChars(pJobject, text);
            env->ReleaseStringUTFChars(imgJstring, imgPath);
            env->ReleaseStringUTFChars(tJobject, ttfPath);
        }
        env->ReleaseStringUTFChars(template_jst, templateFolder);

        editor->AddTextLayer(textLayer);
    } else {
        delete textLayer;
        textLayer = nullptr;
    }


    return layerId;
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_testfreetype_TextEngineJni_addThePreviewLayer2Map(JNIEnv *env, jobject thiz,
                                                                   jlong handle) {
    if (handle <= 0) {
        return -1;
    }
    auto *editor = reinterpret_cast<text_engine *>(handle);

    return editor->AddThePreviewLayer2Map();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_textEngineDrawPreViewByJson(JNIEnv *env, jobject thiz,
                                                                        jlong handler, jstring json,
                                                                        jstring font_folder) {
    if (handler <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handler);

    const char *json_file = env->GetStringUTFChars(json, nullptr);
    const char *font_folder_ = env->GetStringUTFChars(font_folder, nullptr);

    editor->DrawPreViewByJson(json_file, font_folder_);

    env->ReleaseStringUTFChars(json, json_file);
    env->ReleaseStringUTFChars(font_folder, font_folder_);
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_testfreetype_TextEngineJni_addThePreviewLayerByJson2Map(JNIEnv *env, jobject thiz,
                                                                         jlong handle) {
    if (handle <= 0) {
        return -1;
    }
    auto *editor = reinterpret_cast<text_engine *>(handle);
    return editor->AddThePreviewLayer2MapByJson();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_cleanPreview(JNIEnv *env, jobject thiz, jlong handle) {
    if (handle <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handle);

    editor->CleanPreview();
}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_removeTextLayer(JNIEnv *env, jobject thiz, jlong handle,
                                                            jint layer_id) {
    if (handle <= 0) {
        return;
    }

    auto *editor = reinterpret_cast<text_engine *>(handle);
    editor->RemoveTextLayer(layer_id);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_addSimpleSubText(JNIEnv *env, jobject thiz,
                                                             jlong handle, jint layer_id,
                                                             jint subText_id,
                                                             jstring j_ttf, jstring j_text,
                                                             jint font_size, jint fon_color) {
    const char *text = env->GetStringUTFChars(j_text, nullptr);
    const char *ttf_path = env->GetStringUTFChars(j_ttf, nullptr);

    if (handle <= 0) {
        return;
    }

    auto *editor = reinterpret_cast<text_engine *>(handle);
    editor->AddSimpleSubtext(layer_id, subText_id, ttf_path, text, font_size, fon_color);

    env->ReleaseStringUTFChars(j_ttf, ttf_path);
    env->ReleaseStringUTFChars(j_text, text);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_setBasicTextAttributes(JNIEnv *env, jobject thiz,
                                                                   jlong handler, jint layer_id,
                                                                   jint sub_id, jstring j_ttf,
                                                                   jstring j_text, jint font_size,
                                                                   jint font_color) {
    if (handler <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handler);
    const char *text = env->GetStringUTFChars(j_text, nullptr);
    const char *ttf_path = env->GetStringUTFChars(j_ttf, nullptr);


    editor->SetBasicTextAttributes(layer_id, sub_id, text, ttf_path, font_size, font_color);


    env->ReleaseStringUTFChars(j_ttf, ttf_path);
    env->ReleaseStringUTFChars(j_text, text);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_printAll(JNIEnv *env, jobject thiz,
                                                     jlong native_handle) {
    if (native_handle <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(native_handle);
    editor->printAll();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_setStrokeAttributes(JNIEnv *env, jobject thiz,
                                                                jlong handler, jint layer_id,
                                                                jint sub_id, jfloat line_distance,
                                                                jfloat out_line_distance,
                                                                jint out_line_color) {
    if (handler <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handler);
    editor->SetStrokeAttributes(layer_id, sub_id, line_distance, out_line_distance, out_line_color);

}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_updateTextLayerFrameIndex(JNIEnv *env, jobject thiz,
                                                                      jlong handle, jint layer_id,
                                                                      jint frame_index) {
    if (handle <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handle);
    editor->UpdateTextLayerFrame(layer_id, frame_index);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TextEngineJni_textLayerTransform(JNIEnv *env, jobject thiz,
                                                               jlong handler, jint layer_id,
                                                               jfloat tx, jfloat ty,
                                                               jfloat center_x, jfloat center_y,
                                                               jfloat sc, jfloat r) {
    if (handler <= 0) {
        return;
    }
    auto *editor = reinterpret_cast<text_engine *>(handler);

    editor->TextLayerTransform(layer_id, tx, ty, center_x,center_y,sc, r);
}
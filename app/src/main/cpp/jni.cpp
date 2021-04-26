#include <jni.h>
#include "TextEngine.h"
#include <logUtil.h>

//
// Created by admin on 2021/4/21.
//
TextEngine *tx = new TextEngine();
extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TypeJni_test(JNIEnv
                                           *env,
                                           jclass clazz
) {

}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TypeJni_testSDF(JNIEnv *env, jclass clazz, jstring image_file,
                                              jstring image_out_file,
                                              jfloat radius, jfloat image_aspect) {
    tx->createSdfTexture2(env,image_file,image_out_file,radius,image_aspect);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TypeJni_testOpengGlSDF(JNIEnv *env, jclass clazz,jstring image_file,
                                                     jstring image_out_file, jfloat radius,
                                                     jfloat image_aspect) {
    //auto *text = reinterpret_cast<TextEngine *>(na);
    tx->createSdfTexture1(env,image_file,image_out_file,radius,image_aspect);
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_testfreetype_TypeJni_testSdfShaderOnDraw(JNIEnv *env, jclass clazz,
                                                          jlong na,  jint stokeColor ,jint shadowColor ,jfloat distanceMark,jfloat outLineDistanceMark,jfloat shadowDistance
                                                          ,jint shadowAngle
                                                          ,jfloat shadowAlpha) {

    auto *text = reinterpret_cast<TextEngine *>(na);

    text->textureImageDemo->_StokeLineColor = stokeColor;
    text->textureImageDemo->_ShadowColor = shadowColor;
    text->textureImageDemo->_DistanceMark = distanceMark;
    text->textureImageDemo->_OutlineMark = outLineDistanceMark;
    text->textureImageDemo->_ShadowDistanceMark = shadowDistance;
    text->textureImageDemo->_ShadowAngleMark = shadowAngle;
    text->textureImageDemo->_ShadowAlpha = shadowAlpha;


    text->onDraw();

    LOGE("1111","distanceMark %f  outLineDistanceMark %f shadowDistance %f shadowAngle  %d"
         ,distanceMark,outLineDistanceMark,shadowDistance,shadowAngle)
    return 0L;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_testfreetype_TypeJni_testSdfShaderInit(JNIEnv *env, jclass clazz,
                                                        jstring sdf_path, jint s_width,
                                                        jint s_height,jobject bitmap) {

    auto *textEngine = new TextEngine();

    if(sdf_path!= nullptr){
        const char *src = env->GetStringUTFChars(sdf_path, nullptr);
        char *dest = new char[env->GetStringUTFLength(sdf_path)];
        strcpy(dest, src);

        textEngine->initSdfShaderByPath(dest, s_width, s_height);
        env->ReleaseStringUTFChars(sdf_path, src);
    }else{
        textEngine->initSdfShaderByBitmap(env,bitmap, s_width, s_height);
    }

    return reinterpret_cast<jlong>(textEngine);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_testfreetype_TypeJni_testSdfChange(JNIEnv *env, jclass clazz,jlong na,
                                                    jfloat distance_mark) {
    auto *text = reinterpret_cast<TextEngine *>(na);
    float d = abs(sin(distance_mark));
    text->textureImageDemo->change(d,0.,0.);

}
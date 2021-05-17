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

    /**
     * surfaceView 创建
     * @param surface
     * @param width
     * @param height
     */
    void OnSurfaceCreated(jobject surface, int width, int height);

    /**
     * 销毁
     */
    void OnSurfaceDestroyed();

    /**
     * 根据配置文件创建text
     * @param layerJson json文件
     * @param fontFolder 字体存放路径
     * @return
     */
    int AddTextLayer(const char *layerJson, const char *fontFolder);

    /**
     * 添加文字层
     * @param ttfPath  字体路径
     * @param text      渲染文本
     * @param isHorizontal 是否横向
     * @param spacing  横向间距
     * @param lineSpacing 竖向间距
     * @param fontSize 字体大小
     * @param fontColor 字体颜色
     * @param distanceMark 距离
     * @param outLineDistanceMark 描边距离
     * @param outlineColor 描边颜色
     * @param shadowDistance 阴影距离
     * @param shadowAlpha 阴影透明度
     * @param shadowColor 阴影颜色
     * @param shadowAngle 阴影角度
     * @return
     */
    int AddTextLayer(const char *ttfPath, const char *text, char *outPath,
                     bool isHorizontal, int spacing,
                     int lineSpacing, int fontSize, int fontColor, float distanceMark,
                     float outLineDistanceMark, int outLineColor, float shadowDistance,
                     float shadowAlpha,
                     int shadowColor, int shadowAngle);

    int UpdateTextInfo(int layerId, const char *ttfPath, const char *text, char *outPath,
                       bool isHorizontal, int spacing,
                       int lineSpacing, int fontSize, int fontColor, float distanceMark,
                       float outLineDistanceMark, int outLineColor, float shadowDistance,
                       float shadowAlpha,
                       int shadowColor, int shadowAngle);

    /**
     * 读取文件
     * @param path
     * @param buffer
     * @return
     */
    static int ReadFile(const std::string &path, char **buffer);

    void DrawPreView(const char *ttfPath, const char *text, char *outPath, bool isHorizontal, int spacing,
                     int lineSpacing,
                     int fontSize, int fontColor, float distanceMark, float outLineDistanceMark,
                     int outlineColor,
                     float shadowDistance, float shadowAlpha, int shadowColor, int shadowAngle);


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

//
// Created by admin on 2021/5/6.
//

#ifndef TESTFREETYPE_TEXT_CONTROL_H
#define TESTFREETYPE_TEXT_CONTROL_H


#include "message/handler.h"
#include "egl/egl_core.h"
#include "shader_manager.h"
#include "cJSON.h"
#include "java_call_util.h"

#define PREVIEW_ID  1000

class text_control : public Handler {
private:
    BufferPool *buffer_pool_;
    ANativeWindow *window_;
    EGLSurface render_surface_;
    EGLCore *core_;
    int surface_width_;
    int surface_height_;
    MessageQueue *message_queue_;
    pthread_t message_queue_thread_;
    ShaderManager *shaderManager_;
    //普通文字预览层
    TextLayer *previewLayer;
    //模版预览层
    TextLayer *previewTemplateLayer;

    JavaCallHelper *javaCallHelper;

    std::map<int, TextLayer *> layerMaps;
    //Id 自增
    int LayerSelfIdIncreasing;

    int RestoreTmpLayer(bool isFromTemplate);

    static int FillFrame(std::string &templateFolder, TextInfo *&info);

    /**
  * 读取文件
  * @param path
  * @param buffer
  * @return
  */
    static int ReadFile(const std::string &path, char **buffer);


public:
    int Init(JavaCallHelper *callHelper);

    /**
     * 在OpenGL线程发送消息,创建显示的surface
     * 并且创建OpenGL相关的资源, 包含屏幕显示, 硬解需要的纹理
     * 如果此时已经收到了开始播放或者暂停的消息, 则播放视频
     * @param window 创建EGLSurface需要的window
     */
    void OnSurfaceCreated(ANativeWindow *window, int width, int height);

    /**
     * SurfaceView改变时回调,记录新的宽和高
     * @param width 需要显示的宽
     * @param height 需要显示的高
     */
    void OnSurfaceChanged(int width, int height);

    /**
     * SurfaceView销毁时调用
     * 在OpenGL线程发送消息, 销毁EGLSurface
     * 同时把当前记录的视频的宽和高设置为0
     * 在渲染视频时需要重新建立一个FrameBuffer
     */
    void OnSurfaceDestroy();

    virtual void HandleMessage(Message *msg);

    void OnGLCreate();

    void OnGLWindowCreate();

    void OnGLWindowDestroy();

    void OnGLDestroy();

    void OnGlResourcesDestroy();

    static void *MessageQueueThread(void *args);

    text_control();

    ~text_control();

    void ProcessMessage();

    /**
     * 更新临时层数据
     * @param ttfPath
     * @param text
     * @param isHorizontal
     * @param spacing
     * @param lineSpacing
     * @param fontSize
     * @param fontColor
     * @param distanceMark
     * @param outLineDistanceMark
     * @param outLineColor
     * @param shadowDistance
     * @param shadowAlpha
     * @param shadowColor
     * @param shadowAngle
     */
    void UpdatePreViewTextInfo(const char *ttfPath, const char *text, bool isHorizontal,
                               int spacing,
                               int lineSpacing, int fontSize, int fontColor, float distanceMark,
                               float outLineDistanceMark, int outLineColor, float shadowDistance,
                               float shadowAlpha,
                               int shadowColor, int shadowAngle);

    int AddTextLayerByJson(const char *layerJson, const char *fontFolder);


    int AddTextLayer(const char *ttfPath, const char *text,
                     bool isHorizontal, int spacing,
                     int lineSpacing, int fontSize, int fontColor, float distanceMark,
                     float outLineDistanceMark, int outLineColor, float shadowDistance,
                     float shadowAlpha,
                     int shadowColor, int shadowAngle);

    int AddTextLayer(TextLayer *&textLayer);

    /**
     * 发送重绘事件
     */
    void PostDisplay();

    /**
     * 发送添加文字层事件
     * @param textLayer
     */
    void PostAddTextLayer(TextLayer *&textLayer);

    int AddThePreviewLayer2Map();

    int AddThePreviewLayer2MapByJson();


    int UpdatePreViewByJson(const char *layoutJson, const char *fontFolder);

    void CleanPreview();

    void TextLayerTransform(int layerId, float tx, float ty,float canterX, float canterY, float sc, float r);

    /**
     * 设置基础文字属性
     * @param layerId
     * @param subId
     * @param text
     * @param ttfPath
     * @param fonSize
     * @param fontColor
     */
    void SetBasicTextAttributes(int layerId, int subId, const char *text, const char *ttfPath,
                                int fonSize, int fontColor);

    int FindTextInfo(int layerId, int subId, TextLayer *&textLayer, TextInfo *&pTextInfo);

    int FindTextLayer(int layerId, TextLayer *&textLayer);

    int
    AddSimpleSubtext(int layerId, int subTextId, const char *ttfPath, const char *text, int fonSize,
                     int fontColor);

    int RemoveLayer(int layerId);

    void PostRemoveLayer(int layerId);

    void printAll();


    void setStrokeAttributes(int layerId, int subTextId, float lineDistance,
                             float outLineDistance, int outLineColor);

    void UpdateTextLayerFrame(int layerId,int frameIndex);
    void PostUpdateTextLayerFrame(int layerId,int frameIndex);
};


#endif //TESTFREETYPE_TEXT_CONTROL_H

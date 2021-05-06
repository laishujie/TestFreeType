//
// Created by admin on 2021/5/6.
//

#ifndef TESTFREETYPE_TEXT_CONTROL_H
#define TESTFREETYPE_TEXT_CONTROL_H


#include <font-manager.h>
#include <FreeTypeShader.h>
#include "message/handler.h"
#include "egl/egl_core.h"
#include "text_shader.h"

typedef struct {
    char *ttf_file;
    char *text;
} TextInfo;

class text_control : public Handler {
    BufferPool *buffer_pool_;
    ANativeWindow *window_;
    EGLSurface render_surface_;
    EGLCore *core_;
    int surface_width_;
    int surface_height_;
    MessageQueue *message_queue_;
    pthread_t message_queue_thread_;
    ftgl::font_manager_t *fontManager_;
    text_shader *textShader_;
    FreeTypeShader *freeTypeShader;
    TextInfo *current_text_;
public:
    int Init();

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

    void OnGlFontDestroy();

    static void *MessageQueueThread(void *args);

    ftgl::texture_font_t *inset_text(const char *path, const char *text) const;

    text_control();

    ~text_control();

    void ProcessMessage();

    void Draw(char *ttfPath, char *text);
};


#endif //TESTFREETYPE_TEXT_CONTROL_H

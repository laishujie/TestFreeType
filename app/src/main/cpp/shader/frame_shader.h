//
// Created by admin on 2021/5/28.
//

#ifndef TESTFREETYPE_FRAME_SHADER_H
#define TESTFREETYPE_FRAME_SHADER_H


#include "shader_base.h"
#include "text_bean.h"
#include "ImageLoad.h"
class ImageFrameShader : public shader_base {
    GLuint vertexVbo = 0;
    GLuint uvVbo = 0;

    static PointF vertexWithPoint(float x, float y, int width, int height);
public:
    std::deque<FrameCoordinates> allFrameCoordinates;

    void draw();

    void draw(GLuint textureId, float second);

    void onDestroy();

public:
    void Init();

    void OnSurfaceChanged(int width, int height);

    int DrawFrame(int frame, TextInfo *&pInfo);
    int InitFrame(int frame, TextInfo *&pInfo);
};


#endif //TESTFREETYPE_FRAME_SHADER_H

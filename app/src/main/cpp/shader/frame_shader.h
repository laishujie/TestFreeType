//
// Created by admin on 2021/5/28.
//

#ifndef TESTFREETYPE_FRAME_SHADER_H
#define TESTFREETYPE_FRAME_SHADER_H


#include "shader_base.h"

class FrameShader : public shader_base {
    GLuint vertexVbo = 0;
    GLuint uvVbo = 0;

    void Init();

    void draw();

    void draw(GLuint textureId, float second);

    void OnSurfaceChanged(int width, int height);

    void onDestroy();

};


#endif //TESTFREETYPE_FRAME_SHADER_H

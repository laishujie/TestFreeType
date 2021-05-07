//
// Created by admin on 2021/5/7.
//

#ifndef TESTFREETYPE_OUT_SHADER_H
#define TESTFREETYPE_OUT_SHADER_H


#include "shader_base.h"

class out_shader : public shader_base  {

public:
    void onDestroy();

    void Init();

    void OnSurfaceChanged(int width, int height);

    void draw();

    void draw(GLuint textureId);
};


#endif //TESTFREETYPE_OUT_SHADER_H

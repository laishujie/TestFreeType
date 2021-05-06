//
// Created by Lai on 2020/11/30.
//

#ifndef OPENGLDEMO_GLBASE_H
#define OPENGLDEMO_GLBASE_H

//#include "logUtil.h"
#include "comm_gl.hpp"

#define MATH_PI 3.1415926535897932384626433832802

class shader_base {

public:
    shader_base() {};

    ~shader_base() {
        if (glvao != nullptr) {
            delete glvao;
            glvao = nullptr;
        }
        if (glProgram != nullptr) {
            delete glProgram;
            glProgram = nullptr;
        }
        //LOGE("11111","onDestroy shader_base")
    }

    virtual void Init() = 0;

    virtual void draw() = 0;

    virtual void OnSurfaceChanged(int width, int height) = 0;

    virtual void onDestroy() = 0;

protected:
    GLProgram *glProgram = nullptr;
    GLVAO *glvao = nullptr;
    int surfaceWidth, surfaceHeight;

public:
    int getSurfaceWidth() const {
        return surfaceWidth;
    }

    int getSurfaceHeight() const {
        return surfaceHeight;
    }

    GLVAO *getGlVAO() {
        return glvao;
    }
};



#endif //OPENGLDEMO_GLBASE_H


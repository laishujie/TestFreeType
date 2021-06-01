//
// Created by admin on 2021/5/25.
//

#ifndef TESTFREETYPE_MATRIX_SHADER_H
#define TESTFREETYPE_MATRIX_SHADER_H


#include "shader_base.h"

class MatrixShader : public shader_base {
public:
    void onDestroy();

    void Init();

    void OnSurfaceChanged(int width, int height);

    void draw();

    void draw(GLuint textureId);

    void draw(GLuint textureId, float textLeft,float textTop,float textWidth,float textHeight,float cx,float cy,float tx, float ty, float sc, float r);

    void offsetWithMatrixValue(float textLeft,float textTop ,float textWidth,float textHeight,float &tx, float &ty,float &cx, float &cy);

};


#endif //TESTFREETYPE_MATRIX_SHADER_H

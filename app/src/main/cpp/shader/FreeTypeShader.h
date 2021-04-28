//
// Created by admin on 2021/4/27.
//

#ifndef TESTFREETYPE_FREETYPESHADER_H
#define TESTFREETYPE_FREETYPESHADER_H


#include "ShaderBase.h"

class FreeTypeShader : public ShaderBase  {

public:

    void onDestroy();

    void Init();

    void OnSurfaceChanged(int width, int height);

    void draw();

    void draw(GLuint textureId);
};


#endif //TESTFREETYPE_FREETYPESHADER_H

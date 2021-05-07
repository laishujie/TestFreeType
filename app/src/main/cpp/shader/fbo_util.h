//
// Created by admin on 2021/5/7.
//

#ifndef TESTFREETYPE_FBO_UTIL_H
#define TESTFREETYPE_FBO_UTIL_H


#include <GLES3/gl3.h>

typedef struct {
    GLuint frameBuffer;
    GLuint textureId;
} FboInfo;

class fbo_util {

public:
    static FboInfo CreateFbo(int width, int height, GLint format);

    static void BindFbo(GLuint& frameBuffer);

    static void UnBindFbo();
};


#endif //TESTFREETYPE_FBO_UTIL_H

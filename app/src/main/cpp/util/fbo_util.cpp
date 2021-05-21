//
// Created by admin on 2021/5/7.
//

#include "fbo_util.h"

FboInfo fbo_util::CreateFbo(int width, int height, GLint format) {

    FboInfo info = {0, 0};
    glGenTextures(1, &info.textureId);
    glBindTexture(GL_TEXTURE_2D, info.textureId);

    //生成FBO
    glGenFramebuffers(1, &info.frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, info.frameBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //使用纹理作为附件
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, info.textureId, 0);

    //解绑兄弟
    glBindTexture(GL_TEXTURE_2D, 0);

    UnBindFbo();

    return info;
}

void fbo_util::BindFbo(GLuint &frameBuffer) {
    if (frameBuffer != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    }
}

void fbo_util::UnBindFbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

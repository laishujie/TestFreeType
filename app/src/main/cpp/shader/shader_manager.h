//
// Created by admin on 2021/5/7.
//

#ifndef TESTFREETYPE_SHADER_MANAGER_H
#define TESTFREETYPE_SHADER_MANAGER_H


#include <font-manager.h>
#include "text_shader.h"
#include "out_shader.h"
#include "fbo_util.h"

typedef struct {
    char *ttf_file;
    char *text;
    char *outPath;
    int textWidth;
    int textHeight;

} TextInfo;

class TextLayer {
    int id;
public:
    TextLayer() : id(0), frameBuffer(0), textureId(0) {}

    TextLayer(int id, FboInfo fboInfo) : id(id), frameBuffer(fboInfo.textureId),
                                         textureId(fboInfo.frameBuffer) {}
    GLuint textureId;
    GLuint frameBuffer;
    ~TextLayer(){
        glDeleteFramebuffers(1, &frameBuffer);
        glDeleteTextures(1, &textureId);
    }
};

class shader_manager {

private:
    text_shader *textShader_;
    out_shader *outShader_;
    TextLayer *textLayer_;
    ftgl::font_manager_t *fontManager_;

public:
    void drawTextInfo(TextInfo *textInfo);

    void initShader(int width, int height);

    shader_manager();

    ~shader_manager();

    ftgl::texture_font_t *inset_text(const char *path, const char *text) const;
};


#endif //TESTFREETYPE_SHADER_MANAGER_H

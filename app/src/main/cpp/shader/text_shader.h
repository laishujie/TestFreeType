//
// Created by admin on 2021/4/29.
//

#ifndef TESTFREETYPE_TEXT_SHADER_H
#define TESTFREETYPE_TEXT_SHADER_H

#include <texture-font.h>
#include "shader_base.h"
#include <ctype.h>
#include "text_bean.h"

class text_shader : public shader_base {
public:
    GLuint textVbo = 0;
    GLuint uvVbo = 0;
    GLuint ebo = 0;
    float lineSpace = 0.f;

    void Init();

    void draw();

    void OnSurfaceChanged(int width, int height);

    void onDestroy();

    void drawText(GLuint areaTextureId, int textWidth, int textHeight, ftgl::texture_font_t *font,
                  const char *text);

    void drawTextInfo(GLuint areaTextureId, ftgl::texture_font_t *font, TextInfo *&textInfo);
};


#endif //TESTFREETYPE_TEXT_SHADER_H

//
// Created by admin on 2021/4/29.
//

#ifndef TESTFREETYPE_TEXT_SHADER_H
#define TESTFREETYPE_TEXT_SHADER_H

#include <texture-font.h>
#include "shader_base.h"
#include <ctype.h>
#include "text_bean.h"
#include "math.h"


class TextShader {
private:
    int DrawStrokeNormalText(TextInfo *&textInfo, ftgl::texture_font_t *font);

    int DrawShadowText(TextInfo *&textInfo, ftgl::texture_font_t *font);

    GLuint textVbo = 0;
    GLuint uvVbo = 0;
    float lineSpace = 0.f;

    GLProgram *shadowProgram;
    GLVAO *shadowVao;
    GLProgram *glProgram;
    GLVAO *glvao;
public:
    TextShader();

    ~TextShader();

    void Init();

    void DrawTextInfo(ftgl::texture_font_t *font, TextInfo *&textInfo);

    int FillVertex(TextInfo *&textInfo, ftgl::texture_font_t *font);
};


#endif //TESTFREETYPE_TEXT_SHADER_H

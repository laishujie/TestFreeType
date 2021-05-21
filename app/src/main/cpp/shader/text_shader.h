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

    int FillVertex(TextInfo *&textInfo, ftgl::texture_font_t *font);

    int DrawStrokeNormalText(TextInfo *&textInfo,GLuint fontTexture);
    int DrawShadowText(TextInfo *&textInfo,GLuint fontTexture);

};


#endif //TESTFREETYPE_TEXT_SHADER_H

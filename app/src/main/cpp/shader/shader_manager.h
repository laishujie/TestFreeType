//
// Created by admin on 2021/5/7.
//

#ifndef TESTFREETYPE_SHADER_MANAGER_H
#define TESTFREETYPE_SHADER_MANAGER_H

#include <font-manager.h>
#include <map>
#include "text_shader.h"
#include "out_shader.h"
#include "../util/fbo_util.h"
#include "text_bean.h"
#include "FreeTypeShader.h"
#include "matrix_shader.h"
#include "frame_shader.h"

class ShaderManager {

private:
    TextShader *textShader_;
    OutShader *outShader_;
    FreeTypeShader *freeTypeShader;
    ftgl::font_manager_t *fontManager_;
    MatrixShader *matrixShader_;
    ImageFrameShader *imageFrameShader;

public:
    int DrawTextLayer(TextLayer *textLayer);
    int DrawTextInfo(TextLayer *&textLayer,TextInfo *&textInfo);
    int DrawTextImage(TextLayer *&textLayer,TextInfo *&textInfo);

    void InitShader(int width, int height);

    ShaderManager();

    ~ShaderManager();

    ftgl::texture_font_t *InsetTextAndCalculate(TextInfo *&textInfo) const;

    void DrawFrame(int frame, TextInfo *&pInfo);

    void InitTextInfo(TextLayer *pLayer, TextInfo *&pInfo);
};


#endif //TESTFREETYPE_SHADER_MANAGER_H

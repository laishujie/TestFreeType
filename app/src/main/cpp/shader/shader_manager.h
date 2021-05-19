//
// Created by admin on 2021/5/7.
//

#ifndef TESTFREETYPE_SHADER_MANAGER_H
#define TESTFREETYPE_SHADER_MANAGER_H

#include <font-manager.h>
#include <map>
#include "text_shader.h"
#include "out_shader.h"
#include "fbo_util.h"
#include "text_bean.h"
#include "FreeTypeShader.h"

class ShaderManager {

private:
    TextShader *textShader_;
    OutShader *outShader_;
    FreeTypeShader *freeTypeShader;
    ftgl::font_manager_t *fontManager_;

public:
    int DrawTextLayer(TextLayer *textLayer);

    void InitShader(int width, int height);

    ShaderManager();

    ~ShaderManager();

    ftgl::texture_font_t *InsetText(TextInfo *&textInfo) const;
};


#endif //TESTFREETYPE_SHADER_MANAGER_H

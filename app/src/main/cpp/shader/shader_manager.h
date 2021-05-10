//
// Created by admin on 2021/5/7.
//

#ifndef TESTFREETYPE_SHADER_MANAGER_H
#define TESTFREETYPE_SHADER_MANAGER_H


#include <font-manager.h>
#include "text_shader.h"
#include "out_shader.h"
#include "fbo_util.h"
#include "text_bean.h"
#include "FreeTypeShader.h"

class shader_manager {

private:
    text_shader *textShader_;
    out_shader *outShader_;
    FreeTypeShader *freeTypeShader;
    TextLayer *textLayer_;
    ftgl::font_manager_t *fontManager_;

public:
    void drawTextInfo(TextInfo *textInfo);

    void initShader(int width, int height);

    shader_manager();

    ~shader_manager();

    ftgl::texture_font_t *inset_text(const char *path, const char *text,int fontSize) const;
};


#endif //TESTFREETYPE_SHADER_MANAGER_H

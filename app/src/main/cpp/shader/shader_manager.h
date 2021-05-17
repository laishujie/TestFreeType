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

class ShaderManager {

private:
    TextShader *textShader_;
    OutShader *outShader_;
    FreeTypeShader *freeTypeShader;
    TextLayer *previewLayer_;
    ftgl::font_manager_t *fontManager_;

public:
    int DrawTextLayer(TextLayer *textLayer);

    /**
     * 绘制预览textInfo
     * @param textInfo
     */
    void DrawPreViewTextInfo(TextInfo *textInfo);

    void InitShader(int width, int height);

    ShaderManager();

    ~ShaderManager();

    ftgl::texture_font_t *InsetText(TextInfo *&textInfo) const;
};


#endif //TESTFREETYPE_SHADER_MANAGER_H

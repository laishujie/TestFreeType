//
// Created by admin on 2021/4/22.
//

#include "TextEngine.h"

#include <logUtil.h>
#include <utf8-utils.h>
#include <sdf.h>

struct Image {
    unsigned char *data;
    int width, height, bpp;
};

struct Image *imgLoad(const char *path, int requestComp) {
    auto *img = static_cast<Image *>(malloc(sizeof(struct Image)));
    if (img == nullptr) goto error;

    img->data = ImageLoad::load(path, &img->width, &img->height, &img->bpp, requestComp);

    if (img->data == nullptr) goto error;
    return img;

    error:
    if (img != nullptr) {
        free(img->data);
        free(img);
    }
    return nullptr;
}

void imgInverse(struct Image *img) {
    int i;
    for (i = 0; i < img->width * img->height; i++)
        img->data[i] = 255 - img->data[i];
}

struct Image *imgCreate(int w, int h, int bpp, bool ismalloc) {

    auto *img = static_cast<Image *>(malloc(sizeof(struct Image)));
    if (img == nullptr) goto error;
    img->width = w;
    img->height = h;
    img->bpp = bpp;
    if (ismalloc) {
        img->data = static_cast<unsigned char *>(malloc(w * h * bpp));
        if (img->data == nullptr) goto error;
    }
    return img;

    error:
    if (img != nullptr) {
        free(img->data);
        free(img);
    }
    return nullptr;
}

int64_t getPerfTime() {
    struct timeval now;
    gettimeofday(&now, 0);
    return (int64_t) now.tv_sec * 1000000L + (int64_t) now.tv_usec;
}

void imgSave(struct Image *img, const char *path) {
    if (!ImageLoad::savePng(path, img->width, img->height, img->bpp, img->data,
                            img->width * img->bpp)) {
        LOGE("11111", "ERROR: could not write image");
    }
    // stbi_write_png(path, img->width, img->height, img->bpp, img->data, img->width*img->bpp);
}

int deltaTimeUsec(int64_t start, int64_t end) {
    return (int) (end - start);
}


void imgFree(struct Image *img) {
    if (img == nullptr) return;
    free(img->data);
    free(img);
}



void TextEngine::initSdfShaderByBitmap(JNIEnv *env, jobject bitmap, int surfaceWidth,
                                       int surfaceHeight) {
    if (textureImageDemo != nullptr) {
        delete textureImageDemo;
        textureImageDemo = nullptr;
    }

    int iw, ih, n;
    // 加载图片获取宽、高、颜色通道信息
    AndroidBitmapInfo info; // create a AndroidBitmapInfo
    int result;
    // 获取图片信息
    result = AndroidBitmap_getInfo(env, bitmap, &info);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGCATE("AndroidBitmap_getInfo failed, result: %d", result);
        return;
    }
    // 获取像素信息
    unsigned char *data;
    result = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&data));
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGCATE("AndroidBitmap_lockPixels failed, result: %d", result);
        return;
    }
    size_t count = info.stride * info.height;

    auto *resultData = (unsigned char *) malloc(count * sizeof(unsigned char));
    memcpy(resultData, data, count);

    // 像素信息不再使用后需要解除锁定
    result = AndroidBitmap_unlockPixels(env, bitmap);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOGCATE("AndroidBitmap_unlockPixels failed, result: %d", result);
    }
    iw = info.width;
    ih = info.height;


    textureImageDemo = new texture_image_demo();
    textureImageDemo->imgHeight = ih;
    textureImageDemo->imgWidth = iw;
    textureImageDemo->imaData = resultData;
    LOGE("11111", "imageWidth %d imageHeight %d sw %d sh %d textureImageDemo->imaData %d", iw, ih,
         surfaceWidth, surfaceHeight, textureImageDemo->imaData ==
                                      nullptr)

    textureImageDemo->Init();
    textureImageDemo->OnSurfaceChanged(surfaceWidth, surfaceHeight);
    textureImageDemo->draw();

    free(resultData);
}

void TextEngine::initSdfShaderByPath(char *textureImagePath, int surfaceWidth, int surfaceHeight) {
    if (textureImageDemo != nullptr) {
        delete textureImageDemo;
        textureImageDemo = nullptr;
    }
    int iw, ih, n;

    // 加载图片获取宽、高、颜色通道信息
    unsigned char *idata = ImageLoad::load(textureImagePath, &iw, &ih, &n, 4);

    textureImageDemo = new texture_image_demo();
    textureImageDemo->imgHeight = ih;
    textureImageDemo->imgWidth = iw;
    textureImageDemo->imaData = idata;

    LOGE("11111", "imageWidth %d imageHeight %d sw %d sh %d", iw, ih, surfaceWidth, surfaceHeight)

    textureImageDemo->Init();
    textureImageDemo->OnSurfaceChanged(surfaceWidth, surfaceHeight);
    //textureImageDemo->draw();

    delete textureImagePath;
    ImageLoad::imageFree(idata);
}

void TextEngine::onDraw() {
    if (textureImageDemo != nullptr) {
        textureImageDemo->draw();
    }
}

void TextEngine::sdfChange(float distanceMark) {
    if (textureImageDemo != nullptr) {
        textureImageDemo->_DistanceMark = distanceMark;
    }
}

void TextEngine::createSdfTexture1(JNIEnv *env, jstring image_file, jstring image_out_file,
                                   jfloat radius,
                                   jfloat image_aspect) {
    int64_t t0, t1;
    struct Image *img;
    struct Image *img2;

    float imageAspect;
    const char *file = env->GetStringUTFChars(image_file, nullptr);
    const char *out_file = env->GetStringUTFChars(image_out_file, nullptr);

    img = imgLoad(file, 1);
    if (img == nullptr) {
        LOGCATE("Could not load image.\n");
        return;
    }
    imgInverse(img);

    tiny_sdf sdf;
    LOGCATE("Could not load image.1\n");
    img2 = imgCreate(img->width, img->height, 1, true);
    t0 = getPerfTime();
    sdf.transformRasterToSDF(img2->data, img->data, img->width, img->height, radius, 0.25);
    t1 = getPerfTime();

    LOGCATE("sdfBuild(%dx%d) %.1fms\n", img->width, img->height, deltaTimeUsec(t0, t1) / 1000.0f)

    imgSave(img2, out_file);

    imgFree(img);
    imgFree(img2);
    env->ReleaseStringUTFChars(image_file, file);
    env->ReleaseStringUTFChars(image_out_file, out_file);
}


void TextEngine::createSdfTexture2(JNIEnv *env, jstring image_file, jstring image_out_file,
                                   jfloat radius,
                                   jfloat image_aspect) {
    int64_t t0, t1;
    struct Image *img = nullptr;
    struct Image *img2 = nullptr;

    float imageAspect;
    const char *file = env->GetStringUTFChars(image_file, nullptr);
    const char *out_file = env->GetStringUTFChars(image_out_file, nullptr);

    img = imgLoad(file, 1);
    if (img == nullptr) {
        LOGCATE("Could not load image.\n");
        return;
    }
    imgInverse(img);

    imageAspect = img->height * 1.0f / (float) img->width;

    // Build distance field and save it
    img2 = imgCreate(img->width, img->height, 1, true);
    if (img2 == nullptr) {
        return;
    }

    t0 = getPerfTime();

    sdf sdf;

    sdf.sdfBuildDistanceField(img2->data, img2->width, radius, img->data, img->width, img->height,
                              img->width);
    //sdf.make_distance_mapb(img->data,img->width,img->height);
    t1 = getPerfTime();

    LOGCATE("sdfBuild(%dx%d) %.1fms\n", img->width, img->height, deltaTimeUsec(t0, t1) / 1000.0f)

    imgSave(img2, out_file);


    imgFree(img);
    imgFree(img2);
    env->ReleaseStringUTFChars(image_file, file);
    env->ReleaseStringUTFChars(image_out_file, out_file);
}

void TextEngine::initFreeTypeShader(const char *path, int surfaceWidth, int surfaceHeight) {
    if (freeTypeShader != nullptr) {
        delete freeTypeShader;
        freeTypeShader = nullptr;
    }

    freeTypeShader = new FreeTypeShader();
    freeTypeShader->Init();
    freeTypeShader->OnSurfaceChanged(surfaceWidth, surfaceHeight);

    glFontManagerInit();
}

void TextEngine::freeTypeDraw() const {
    if (freeTypeShader == nullptr || fontManager->atlas->id == 0) return;
    freeTypeShader->draw(fontManager->atlas->id);
}

TextEngine::~TextEngine() {
    LOGE("11111", "~~TextEngine")
    glDeleteTextures(1, &fontManager->atlas->id);
    fontManager->atlas->id = 0;
    font_manager_delete(fontManager);
}

ftgl::texture_font_t *TextEngine::insetText(const char *path, const char *text) const {
    //返回字体表
    ftgl::texture_font_t *pFont = ftgl::font_manager_get_from_filename(fontManager, path, 32);
    pFont->rendermode = ftgl::RENDER_NORMAL;
    //获取对应得文本
    int i = ftgl::texture_font_load_glyphs_isOk(pFont, text);

    if (i == 1) {
        glBindTexture(GL_TEXTURE_2D, fontManager->atlas->id);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fontManager->atlas->width,
                        fontManager->atlas->height,
                        GL_RED, GL_UNSIGNED_BYTE, fontManager->atlas->data);
    }/*else{
        LOGCATE("无需更新");
    }*/
    return pFont;
}

char *myFormatStringByFun(char *format, ...) {
    va_list list;
    //1. 先获取格式化后字符串的长度
    va_start(list, format);
    int size = vsnprintf(nullptr, 0, format, list);
    va_end(list);
    if (size <= 0) {
        return nullptr;
    }
    size++;

    //2. 复位va_list，将格式化字符串写入到buf
    va_start(list, format);
    char *buf = (char *) malloc(size);
    vsnprintf(buf, size, format, list);
    va_end(list);
    return buf;
}

const char *TextEngine::getTextInfo(const char *path, const char *text) const {
    ftgl::texture_font_t *pFont = ftgl::font_manager_get_from_filename(fontManager, path, 32);

    ftgl::texture_glyph_t *pGlyph = ftgl::texture_font_find_glyph(pFont, text);
    if (pGlyph == nullptr) {
        return "字形纹理不存在";
    }
    float d = ftgl::texture_glyph_get_kerning(pGlyph, text);

    LOGCATE("\nkerning = %f font->descender= %f\n "
            "pGlyph->advance_x =%f pGlyph->advance_y =%f \n"
            "glyph->offset_x =%d glyph->offset_y =%d \n"
            "glyph->width =%d glyph->height =%d \n "
            "glyph->s0 =%f glyph->t0 =%f \n "
            "glyph->s1 =%f  glyph->t1 =%f",
            d, pFont->descender,
            pGlyph->advance_x, pGlyph->advance_y,
            pGlyph->offset_x, pGlyph->offset_y,
            pGlyph->width, pGlyph->height,
            pGlyph->s0, pGlyph->t0,
            pGlyph->s1, pGlyph->t1)

    char *text2 = "\nkerning = %f font->descender= %f\n pGlyph->advance_x =%f pGlyph->advance_y =%f \n glyph->offset_x =%d glyph->offset_y =%d \nglyph->width =%d glyph->height =%d \n glyph->s0 =%f glyph->t0 =%f \n glyph->s1 =%f  glyph->t1 =%f";


    return myFormatStringByFun(text2, d, pFont->descender,
                               pGlyph->advance_x, pGlyph->advance_y,
                               pGlyph->offset_x, pGlyph->offset_y,
                               pGlyph->width, pGlyph->height,
                               pGlyph->s0, pGlyph->t0,
                               pGlyph->s1, pGlyph->t1);
}

void TextEngine::glInitTextShader(int surfaceWidth, int surfaceHeight) {
    if (textShader != nullptr) {
        delete textShader;
        textShader = nullptr;
    }

    textShader = new text_shader();
    textShader->Init();
    //textShader->OnSurfaceChanged(surfaceWidth, surfaceHeight);
    glFontManagerInit();
}

void TextEngine::glFontManagerInit() const {
    if (fontManager->atlas->id != 0) return;
    glGenTextures(1, &fontManager->atlas->id);
    glBindTexture(GL_TEXTURE_2D, fontManager->atlas->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontManager->atlas->width, fontManager->atlas->height,
                 0, GL_RED, GL_UNSIGNED_BYTE, fontManager->atlas->data);
    // 设置纹理选项
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void TextEngine::glRenderText(const char *path, const char *text) const {
    ftgl::texture_font_t *pFont = insetText(path, text);
   // textShader->drawText(fontManager->atlas->id, fontManager->atlas->width,fontManager->atlas->height,pFont, text);
}




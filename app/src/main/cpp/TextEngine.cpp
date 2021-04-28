//
// Created by admin on 2021/4/22.
//

#include "TextEngine.h"

#include <logUtil.h>
#include <sdf.h>
#include <utf8-utils.h>

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


    textureImageDemo = new TextureImageDemo();
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

    textureImageDemo = new TextureImageDemo();
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

    TinySDF sdf;
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
        LOGE("11111", "delete freeTypeShader")

        delete freeTypeShader;

        if (freeTextureId != 0) {
            glDeleteTextures(1, &freeTextureId);
            freeTextureId = 0;
        }

        freeTypeShader = nullptr;
    }


    freeTypeShader = new FreeTypeShader();
    freeTypeShader->Init();
    freeTypeShader->OnSurfaceChanged(surfaceWidth, surfaceHeight);

    //Image* img = imgLoad(path, 1);

    glGenTextures(1, &freeTextureId);
    freeActiveTextureIndex = GL_TEXTURE0;
    if (fontManager == nullptr) {
        fontManager = ftgl::font_manager_new(512, 512, 1);
        LOGE("11111", "init font_manager_new")
    }

    ftgl::font_manager_get_from_filename(fontManager, path, 32);

    glActiveTexture(freeActiveTextureIndex);
    glBindTexture(GL_TEXTURE_2D, freeTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontManager->atlas->width, fontManager->atlas->height,
                 0, GL_RED, GL_UNSIGNED_BYTE, fontManager->atlas->data);

    // 设置纹理选项
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    // imgFree(img);
}

void TextEngine::freeTypeDraw() const {
    if (freeTypeShader == nullptr || freeTextureId == 0) return;
// Disable the depth test since the text is being rendered in 2D
    freeTypeShader->draw(freeTextureId);
}

TextEngine::~TextEngine() {
    LOGE("11111", "~~TextEngine")
    glDeleteTextures(1, &fontManager->atlas->id);
    fontManager->atlas->id = 0;
    font_manager_delete(fontManager);
}

void TextEngine::insetText(const char *path, const char *text) const {
    //返回字体表
    ftgl::texture_font_t *pFont = ftgl::font_manager_get_from_filename(fontManager, path, 32);
    pFont->rendermode = ftgl::RENDER_NORMAL;
    //获取对应得文本
    ftgl::texture_font_load_glyphs(pFont, text);
    glBindTexture(GL_TEXTURE_2D, freeTextureId);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fontManager->atlas->width, fontManager->atlas->height,
                    GL_RED, GL_UNSIGNED_BYTE, fontManager->atlas->data);

//    for (int i = 0; i < strlen(text); i +=ftgl::utf8_surrogate_len(text + i)) {
//        ftgl::texture_glyph_t *pGlyph = texture_font_get_glyph(pFont, text + i);
//        if (pGlyph!= nullptr) {
//
//        }
//    }

    //freeTypeDraw();
}



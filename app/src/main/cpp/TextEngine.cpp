//
// Created by admin on 2021/4/22.
//

#include "TextEngine.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include <logUtil.h>
#include <sdf.h>

struct Image {
    unsigned char *data;
    int width, height, bpp;
};

struct Image *imgLoad(const char *path, int bpp) {
    auto *img = static_cast<Image *>(malloc(sizeof(struct Image)));
    if (img == nullptr) goto error;
    img->data = stbi_load(path, &img->width, &img->height, &img->bpp, bpp);

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
    if (!stbi_write_png(path, img->width, img->height, img->bpp, img->data,
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

void TextEngine::initSdfShaderByBitmap(JNIEnv *env, jobject bitmap, int surfaceWidth, int surfaceHeight) {
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

    auto *  resultData = (unsigned char *) malloc(count * sizeof(unsigned char));
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
    textureImageDemo->imaData=resultData;
    LOGE("11111","imageWidth %d imageHeight %d sw %d sh %d textureImageDemo->imaData %d",iw,ih,surfaceWidth,surfaceHeight,textureImageDemo->imaData==
                                                                                                                          nullptr)

    textureImageDemo->Init();
    textureImageDemo->OnSurfaceChanged(surfaceWidth,surfaceHeight);
    textureImageDemo->draw();

    free(resultData);
}

void TextEngine::initSdfShaderByPath(char *textureImagePath,int surfaceWidth,int surfaceHeight) {
    if (textureImageDemo != nullptr) {
        delete textureImageDemo;
        textureImageDemo = nullptr;
    }
    int iw, ih, n;

    // 加载图片获取宽、高、颜色通道信息
    unsigned char *idata = stbi_load(textureImagePath, &iw, &ih, &n, STBI_rgb_alpha);

    textureImageDemo = new TextureImageDemo();
    textureImageDemo->imgHeight = ih;
    textureImageDemo->imgWidth = iw;
    textureImageDemo->imaData = idata;

    LOGE("11111","imageWidth %d imageHeight %d sw %d sh %d",iw,ih,surfaceWidth,surfaceHeight)

    textureImageDemo->Init();
    textureImageDemo->OnSurfaceChanged(surfaceWidth,surfaceHeight);
    //textureImageDemo->draw();

    delete textureImagePath;
    stbi_image_free(idata);
}

void TextEngine::onDraw() {
    if(textureImageDemo!= nullptr){
        textureImageDemo->draw();
    }
}

void TextEngine::sdfChange(float distanceMark) {
    if(textureImageDemo!= nullptr) {
       textureImageDemo->_DistanceMark = distanceMark;
    }
}

void TextEngine::createSdfTexture1(JNIEnv *env,jstring image_file, jstring image_out_file, jfloat radius,
                                   jfloat image_aspect) {
     int64_t t0, t1;
    struct Image* img;
    struct Image* img2;

    float imageAspect;
    const char *file = env->GetStringUTFChars(image_file, nullptr);
    const char *out_file = env->GetStringUTFChars(image_out_file, nullptr);

    img = imgLoad(file, 1);
    if (img == nullptr) {
        LOGCATE("Could not load image.\n");
        return ;
    }
    imgInverse(img);

    TinySDF sdf;
    LOGCATE("Could not load image.1\n");
    img2 = imgCreate(img->width, img->height, 1,true);
    t0 = getPerfTime();
    sdf.transformRasterToSDF(img2->data,img->data, img->width,img->height, radius, 0.25);
    t1 = getPerfTime();

    LOGCATE("sdfBuild(%dx%d) %.1fms\n", img->width, img->height, deltaTimeUsec(t0, t1) / 1000.0f)

    imgSave(img2, out_file);

    imgFree(img);
    imgFree(img2);
    env->ReleaseStringUTFChars(image_file,file);
    env->ReleaseStringUTFChars(image_out_file,out_file);
}


void TextEngine::createSdfTexture2(JNIEnv *env,jstring image_file, jstring image_out_file, jfloat radius,
                                   jfloat image_aspect) {
        int64_t t0, t1;
    struct Image* img = nullptr;
    struct Image* img2 = nullptr;

    float imageAspect;
    const char *file = env->GetStringUTFChars(image_file, nullptr);
    const char *out_file = env->GetStringUTFChars(image_out_file, nullptr);

    img = imgLoad(file, 1);
    if (img == nullptr) {
        LOGCATE("Could not load image.\n");
        return ;
    }
   imgInverse(img);

    imageAspect = img->height * 1.0f / (float) img->width;

    // Build distance field and save it
    img2 = imgCreate(img->width, img->height, 1,true);
    if (img2 == nullptr) {
        return ;
    }

    t0 = getPerfTime();

    sdf sdf;

    sdf.sdfBuildDistanceField(img2->data, img2->width, radius, img->data, img->width, img->height, img->width);
    //sdf.make_distance_mapb(img->data,img->width,img->height);
    t1 = getPerfTime();

    LOGCATE("sdfBuild(%dx%d) %.1fms\n", img->width, img->height, deltaTimeUsec(t0, t1) / 1000.0f)

    imgSave(img2, out_file);


    imgFree(img);
    imgFree(img2);
    env->ReleaseStringUTFChars(image_file,file);
    env->ReleaseStringUTFChars(image_out_file,out_file);
}


//
// Created by admin on 2021/4/26.
//

#include "ImageLoad.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

ImageLoad::~ImageLoad() {
    if (_imgData)
        stbi_image_free(_imgData);
}


unsigned char *ImageLoad::load(const char *filename, int *iw, int *ih, int *comp, int req_comp) {
    return stbi_load(filename, iw, ih, comp, req_comp);
}

int ImageLoad::savePng(const char *filename, int x, int y, int comp, const void *data,
                       int stride_bytes) {
    return stbi_write_png(filename, x, y, comp, data, stride_bytes);
}

void ImageLoad::imageFree(void *retval_from_stbi_load) {
    stbi_image_free(retval_from_stbi_load);
}

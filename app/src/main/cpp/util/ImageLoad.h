//
// Created by admin on 2021/4/26.
//

#ifndef TESTFREETYPE_IMAGELOAD_H
#define TESTFREETYPE_IMAGELOAD_H


class ImageLoad {
    ImageLoad() : width(0), height(0), _imgData(nullptr) {}

    ~ImageLoad();

public:
    unsigned char* getData() const { return _imgData; }

    static unsigned char * load(char const *filename, int *x, int *y, int *comp, int req_comp);
    static int savePng(char const *filename, int x, int y, int comp, const void *data, int stride_bytes);
    static void imageFree(void *retval_from_stbi_load);
protected:
    int width;
    int height;
    unsigned char *_imgData;
};


#endif //TESTFREETYPE_IMAGELOAD_H

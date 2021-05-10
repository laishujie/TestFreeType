//
// Created by admin on 2021/5/8.
//

#ifndef TESTFREETYPE_TEXT_BEAN_H
#define TESTFREETYPE_TEXT_BEAN_H

#include <GLES3/gl3.h>

class TextInfo {

public:
    char *ttf_file;
    char *text;
    char *outPath;
    int textWidth;
    int textHeight;
    bool isHorizontal;
    int spacing;
    int lineSpacing;
    int fontSize;
    int fontColor;
    float distanceMark;
    float outlineDistanceMark;
    TextInfo() : ttf_file(nullptr), text(nullptr), outPath(nullptr),
                 textWidth(0), textHeight(0), isHorizontal(true), spacing(0), lineSpacing(0),
                 fontSize(72), fontColor(0), distanceMark(0.5f),outlineDistanceMark(0.5) {
    }

    ~TextInfo() {
        if (ttf_file != nullptr) {
            delete ttf_file;
            ttf_file = nullptr;
        }
        if (text != nullptr) {
            delete text;
            text = nullptr;
        }
        if (outPath != nullptr) {
            delete outPath;
            outPath = nullptr;
        }
    }


};

typedef struct {
    GLuint frameBuffer;
    GLuint textureId;
} FboInfo;

class TextLayer {
    int id;
public:
    TextLayer() : id(0), frameBuffer(0), textureId(0) {}

    TextLayer(int id, FboInfo fboInfo) : id(id), frameBuffer(fboInfo.textureId),
                                         textureId(fboInfo.frameBuffer) {}

    GLuint textureId;
    GLuint frameBuffer;

    ~TextLayer() {
        glDeleteFramebuffers(1, &frameBuffer);
        glDeleteTextures(1, &textureId);
    }
};


#endif //TESTFREETYPE_TEXT_BEAN_H

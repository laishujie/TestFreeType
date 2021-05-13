//
// Created by admin on 2021/5/8.
//

#ifndef TESTFREETYPE_TEXT_BEAN_H
#define TESTFREETYPE_TEXT_BEAN_H

#include <GLES3/gl3.h>
#include <string>
#include <deque>

class TextInfo {

public:
    std::string ttf_file;
    std::string text;
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
    int outLineColor;
    float shadowDistance;
    float shadowAlpha;
    int shadowColor;
    int shadowAngle;
    float x, y;
    bool isFromTemplate;
    TextInfo() : ttf_file(), text(), outPath(nullptr),
                 textWidth(0), textHeight(0), isHorizontal(true), spacing(0), lineSpacing(0),
                 fontSize(72), fontColor(0xFFFFFFFF), distanceMark(0.5f), outlineDistanceMark(0.5),
                 outLineColor(0), shadowDistance(0), shadowAlpha(0.5f), x(0.), y(0.),
                 shadowColor(0), shadowAngle(0),isFromTemplate(false){
    }

    ~TextInfo() {
        /* if (ttf_file != nullptr) {
             delete[] ttf_file;
             ttf_file = nullptr;
         }
         if (text != nullptr) {
             delete text;
             text = nullptr;
         }*/
        if (outPath != nullptr) {
            delete[] outPath;
            outPath = nullptr;
        }
    }
};

typedef struct {
    GLuint frameBuffer;
    GLuint textureId;
} FboInfo;

class TextLayer {
public:
    TextLayer() : id(0), frameBuffer(0), textureId(0), text_deque() {}

    TextLayer(int id, FboInfo fboInfo) : id(id), frameBuffer(fboInfo.textureId),
                                         textureId(fboInfo.frameBuffer), text_deque() {}

    //容器信息
    std::deque<TextInfo *> text_deque;
    GLuint textureId;
    GLuint frameBuffer;
    int id;
    ~TextLayer() {
        glDeleteFramebuffers(1, &frameBuffer);
        glDeleteTextures(1, &textureId);

        for (auto clip : text_deque) {
            delete clip;
        }
        text_deque.clear();
    }
};


#endif //TESTFREETYPE_TEXT_BEAN_H

//
// Created by admin on 2021/5/8.
//

#ifndef TESTFREETYPE_TEXT_BEAN_H
#define TESTFREETYPE_TEXT_BEAN_H

#include <GLES3/gl3.h>
#include <string>
#include <deque>
#include <vector>

struct PointF {
    float x;
    float y;
};

class TextArea {
public:
    float left, top, right, bottom;

    float getWidth() {
        return right - left;
    }
    float getHeight() {
        return bottom - top;
    }

    void reset() {
        left = 0.f;
        top = 0.f;
        right = 0.f;
        bottom = 0.f;
    }
};


typedef struct FrameCoordinates {
    int x, y, w, h,i;
} FrameCoordinates;


class TextImage {
public:
    TextImage() : frameImg(), textureId(0), frameCoordinates(),size({0.f,0.f}) {

    }

    std::string frameImg;
    GLuint textureId;
    PointF size;
    std::deque<FrameCoordinates> frameCoordinates;
};


class TextInfo {
private:
    int lastFontSize;
    unsigned int lastTextSize;
    std::string last_ttf_file;
public:
    int id;
    std::string ttf_file;
    std::string text;
    char *outPath;
    int surfaceWidth;
    int surfaceHeight;
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
    float offset_x, offset_y;
    bool isFromTemplate;
    float textWidth;
    float textHeight;
    TextArea area;
    int indexVertex;
    bool isTextImage;
    std::string file;
    std::deque<TextImage> textImages;
    bool isInit;

    bool isCreateVertexAndSet() {
        bool isCreate = isSameText();
        if (isCreate) {
            lastFontSize = fontSize;
            lastTextSize = text.size();
            last_ttf_file = ttf_file;
        }
        return isCreate;
    }

    bool isSameText() {
        return lastFontSize != fontSize || lastTextSize != text.size() ||
               last_ttf_file != ttf_file;
    }


    TextInfo() : id(0), area(), ttf_file(), text(), outPath(nullptr),
                 surfaceWidth(0), surfaceHeight(0), isHorizontal(true), spacing(0), lineSpacing(0),
                 fontSize(72), fontColor(0xFFFFFFFF), distanceMark(0.465f), outlineDistanceMark(0.5),
                 outLineColor(0), shadowDistance(0), shadowAlpha(0.5f), offset_x(0.), offset_y(0.),
                 shadowColor(0), shadowAngle(0), isFromTemplate(false), textWidth(0.f),
                 textHeight(0.f), lastFontSize(0), lastTextSize(0), indexVertex(0),
                 last_ttf_file(), isTextImage(false), textImages(),isInit(false) {
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
    TextLayer() : id(0), frameBuffer(0), textureId(0), text_deque(), isTemplate(false), textArea(),
                  isChangeTextArea(false), tx(0.f), ty(0.f),cx(0.f),cy(0.f), sc(1.f), r(0.f), applyMatrix(false),
                  isFristCreate(true), isDraw(true),templateFolder(),frameIndex(75) {}

    //层id
    int id;
    //容器信息
    std::deque<TextInfo *> text_deque;
    //层纹理
    GLuint textureId;
    //当前层fbo
    GLuint frameBuffer;
    //当前层大小
    TextArea textArea;
    //是否模板
    bool isTemplate;
    //是否改变文本区域
    bool isChangeTextArea;
    //层平移信息
    float tx, ty, sc, r,cx,cy;
    //是否应用变换信息
    bool applyMatrix;
    //是否第一次被创建
    bool isFristCreate;
    bool isDraw;
    int frameIndex;
    std::string templateFolder;

    ~TextLayer() {
        glDeleteFramebuffers(1, &frameBuffer);
        glDeleteTextures(1, &textureId);

        for (auto &clip : text_deque) {
            delete clip;
            clip = nullptr;
        }

        text_deque.clear();
    }
};


#endif //TESTFREETYPE_TEXT_BEAN_H

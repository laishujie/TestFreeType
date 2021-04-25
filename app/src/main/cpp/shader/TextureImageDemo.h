//
// Created by Lai on 2021/2/4.
//

#ifndef WHITEBOARD_TEXTUREIMAGEDEMO_H
#define WHITEBOARD_TEXTUREIMAGEDEMO_H

#include "ShaderBase.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.inl>

class TextureImageDemo : public ShaderBase {
    glm::mat4 mInitMatrix;
    GLuint textureId;
    unsigned int lineVBO;
    unsigned  int lineVAO;

public:
    void onDestroy();

    void Init();

    void OnSurfaceChanged(int width, int height);
    int imgWidth;
    int imgHeight;

    //模拟一张简答的图片
    //RGB RGB RGB RGB
    unsigned char* imaData= nullptr;
    void draw();


    void change(float x,float y,float z);

    ~TextureImageDemo();

    float  _DistanceMark =0.25;
};


#endif //WHITEBOARD_TEXTUREIMAGEDEMO_H

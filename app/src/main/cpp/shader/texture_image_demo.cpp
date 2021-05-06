//
// Created by Lai on 2021/2/4.
//

#include "texture_image_demo.h"
#include "logUtil.h"


void texture_image_demo::Init() {

    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "layout(location = 1) in vec3 uvPos;\n"
            "uniform mat4 uMatrix;\n"
            "out vec3 outUvPos;\n"
            "void main()                              \n"
            "{                                        \n"
            "   gl_Position =  vPosition;              \n"
            "   outUvPos = uvPos;              \n"
            "}                                        \n";

    char fboShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "uniform sampler2D textureMap;\n"
            "in vec3 outUvPos;\n"

            "#define AZUR  vec3(0.0, 0.5, 1.0)\n"
            "#define PURPLE vec3(1.0, 0.0, 0.5)\n"
            "#define MAGENTA vec3(1.0, 0.0, 1.0)\n"


            "uniform vec4 _OutlineColor;"
            "uniform vec4 _ShadowColor;"
            "vec4 _GlowColor = vec4(MAGENTA,1. );"
            "uniform float _OutlineDistanceMark;"
            "float _SmoothDelta =0.25;"
            "float _GlowDistanceMark =0.25;"
            "vec4 _MainColor = vec4(1.0, 1.0, 1.0, 1.0);"
            "uniform float _DistanceMark;"
            "uniform float _ShadowOffsetPixie;"
            "uniform int _ShadowAngleMark;"

            "uniform float _ShadowAlpha;"

            "mat2 scale(vec2 _scale){\n"
            "    return mat2(_scale.x,0.0,\n"
            "                0.0,_scale.y);\n"
            "}"

    "void main(){"
        "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
/*
            "uv-=vec2(0.5); "
            "uv *= scale(vec2(1.,0.9));"
            "uv+=vec2(0.5);"*/

        "vec4 col = texture(textureMap,uv); \n"

        "float distance = col.r;"
        //r分量到了多少需要进行描边绘制
        "if(distance<_DistanceMark){"
            //实体描边 _OutlineDistanceMark 分量多少算描边 如果_X 大于或等于_Y，返回 1;否则 0，
            "vec4 stokeColor = _OutlineColor;"
            "stokeColor.a = step(_OutlineDistanceMark, distance);"
            "if(stokeColor.a!=0.){"
            //属于描边范围直接绘制
                "fragColor = stokeColor;"
            "}else{"
            //属于外部范围，进行阴影绘制 这个浮点数是 pi / 180，角度转弧度
            "float rad = radians(float(_ShadowAngleMark));"
            //取得纹理大小
            "ivec2 size = textureSize(textureMap, 0);\n"
            //获取一个像素所占的值//单位坐标
            "vec2 unit = 1.0 / vec2(float(size.x),float(size.y));\n"
            // 根据半径可获取圆点偏移坐标点  //偏移量
            "vec2 offset = vec2(_ShadowOffsetPixie * cos(rad) * unit.x, _ShadowOffsetPixie * sin(rad) * unit.y);"
            //坐标点采集R分量
            "float offsetDistance = texture(textureMap, uv + offset).r;"
            "if(offsetDistance !=0.){"
            //进行线性插值
            "float a = smoothstep(0., _ShadowAlpha, offsetDistance);"
            "vec4 shadowColor = vec4(_ShadowColor.rgb,a*_ShadowColor.a);"
            "fragColor = shadowColor;"
           // "for(int i=0;i<30;i++){"
            //"}"
          "}"
         "}"
    "}else{"
            //文字颜色
            "fragColor = _MainColor;"
          "}"
 "}";






//            "void main(){"
//            "    vec2 sc = vec2(1.1,1.14);"
//            "    vec2 tx = vec2(0.,-0.5);"
//            "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
//            "vec4 col = texture(textureMap,uv); \n"
//
//            //获取红色的分量
//            "float distance = col.r;"
//
//            "_OutlineColor.a = smoothstep(_OutlineDistanceMark - _SmoothDelta, _OutlineDistanceMark + _SmoothDelta, distance);"
//
//            //限定范围进行插值，。超过直接抛弃
//            "float finalalpha = smoothstep(_DistanceMark - _SmoothDelta, _DistanceMark + _SmoothDelta, distance);"
//
//            "col.rgb = _MainColor.rgb;"
//
//            "col.a = finalalpha;"
//    "}";

           /*
            *
            * 描边
            * "void main()                                  \n"
            "{                                            \n"
            "vec2 uv = vec2(outUvPos.x,outUvPos.y); \n"
            "vec4 col = texture(textureMap,uv); \n"
            " const vec3 glyphcolor = vec3(1.0, 0.1255, 0.1255);"
            "const vec3 glowcolor = vec3(1.0, 1.0, 1.0);  "
            "    float dist = texture(textureMap, uv).r;  "
            "    float delta = 0.1;  "
            "float finalalpha = smoothstep(0.5-delta, 0.5+delta, dist);  "
            "vec4 clr = vec4(glyphcolor, dist);  "
            " int glow = 1;  "
            "if(glow == 1) {  "
            "   clr.rgb = mix(glowcolor, glyphcolor, finalalpha);  "
            "    float alpha = smoothstep(0.0, 0.5+(0.6*(1.*0.5)), sqrt(dist));  "
            "    clr.a = alpha;  "
            "    fragColor = clr; "
            "}                                            \n"
    "}";*/

    glProgram = new GLProgram(vShaderStr, fboShaderStr);
    glvao = new GLVAO();

    GLfloat rectangleVertices[] = {
            -1.0f, 1.0f, 0.0f,// 左上角
            1.0f, 1.0f, 0.0f,//右上角
            1.0f, -1.0f, 0.0f,//右下角
            -1.0f, -1.0f, 0.0f};//左下角

    //y纹理颠倒
    GLfloat uv[] = {
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 1.0, 0.0,
            0.0f, 1.0, 0, 0
    };


    unsigned int index[] = {0, 1, 2, 2, 3, 0};


    glvao->AddVertex3D(rectangleVertices, 4, 0);
    glvao->AddVertex3D(uv, 4, 1);
    glvao->setIndex(index, 6);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //LOGCATE("===== textureId %d", textureId);
    //像素偏移1
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,imaData);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLfloat lineVertices[] = {
            -1.0f, 0.0f, 0.0f,
            1.0, -0, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,

            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, 1.0f
    };

    glLineWidth(10.0f);

    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    //绑定对象 GL_ARRAY_BUFFER：可以用来保存glVertexAttribPointer()设置的顶点数组数据
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    //为当前VBO塞点数据 GL_STATIC_DRAW 表示数据不会或几乎不会改变。
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

void texture_image_demo::draw() {
    glClearColor(0.8, 0.8, 0.8, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glProgram->useProgram();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    GLint textureIndex = glGetUniformLocation(glProgram->program, "textureMap");
    //激活纹理单元编号和纹理单元对应
    glActiveTexture(GL_TEXTURE0);
    //textureId 绑定到GL_TEXTURE0纹理单元上
    glBindTexture(GL_TEXTURE_2D, textureId);
    //第二个参数传的是纹理单元
    glUniform1i(textureIndex, 0);



    GLint distanceMarkIndex = glGetUniformLocation(glProgram->program, "_DistanceMark");
    glUniform1f(distanceMarkIndex, _DistanceMark);
    GLint outLineMarkIndex = glGetUniformLocation(glProgram->program, "_OutlineDistanceMark");
    glUniform1f(outLineMarkIndex, _OutlineMark);

    GLint offsetMarkIndex = glGetUniformLocation(glProgram->program, "_ShadowOffsetPixie");
    glUniform1f(offsetMarkIndex, _ShadowDistanceMark);

    GLint shadowAngleMarkIndex = glGetUniformLocation(glProgram->program, "_ShadowAngleMark");
    glUniform1i(shadowAngleMarkIndex, _ShadowAngleMark);


    GLint testShadowOffsetPixie1 = glGetUniformLocation(glProgram->program, "_ShadowAlpha");
    glUniform1f(testShadowOffsetPixie1, _ShadowAlpha);

    GLint vertexColorLocation = glGetUniformLocation(glProgram->program, "_OutlineColor");

//    float r = color_util::getColorR(_StokeLineColor);
//    float g = color_util::getColorG(_StokeLineColor);
//    float b = color_util::getColorB(_StokeLineColor);
//    LOGCATE("r %f g %f b %f",r,g,b)
    glUniform4f(vertexColorLocation, color_util::getColorR(_StokeLineColor), color_util::getColorG(_StokeLineColor), color_util::getColorB(_StokeLineColor), 1.0f);

    GLint shadowColorLocation = glGetUniformLocation(glProgram->program, "_ShadowColor");
    glUniform4f(shadowColorLocation, color_util::getColorR(_ShadowColor), color_util::getColorG(_ShadowColor), color_util::getColorB(_ShadowColor), 1.0f);





    //GLint transformLoc = glGetUniformLocation(glProgram->program, "uMatrix");
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mInitMatrix));

    glvao->BindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


    glBindVertexArray(lineVAO);
    glDrawArrays(GL_LINES, 0, 6);
}

void texture_image_demo::OnSurfaceChanged(int width, int height) {
    glProgram->OnSurfaceChanged(0, 0, width, height);

    surfaceWidth = width;
    surfaceHeight = height;

//    float left = -1.0f;
//    float right = 1.0f;
//    float bottom = -1.0f;
//    float top = 1.0f;
//    float aspectTexture = 441. / (float) 105;
//    float aspectPlane = surfaceWidth / (float) surfaceHeight;
//
////width 1080 height 1680 aspectTexture 0.642857 aspectPlane 1.000000
////1080
////fitCenter
//    //1. 纹理比例 > 投影平面比例 固定宽度修改高度
//    if (aspectTexture > aspectPlane) {
//        top = 1 / aspectPlane * aspectTexture;
//        bottom = -top;
//    } else {
//        left = -aspectPlane / aspectTexture;
//        right = -left;
//    }
//
//    //通过正交矩阵裁剪显示范围
//    mInitMatrix = glm::ortho(left, right, bottom, top);
}

void texture_image_demo::change(float x, float y, float z) {
    _DistanceMark = x;

    draw();
}

texture_image_demo::~texture_image_demo() {
    texture_image_demo::onDestroy();
}

void texture_image_demo::onDestroy() {
    if (textureId > 0) {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
    if (imaData != nullptr) {
        free(imaData);
        imaData = nullptr;
    }
}

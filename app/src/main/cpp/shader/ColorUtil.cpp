//
// Created by admin on 2021/4/25.
//

#include "ColorUtil.h"

float ColorUtil::getColorR( int color) {
    return ((color & 0x00FF0000) >> 0x10 ) / 255.0f;
}

float ColorUtil::getColorG(int color) {
    return ((color & 0x0000FF00) >> 0x08 ) / 255.0f;

}

float ColorUtil::getColorB(int color) {
    return (color & 0x000000FF) / 255.0f;
}

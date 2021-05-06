//
// Created by admin on 2021/4/25.
//

#include "color_util.h"

float color_util::getColorR(int color) {
    return ((color & 0x00FF0000) >> 0x10 ) / 255.0f;
}

float color_util::getColorG(int color) {
    return ((color & 0x0000FF00) >> 0x08 ) / 255.0f;

}

float color_util::getColorB(int color) {
    return (color & 0x000000FF) / 255.0f;
}

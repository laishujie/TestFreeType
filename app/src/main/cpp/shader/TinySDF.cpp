//
// Created by admin on 2021/4/22.
//

#include "TinySDF.h"
static const double INF = 1e20;
void TinySDF::edt(std::vector<double> &data, uint32_t width, uint32_t height, std::vector<double> &f, std::vector<double> &d, std::vector<int16_t> &v, std::vector<double> &z) {
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < height; y++) {
            f[y] = data[y * width + x];
        }
        edt1d(f, d, v, z, height);
        for (uint32_t y = 0; y < height; y++) {
            data[y * width + x] = d[y];
        }
    }
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            f[x] = data[y * width + x];
        }
        edt1d(f, d, v, z, width);
        for (uint32_t x = 0; x < width; x++) {
            data[y * width + x] = sqrt(d[x]);
        }
    }
}

void TinySDF::edt1d(std::vector<double> &f, std::vector<double> &d, std::vector<int16_t> &v, std::vector<double> &z, uint32_t n) {
    v[0] = 0;
    z[0] = -INF;
    z[1] = +INF;

    for (uint32_t q = 1, k = 0; q < n; q++) {
        double s = ((f[q] + q * q) - (f[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
        while (s <= z[k]) {
            k--;
            s = ((f[q] + q * q) - (f[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
        }
        k++;
        v[k] = q;
        z[k] = s;
        z[k + 1] = +INF;
    }

    for (uint32_t q = 0, k = 0; q < n; q++) {
        while (z[k + 1] < q)
            k++;
        d[q] = (q - v[k]) * (q - v[k]) + f[v[k]];
    }
}

void TinySDF::transformRasterToSDF(unsigned char* data,const unsigned char *img,unsigned int width,unsigned int height,double radius, double cutoff){
    uint32_t size = width * height;
    uint32_t maxDimension = std::max(width, height);

    // temporary arrays for the distance transform
    std::vector<double> gridOuter(size);
    std::vector<double> gridInner(size);
    std::vector<double> f(maxDimension);
    std::vector<double> d(maxDimension);
    std::vector<double> z(maxDimension + 1);
    std::vector<int16_t> v(maxDimension);

    for (uint32_t i = 0; i < size; i++) {
        double a = double(img[i]) / 255; // alpha value
        gridOuter[i] =
                a == 1.0 ? 0.0 : a == 0.0 ? INF : pow(std::max(0.0, 0.5 - a), 2.0);
        gridInner[i] =
                a == 1.0 ? INF : a == 0.0 ? 0.0 : pow(std::max(0.0, a - 0.5), 2.0);
    }
    edt(gridOuter, width, height, f, d, v, z);
    edt(gridInner, width, height, f, d, v, z);

    for (uint32_t i = 0; i < size; i++) {
        double distance = gridOuter[i] - gridInner[i];
        data[i] =
                std::max(0l, std::min(255l, ::lround(255.0 - 255.0 * (distance / radius + cutoff))));
    }

}
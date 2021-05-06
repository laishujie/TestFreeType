//
// Created by admin on 2021/4/22.
//


#ifndef TESTFREETYPE_TINYSDF_H
#include<vector>
#include <math.h>
#include <memory>

class tiny_sdf {
public:
    // 1D squared distance transform
    void edt1d(std::vector<double>& f,
               std::vector<double>& d,
               std::vector<int16_t>& v,
               std::vector<double>& z,
               uint32_t n);

    void edt(std::vector<double>& data,
             uint32_t width,
             uint32_t height,
             std::vector<double>& f,
             std::vector<double>& d,
             std::vector<int16_t>& v,
             std::vector<double>& z);
    void  transformRasterToSDF(unsigned char* data,const unsigned char *img,unsigned int width,unsigned int height,double radius, double cutoff);
};


#endif //TESTFREETYPE_TINYSDF_H

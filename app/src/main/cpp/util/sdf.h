//
// Created by admin on 2021/4/22.
//

#ifndef TESTFREETYPE_SDF_H
#define TESTFREETYPE_SDF_H


#include <math.h>
#include <stdlib.h>

class sdf {

    // Same as distXform, but does not allocate any memory.
// The 'temp' array should be enough to fit width * height * sizeof(float) * 3 bytes.
    void sdfBuildDistanceFieldNoAlloc(unsigned char* out, int outstride, float radius,
                                      const unsigned char* img, int width, int height, int stride,
                                      unsigned char* temp);
    void sdfCoverageToDistanceField(unsigned char* out, int outstride,
                                    const unsigned char* img, int width, int height, int stride);

public:
    int sdfBuildDistanceField(unsigned char* out, int outstride, float radius,
                              const unsigned char* img, int width, int height, int stride);

    double * make_distance_mapd(double *data, unsigned int width, unsigned int height );

    unsigned char * make_distance_mapb( unsigned char *img,
                        unsigned int width, unsigned int height );

};


#endif //TESTFREETYPE_SDF_H

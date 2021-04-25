//
// Created by admin on 2021/4/22.
//

#ifndef TESTFREETYPE_EDTAA3FUNC_H
#define TESTFREETYPE_EDTAA3FUNC_H

#include <math.h>
#include <stdlib.h>
#include <string.h>

class edtaa3func {

/*
 * A somewhat tricky function to approximate the distance to an edge in a
 * certain pixel, with consideration to either the local gradient (gx,gy)
 * or the direction to the pixel (dx,dy) and the pixel greyscale value a.
 * The latter alternative, using (dx,dy), is the metric used by edtaa2().
 * Using a local estimate of the edge gradient (gx,gy) yields much better
 * accuracy at and near edges, and reduces the error even at distant pixels
 * provided that the gradient direction is accurately estimated.
 */
    double edgedf(double gx, double gy, double a);


    double distaa3(double *img, double *gximg, double *gyimg, int w, int c, int xc, int yc, int xi, int yi);

// Shorthand macro: add ubiquitous parameters dist, gx, gy, img and w and call distaa3()
#define DISTAA(c,xc,yc,xi,yi) (distaa3(img, gx, gy, w, c, xc, yc, xi, yi))

public:
/*
* Compute the local gradient at edge pixels using convolution filters.
* The gradient is computed only at edge pixels. At other places in the
* image, it is never used, and it's mostly zero anyway.
*/
void computegradient(double *img, int w, int h, double *gx, double *gy);

  void edtaa3(double *img, double *gx, double *gy, int w, int h, short *distx, short *disty, double *dist);
};


#endif //TESTFREETYPE_EDTAA3FUNC_H

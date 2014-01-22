#include <stdio.h>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdint.h>
#include "libsign.h"

#define max(a, b) (a > b? a : b)
#define min(a, b) (a < b? a : b)

int dx[] = {-1, 0, 1, 1, 1, 0, -1, -1};
int dy[] = {-1, -1, -1, 0, 1, 1, 1, 0};
int queue[1 << 20];
int qs, qe;
IplImage *desgraca;

CvRect sign_get_min_max(IplImage *img){
    int minx = 0x3f3f3f3f;
    int maxx = -0x3f3f3f3f;
    int miny = 0x3f3f3f3f;
    int maxy = -0x3f3f3f3f;
    CvRect r;
    int total = img->widthStep * img->height;
    uint8_t *pixels = (uint8_t *) img->imageData;
    int i, y, x;

    for(i = 0; i < total; i++){
        if(pixels[i]) continue;

        y = i / img->widthStep;
        x = i % img->widthStep;
        if(x >= img->width) continue;

        minx = min(minx, x);
        miny = min(miny, y);
        maxx = max(maxx, x);
        maxy = max(maxy, y);
    }

    r.x = minx;
    r.y = miny;
    r.width = maxx - minx + 1;
    r.height = maxy - miny + 1;

    return r;
}

void sign_swallow(IplImage *img, uint8_t *is, uint8_t *val){
    int i;
    int total = img->widthStep * img->height;
    uint8_t *pixels = (uint8_t *) img->imageData;

    for(i = 0; i < total; i++)
        if(val[is[i]]) pixels[i] ^= 0xff;
}

void sign_transpose(IplImage *img, CvRect rect){
    int y, x;
    uint8_t *p = (uint8_t *) img->imageData;
    uint8_t *p2 = p + rect.y * img->widthStep + rect.x;

    for(y = 0; y < rect.height; y++, p += img->widthStep, p2 += img->widthStep)
        for(x = 0; x < rect.width; x++)
            p[x] = p2[x];
}

void sign_threshold(IplImage *img, uint8_t t){
    int total = img->widthStep * img->height;
    uint8_t *pixels = (uint8_t *) img->imageData;
    uint8_t *p;
    for(p = pixels; p < pixels + total; p++)
        *p = (*p > t? 0xff : 0);
}

int sign_fill(int s, int v, uint8_t *is, IplImage *img, int *t){
    uint8_t *pixels = (uint8_t *) img->imageData;
    uint8_t e = pixels[s];
    int p, n, i, x, y, xx, yy, r = 1;

    qs = qe = 0;
    queue[qe++] = s;
    is[s] = v;
    *t = 0;

    while(qs < qe){
        n = queue[qs++];
        x = n % img->widthStep;
        y = n / img->widthStep;
        (*t)++;

        for(i = 0; i < 8; i++){
            xx = x + dx[i];
            yy = y + dy[i];
            p = yy * img->widthStep + xx;
            if(xx < 0 || xx >= img->width || yy < 0 || yy >= img->height){
                r = 0;
                continue;
            }
            if(is[p] || pixels[p] != e)
                continue;

            is[p] = v;
            queue[qe++] = p;
        }
    }

    return (e? r : 0);
}

void sign_bucket(IplImage *img, int *bs, int *cr, uint8_t *is, uint8_t *val){
    int y, x;
    int i, t, r;
    uint8_t n = 0;
    uint8_t *pixels = (uint8_t *) img->imageData;

    *bs = 0;
    *cr = 0;

    for(i = 0, y = 0; y < img->height; y++){
        for(x = 0; x < img->width; x++, i++){
            if(is[i]) continue;
            r = sign_fill(i, ++n, is, img, &t);

            if(t >= MIN_PIXELS){
                if(!pixels[i])
                    (*bs)++;
                *cr += r;
            }
            else
                val[n] = 1;
        }
        i += img->widthStep - img->width;
    }
}

void fill_mask(IplImage *img, CvRect rect, double *has){
    int y, x;
    int maxy, miny;
    maxy = 0;
    miny = rect.height-1;

    for(y = 0; y < rect.height; y++)
        for(x = 0; x < rect.width; x++){
            if(!(CV_IMAGE_ELEM(img, uchar, y + rect.y, x + rect.x))){
                maxy = max(maxy, y + rect.y);
                miny = min(miny, y + rect.y);
            }
        }

    has[0] = (double) miny / (double) rect.height;
    has[1] = (double) maxy / (double) rect.height;
}

Chars sign_mask(IplImage *img, CvRect rect){
    Chars r;
    double has[2];
    int i, t, p = 0;
    double sw, x;
    sw = (double) rect.width / (double) GRIDX;

    memset(has, 0, sizeof(has));

    for(x = 0.0, i = 0; i < GRIDX; i++, x += sw, p += 2){
        fill_mask(img,
            cvRect((int) x, 0,
                min((int) (x+sw+0.5), rect.width-1) - (int) x + 1,
                rect.height), has);
        for(t = 0; t < 2; t++)
            r.v[OFFSET + p + t] = has[t];
    }

    return r;
}

Chars sign_get_chars(IplImage *img){
    Chars r;
    CvRect rect;
    int bs, cr;
    uint8_t val[256];
    uint8_t *is = malloc(img->widthStep * img->height * sizeof(uint8_t));
    memset(is, 0, sizeof(uint8_t) * img->widthStep * img->height);
    memset(val, 0, sizeof(val));


    sign_threshold(img, THRESHOLD);
    sign_bucket(img, &bs, &cr, is, val);
    sign_swallow(img, is, val);
    rect = sign_get_min_max(img);
    desgraca = cvCreateImage(cvSize(rect.width, rect.height),
        img->depth, img->nChannels);
    sign_transpose(img, rect);

    r = sign_mask(img, rect);
    r.v[0] = (double) rect.width / (double) rect.height;
    r.v[1] = bs;
    r.v[2] = cr;

    /*cvNamedWindow("opa", CV_WINDOW_AUTOSIZE);
    cvShowImage("opa", desgraca);
    cvWaitKey(-1);
    cvDestroyWindow("opa");
    cvReleaseImage(&desgraca);*/

    free(is);

    return r;
}

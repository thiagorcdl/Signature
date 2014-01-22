#include <stdio.h>
#include <string.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "libsign.h"

IplImage *img = NULL;
Chars c;

int main(int argc, char *argv[]){
    int i;

    if(argc < 3){
        fprintf(stderr, "Usage: %s INPUT_IMAGE CLASS DISPLAY\n", argv[0]);
        return 1;
    }

    img = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    if(!img){
        fprintf(stderr, "Failed to open %s.\n", argv[1]);
        return 2;
    }

    c = sign_get_chars(img);
    c.c = atoi(argv[2]);
    uint64_t *p = (uint64_t *) c.v;

    for(i = 0; i < CHARS; i++)
        printf("%016" PRIX64 "\n", p[i]);
    printf("%d\n", c.c);

    if(argc >= 4){
        cvNamedWindow("Output", CV_WINDOW_AUTOSIZE);
        cvShowImage("Output", img);
        cvWaitKey(-1);
        cvDestroyWindow("Output");
    }

    cvReleaseImage(&img);

    return 0;
}

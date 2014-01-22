#define THRESHOLD 224
#define MIN_PIXELS 16
#define GRIDX 16
#define OFFSET 3
#define CHARS ((GRIDX << 1) + OFFSET)

typedef struct Chars {
    double v[256];
    int c;
} Chars;

CvRect sign_get_min_max(IplImage *);
void sign_transpose(IplImage *, CvRect);
void sign_threshold(IplImage *, uint8_t);
Chars sign_get_chars(IplImage *);

#ifndef SRBUFHEADER
#define SRBUFHEADER
typedef struct {
    unsigned short* amp;
    unsigned short* pha;
    int nCols;
    int nRows;
    int bufferSizeInBytes;
} SRBUF;
#endif // 

#ifndef SRBUFHEADER
#define SRBUFHEADER
typedef struct srBufVar{
    unsigned short* amp;
    unsigned short* pha;
    int nCols;
    int nRows;
    int bufferSizeInBytes;
} SRBUF;
#endif // 

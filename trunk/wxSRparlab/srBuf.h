/*
 * srBuf.h
 * A convenience structure to define a srBuffer (amp and phase),
 * along with number of rows and columns
 *
 * @author: James Mure-Dubois
 * @version: 2008.10.30
 */

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


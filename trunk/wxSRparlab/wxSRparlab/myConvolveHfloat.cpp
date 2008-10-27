#include "wxSRparlab.h"
#include <math.h>
#include "myConvolveHfloat.h"

#ifndef IPLENG
myKernel1D* CreateMyKernel1D( int size, int anchor, float* coeffs)
{
	myKernel1D* myKern1D = NULL;								// declare pointer
	/*myKern1D=(myKernel1D*)malloc(sizeof(myKernel1D)+sizeof(float)*size);*/
	myKern1D=(myKernel1D*)malloc(sizeof(myKernel1D));	// allocate memory
	myKern1D->_kSize = size;							// set size
	myKern1D->_kA = anchor;								// set anchor point
	myKern1D->_coeffs = (float*)malloc(sizeof(float)*size);	// allocate memory
															// for coeffs
	/*memset(myKern1D->_coeffs ,0x00,sizeof(float)*(size-1));*/
	for(int i= 0; i<size; i++)					// initialize kernel coefficients
	{
		myKern1D->_coeffs[i] = coeffs[i];
	}
	return myKern1D;
}


void    DeallocateMyKernel1D(myKernel1D* myKern1D)
{
	if(!myKern1D ) return;
	SAFE_FREE(myKern1D->_coeffs);	// free coefficients buffer
	return;
}

myConvImg* AllocMyConvImg( int size, int step)
{
	myConvImg* myImg;								// declare pointer
	myImg=(myConvImg*)malloc(sizeof(myConvImg));	// allocate memory
	myImg->_imgSize = size;							// set size
	myImg->_step    = step;							// set anchor point
	myImg->_data = (float*)malloc(sizeof(float)*size);	// allocate memory
														// for coeffs
	memset(myImg->_data ,0x00,sizeof(float)*(size));	// initialize image
	return myImg;
}

void    DeallocateMyConvImg(myConvImg* myImg)
{
	if(!myImg ) return;
	SAFE_FREE(myImg->_data);	// free image buffer
	return;
}

//---------------------------------------------------
/*! 
	SLOW convolution with horizontal 1D kernel
*/
//! convolution with horizontal 1D kernel
int myConvolveHfloat( myConvImg* myImg,		//!< input image
					  myKernel1D* myKern1D, //!< input kernel
					  myConvImg* imgOut)	//!< output image
{	
	int res = 0; 

	size_t imgSize = myImg->_imgSize;
	size_t imgStep = myImg->_step;
	float*     img = myImg->_data; 

	float* kh     = myKern1D->_coeffs ;
	size_t khSize = myKern1D->_kSize  ;
	size_t khStep = myKern1D->_kSize  ;
	size_t khA    = myKern1D->_kA     ;
	
	float imgCheck = fmod( (float)imgSize , (float)imgStep );
	if( imgCheck != 0.0f) return -2;
	if( khSize != khStep) return -3;
	if( (khA < 0) || (khA > khSize) ) return -4;

	int nLines = (int)imgSize / (int)imgStep ;

	int x, y, i, k;
	float sSum;

	for(y = 0; y < nLines; y++)
	{
		for( x = 0; x < (int)imgStep; x++)
		{
			sSum = 0.0f;
			k = 0;
			for( i=0; i<(int)khStep; i++)
			{
				if( !(( (x -(int)khA +i)< 0 ) || ( (x -(int)khA +i) > ((int)imgStep-1) ) ))
				{
					sSum += img[ y*(int)imgStep + x -(int)khA + i] * kh[i];
					k+=1;
				}
			}
			if(k==0)
			{
				k=1;
				res = -1;
			}
			imgOut->_data[ y*(int)imgStep + x ] = sSum / k;
		}
	}

	return res;
}

//---------------------------------------------------
/*! 
	SLOW convolution with vertical 1D kernel
*/
//! convolution with vertical 1D kernel
int myConvolveVfloat( myConvImg*  myImg,	//!< input image
					  myKernel1D* myKern1D,	//!< input kernel
					  myConvImg*  imgOut)	//!< output image
{	
	int res = 0; 

	size_t imgSize = myImg->_imgSize;
	size_t imgStep = myImg->_step;
	float*     img = myImg->_data; 

	float* kv     = myKern1D->_coeffs ;
	size_t kvSize = myKern1D->_kSize  ;
	size_t kvStep = myKern1D->_kSize  ;
	size_t kvA    = myKern1D->_kA     ;
	
	float imgCheck = fmod( (float)imgSize , (float)imgStep );
	if( imgCheck != 0.0f) return -2;
	if( kvSize != kvStep) return -3;
	if( (kvA < 0) || (kvA > kvSize) ) return -4;

	int nLines = (int)imgSize / (int)imgStep ;

	int x, y, i, k;
	float sSum;

	for(y = 0; y < nLines; y++)
	{
		for( x = 0; x < (int)imgStep; x++)
		{
			sSum = 0.0f;
			k = 0;
			for( i=0; i<(int)kvStep; i++)
			{
				if( !(( (y -(int)kvA +i)< 0 ) || ( (y -(int)kvA +i) > (nLines-1) ) ))
				{
					sSum += img[ (y -(int)kvA + i)*(int)imgStep + x ] * kv[i];
					k+=1;
				}
			}
			if(k==0)
			{
				k=1;
				res = -1;
			}
			imgOut->_data[ y*(int)imgStep + x ] = sSum / k;
		}
	}

	return res;
}

//---------------------------------------------------
/*! 
	SLOW 2D convolution separated in two 1D convolutions. \n
	This is appropriate only for GAUSSIAN kernels.\n
*/
//! 2D convolution separated in two 1D convolutions
int myConvolveHVfloat( myConvImg* myImg,	//!< input image
					  myKernel1D* myKernH,	//!< input kernel (horizontal)
					  myKernel1D* myKernV,	//!< input kernel (vertical)
					  myConvImg*  imgOut)	//!< output image
{	
	int res = 0; 
		// local variables
	size_t imgSize = myImg->_imgSize;	// image size
	size_t imgStep = myImg->_step;		// image width
	float*     img = myImg->_data;		// image data

	float* kh     = myKernH->_coeffs ;	// kernel coeffs
	size_t khSize = myKernH->_kSize  ;	// kernel total size
	size_t khStep = myKernH->_kSize  ;	// kernel width
	size_t khA    = myKernH->_kA     ;	// kernel anchor

	float* kv     = myKernV->_coeffs ;	// kernel coeffs
	size_t kvSize = myKernV->_kSize  ;	// kernel total size
	size_t kvStep = myKernV->_kSize  ;	// kernel height
	size_t kvA    = myKernV->_kA     ;	// kernel anchor
	
	float imgCheck = fmod( (float)imgSize , (float)imgStep );
	if( imgCheck != 0.0f) return -2;	// return error if image size is not 
										// a multiple of image width.
	if( khSize != khStep) return -3;	// return error if kernel is not 1D
	if( (khA < 0) || (khA > khSize) ) return -4;  // return error if anchor invalid

	int nLines = (int)imgSize / (int)imgStep ;	// determine image height
	  // allocate and initializewintermediate image
	float* imgInt  = (float*)malloc(imgSize*sizeof(float));
	memset(imgInt ,0x00,sizeof(float)*(imgSize));

	int x, y, i;		// some variables used in loops
	float sSum, k;		// some variables used in loops

// FIRST convolution : horizontal
	for(y = 0; y < nLines; y++)	// for each line
	{
		for( x = 0; x < (int)imgStep; x++) // for each pixel in line
		{
			sSum = 0.0f;	// reset sum
			k = 0.0f;		// reset kernel normalization sum (for edge effects)
			for( i=0; i<(int)khStep; i++) // convolution accumulationloop
			{
				//k+=kh[i];		// constant kernel integral
				if( !(( (x -(int)khA +i)< 0 ) || ( (x -(int)khA +i) > ((int)imgStep-1) ) ))
				{
					sSum += img[ y*(int)imgStep + x -(int)khA + i] * kh[i];
					k+=kh[i];	// 'edge-aware' kernel integral
				}
			}
			if(k==0.0f)		// if no kernel coeff was hit
			{
				k=1.0f;		// prevent divide by 0 error
				res = -1;	// report error
			}
			k = 1.0f; // use this for conformity with IPLconvolveSep2D
			imgInt[ y*(int)imgStep + x ] = sSum / k;	// convolution normalization
		}
	}
// SECOND convolution : vertical
	for(y = 0; y < nLines; y++)	// for each line
	{
		for( x = 0; x < (int)imgStep; x++)	// for each pixel in line
		{
			sSum = 0.0f;
			k = 0.0f;
			for( i=0; i<(int)kvStep; i++)	// convolution accumulate loop
			{
				//k+=kv[i];		// constant kernel integral
				if( !(( (y -(int)kvA +i)< 0 ) || ( (y -(int)kvA +i) > (nLines-1) ) ))
				{
					sSum += imgInt[ (y -(int)kvA + i)*(int)imgStep + x ] * kv[i];
					k+=kv[i];  // 'edge-aware' kernel integral
				}
			}
			if(k==0.0f)		// if no kernel coeff was hit
			{
				k=1.0f;		// prevent divide by 0 error
				res = -1;	// report error
			}
			k = 1.0f; // use this for conformity with IPLconvolveSep2D
			imgOut->_data[ y*(int)imgStep + x ] = sSum / k; // convolution normalization
		}
	}
	SAFE_FREE(imgInt);

	return res;
}

//---------------------------------------------------
/*! 
	EXTREMELY SLOW 2D convolution NOT SEPARATED. \n
	This is appropriate only for GAUSSIAN kernels.\n
*/
//! 2D convolution separated in two 1D convolutions
int myConvolve2Dfloat( myConvImg* myImg,	//!< input image
					  myKernel1D* myKernH,	//!< input kernel (horizontal)
					  myKernel1D* myKernV,	//!< input kernel (vertical)
					  myConvImg*  imgOut)	//!< output image
{	
	int res = 0; 
		// local variables
	size_t imgSize = myImg->_imgSize;	// image size
	size_t imgStep = myImg->_step;		// image width
	float*     img = myImg->_data;		// image data

	float* kh     = myKernH->_coeffs ;	// kernel coeffs
	size_t khSize = myKernH->_kSize  ;	// kernel total size
	size_t khStep = myKernH->_kSize  ;	// kernel width
	size_t khA    = myKernH->_kA     ;	// kernel anchor

	float* kv     = myKernV->_coeffs ;	// kernel coeffs
	size_t kvSize = myKernV->_kSize  ;	// kernel total size
	size_t kvStep = myKernV->_kSize  ;	// kernel height
	size_t kvA    = myKernV->_kA     ;	// kernel anchor
	
	float imgCheck = fmod( (float)imgSize , (float)imgStep );
	if( imgCheck != 0.0f) return -2;	// return error if image size is not 
										// a multiple of image width.
	if( khSize != khStep) return -3;	// return error if kernel is not 1D
	if( (khA < 0) || (khA > khSize) ) return -4;  // return error if anchor invalid

	int nLines = (int)imgSize / (int)imgStep ;	// determine image height

	int x, y, i,j;		// some variables used in loops
	float sSum, k;		// some variables used in loops

// FIRST convolution : horizontal
	for(y = 0; y < nLines; y++)	// for each line
	{
		for( x = 0; x < (int)imgStep; x++) // for each pixel in line
		{
			sSum = 0.0f;	// reset sum
			k = 0.0f;		// reset kernel normalization sum (for edge effects)
			for( i=0; i<(int)khStep; i++) // convolution accumulationloop
			{
				for( j=0; j<(int)kvStep; j++)	// convolution accumulate loop
				{
					//k+=kh[i]*kv[j];		// constant kernel integral
					if( !(( (x -(int)khA +i)< 0 ) || ( (x -(int)khA +i) > ((int)imgStep-1) ) ))
					{
						if( !(( (y -(int)kvA +j)< 0 ) || ( (y -(int)kvA +j) > (nLines-1) ) ))
						{
							sSum += img[ (y -(int)kvA + j)*(int)imgStep + x -(int)khA + i] * kh[i] * kv[j];
							k+=kh[i]*kv[j];  // 'edge-aware' kernel integral
						}
					}
				}
			}
			if(k==0.0f)		// if no kernel coeff was hit
			{
				k=1.0f;		// prevent divide by 0 error
				res = -1;	// report error
			}
			k = 1.0f; // use this for conformity with IPLconvolveSep2D
			imgOut->_data[ y*(int)imgStep + x ] = sSum / k; // convolution normalization
		}
	}
	return res;
}

#endif

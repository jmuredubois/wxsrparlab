// myConvolveHfloat.h :  header file 
#pragma once


#ifndef IPLENG

//---------------------------------------------------
/*!
	Structure for 1D kernel \n
*/
//! 1D Kernel
typedef struct _myKernel1D
{
  int		_kSize;	//!< kernel size (number of coeffs)
  int		_kA;	//!< kernel anchor point
  float*	_coeffs;//!< kernel coefficients
}myKernel1D;

//---------------------------------------------------
/*!
	Structure for convolution images \n
*/
//! float image structure
typedef struct _myConvImg
{
  int		_imgSize;	//!< size of image
  int		_step;		//!< image width
  float*	_data;		//!< image data
}myConvImg;

  //! Create myKernel1D object
myKernel1D* CreateMyKernel1D(int size, int anchor, float* coeffs);
  //! Deallocate myKernel1D object
void    DeallocateMyKernel1D(myKernel1D* myKern1D);

  //! Create myConvImg object
myConvImg* AllocMyConvImg( int size, int step);
  //! Deallocate myConvImg object
void    DeallocateMyConvImg(myConvImg* myImg);

  //! convolution with horizontal 1D kernel
int myConvolveHfloat( myConvImg* myImg,		//!< input image
					  myKernel1D* myKern1D, //!< input kernel
					  myConvImg* imgOut);	//!< output image
  //! convolution with vertical 1D kernel
int myConvolveVfloat( myConvImg* myImg,		//!< input image
					  myKernel1D* myKern1D,	//!< input kernel
					  myConvImg* imgOut);	//!< output image
  //! 2D convolution separated in two 1D convolutions
int myConvolveHVfloat( myConvImg* myImg,	//!< input image
					  myKernel1D* myKernH,	//!< input kernel (horizontal)
					  myKernel1D* myKernV,	//!< input kernel (vertical)
					  myConvImg*  imgOut);	//!< output image
  //! 2D convolution NOT SEPARATED 
int myConvolve2Dfloat( myConvImg* myImg,	//!< input image
					  myKernel1D* myKernH,	//!< input kernel (horizontal)
					  myKernel1D* myKernV,	//!< input kernel (vertical)
					  myConvImg*  imgOut);	//!< output image

#endif   // !IPLENG
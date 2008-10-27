#include "wxSRparlab.h"
#include "imgPadder.h"

///* NOT IMPLEMENTED - Pads a fftw_complex image
// *
// */
//int CImgPadder::imgPad(double* dbIn, long dbLen, double padFact)
//{
//	int res = 0;
//	return res;
//}

///* NOT IMPLEMENTED - Pads a fftw_complex image
// *
// */
//int CImgPadder::imgPad(fftw_complex* cxIn, long cxLen, double padFact)
//{
//	int res = 0;
//	return res;
//}

/* Pads a fftw_complex image
 *
 */
int CImgPadder::imgPad(fftw_complex* cxIn, long cxLin, IMGSIZE szIn, fftw_complex* cxOut, long cxLout, IMGSIZE szOut)
{
	int res = 0;
	if(!cxIn)   return -1;					// basic error check
	if(cxLin != szIn.cx*szIn.cy*sizeof(fftw_complex))  return -2;
	if(!cxOut)   return -3;					// basic error check
	if(cxLout != szOut.cx*szOut.cy*sizeof(fftw_complex))  return -4;
	if( (szIn.cx >= szOut.cx) || (szIn.cy >= szOut.cy)) return -5;

	// compute size of pad ramps
	int nNorth = (int) ceil(  (szOut.cy-szIn.cy) /2.0);
	int nSouth = (int) floor( (szOut.cy-szIn.cy) /2.0);
	int nWest  = (int) ceil(  (szOut.cx-szIn.cx) /2.0);
	int nEast  = (int) floor( (szOut.cx-szIn.cx) /2.0);
	
	// allocate pad ramps
	double* rNorth = (double*) malloc(nNorth*sizeof(double));
	double* rSouth = (double*) malloc(nSouth*sizeof(double));
	double* rWest  = (double*) malloc(nWest *sizeof(double));
	double* rEast  = (double*) malloc(nEast *sizeof(double));
#ifndef PAD_ZEROS
	//// fill ramps
	//res += gaussRamp(rNorth, nNorth*sizeof(double), nNorth);
	//res += gaussRamp(rSouth, nSouth*sizeof(double), nSouth);
	//res += gaussRamp(rWest , nWest *sizeof(double), nWest );
	//res += gaussRamp(rEast , nEast *sizeof(double), nEast );

	// fill ramps // linear-gaussian
	res += linGaussRamp(rNorth, nNorth*sizeof(double), nNorth);
	res += linGaussRamp(rSouth, nSouth*sizeof(double), nSouth);
	res += linGaussRamp(rWest , nWest *sizeof(double), nWest );
	res += linGaussRamp(rEast , nEast *sizeof(double), nEast );
#else
	// fill ramps // ZEROS
	res += zeroRamp(rNorth, nNorth*sizeof(double), nNorth);
	res += zeroRamp(rSouth, nSouth*sizeof(double), nSouth);
	res += zeroRamp(rWest , nWest *sizeof(double), nWest );
	res += zeroRamp(rEast , nEast *sizeof(double), nEast );
#endif
	// initialize new image
	memset((void*) cxOut, 0x0, szOut.cx*szOut.cy*sizeof(fftw_complex));
	
	// CENTER part of the new image -> identical to the old image
	int off = ( nNorth * szOut.cx );
	int m = 0; int n = 0;
	for(m = 0; m < szIn.cy ; m++) // for all lines of input image
	{
		for(n = 0; n < szIn.cx; n++) 
		{
			cxOut[off+ m*szOut.cx +nWest +n][0] = cxIn[m*szIn.cx+n][0];
			cxOut[off+ m*szOut.cx +nWest +n][1] = cxIn[m*szIn.cx+n][1];
		}
	}

	// WEST part of the image
	for(m = 0; m < szIn.cy ; m++) // for all lines of input image
	{
		double re = cxIn[m*szIn.cx][0];
		double im = cxIn[m*szIn.cx][1];
		for(n = 0; n < nWest; n++) 
		{
			cxOut[off+ m*szOut.cx +nWest -n-1][0] = re*rWest[n];
			cxOut[off+ m*szOut.cx +nWest -n-1][1] = im*rWest[n];
		}
	}

	// EAST part of the image
	for(m = 0; m < szIn.cy ; m++) // for all lines of input image
	{
		double re = cxIn[(m+1)*szIn.cx-1][0];
		double im = cxIn[(m+1)*szIn.cx-1][1];
		for(n = 0; n < nEast; n++) 
		{
			cxOut[off+ m*szOut.cx +nWest+szIn.cx +n][0] = re*rEast[n];
			cxOut[off+ m*szOut.cx +nWest+szIn.cx +n][1] = im*rEast[n];
		}
	}

	// NORTH part of the image
	for(n = 0; n < szOut.cx ; n++) // for columns of OUTput image
	{
		double re = cxOut[off+n][0];
		double im = cxOut[off+n][1];
		for(m = 0; m < nNorth; m++) 
		{
			cxOut[m*szOut.cx +n][0] = re*rNorth[nNorth-1-m];
			cxOut[m*szOut.cx +n][1] = im*rNorth[nNorth-1-m];
		}
	}

	// SOUTH part of the image
	off = ( (nNorth+szIn.cy) * szOut.cx );
	for(n = 0; n < szOut.cx ; n++) // for columns of OUTput image
	{
		double re = cxOut[off-szOut.cx+n][0];
		double im = cxOut[off-szOut.cx+n][1];
		for(m = 0; m < nSouth; m++) 
		{
			cxOut[off+m*szOut.cx +n][0] = re*rSouth[m];
			cxOut[off+m*szOut.cx +n][1] = im*rSouth[m];
		}
	}

	//free pad ramps
	if(rNorth){ free(rNorth); rNorth = NULL;};
	if(rSouth){ free(rSouth); rSouth = NULL;};
	if(rWest ){ free(rWest ); rWest  = NULL;};
	if(rEast ){ free(rEast ); rEast  = NULL;};

	// return (after all objects have been freed!)
	return res;
}

/* Computes weights of a gaussian ramp for image padding
 *  - nRamp contains the number or coefficitns desired
 *  - the buffer wgtIn will be overwritten with the weights values
 *  - wgLen is the length of the buffer in bytes
 */
int CImgPadder::gaussRamp(double* wgtIn, long wgLen, int nRamp)
{
	int res = 0;
	if(!wgtIn) return -1;
	if(nRamp*sizeof(double)!=wgLen) return -2;
	double sigma = nRamp / 4.0;
	//double pi = 3.1415926535;
	for(int k=0; k< nRamp; k++)
	{
		wgtIn[k] = exp(-( (k*k) / (2.0*sigma*sigma)));
	}
	wgtIn[nRamp-1] = 0; // finish by a zero to be sure.
	return res;
}

/* Computes weights of a linear gaussian ramp for image padding
 *  - nRamp contains the number or coefficitns desired
 *  - the buffer wgtIn will be overwritten with the weights values
 *  - wgLen is the length of the buffer in bytes
 */
int CImgPadder::linGaussRamp(double* wgtIn, long wgLen, int nRamp)
{
	int res = 0;
	if(!wgtIn) return -1;
	if(nRamp*sizeof(double)!=wgLen) return -2;
	double sigma = nRamp / 8.0;
	//double pi = 3.1415926535;
	int nLin = (int) floor(nRamp/2.0) ;
	int nGau = (int)  ceil(nRamp/2.0) ;
	if(nLin+nGau!=nRamp) return -3;
	int k;
	for( k=0; k< nLin; k++)
	{
		wgtIn[k] = 1.0;
	}
	for( k=0; k< nGau; k++)
	{
		wgtIn[k+nLin] = exp(-( (k*k) / (2.0*sigma*sigma)));
	}
	wgtIn[nRamp-1] = 0; // finish by a zero to be sure.
	return res;
}

/* Computes weights of a linear gaussian ramp for image padding
 *  - nRamp contains the number or coefficitns desired
 *  - the buffer wgtIn will be overwritten with the weights values
 *  - wgLen is the length of the buffer in bytes
 */
int CImgPadder::zeroRamp(double* wgtIn, long wgLen, int nRamp)
{
	int res = 0;
	if(!wgtIn) return -1;
	if(nRamp*sizeof(double)!=wgLen) return -2;
	double sigma = nRamp / 8.0;
	//double pi = 3.1415926535;
	int nLin = (int) floor(nRamp/2.0) ;
	int nGau = (int)  ceil(nRamp/2.0) ;
	if(nLin+nGau!=nRamp) return -3;
	int k;
	for( k=0; k< nLin; k++)
	{
		wgtIn[k] = 0.0;
	}
	for( k=0; k< nGau; k++)
	{
		wgtIn[k+nLin] = 0.0;
	}
	wgtIn[nRamp-1] = 0.0; // finish by a zero to be sure.
	return res;
}

/* UnPads a fftw_complex image
 *
 */
int CImgPadder::imgUnPad(fftw_complex* cxIn, long cxLin, IMGSIZE szIn, fftw_complex* cxOut, long cxLout, IMGSIZE szOut)
{
	int res = 0;
	if(!cxIn)   return -1;					// basic error check
	if(cxLin != szIn.cx*szIn.cy*sizeof(fftw_complex))  return -2;
	if(!cxOut)   return -3;					// basic error check
	if(cxLout != szOut.cx*szOut.cy*sizeof(fftw_complex))  return -4;
	if( (szIn.cx <= szOut.cx) || (szIn.cy <= szOut.cy)) return -5;

	// compute size of pad ramps
	int nNorth = (int) ceil(  (szIn.cy-szOut.cy) /2.0);
	int nSouth = (int) floor( (szIn.cy-szOut.cy) /2.0);
	int nWest  = (int) ceil(  (szIn.cx-szOut.cx) /2.0);
	int nEast  = (int) floor( (szIn.cx-szOut.cx) /2.0);

	// CENTER part of the new image -> identical to the old image
	int off = ( nNorth * szIn.cx );
	int m = 0; int n = 0;
	for(m = 0; m < szOut.cy ; m++) // for all lines of input image
	{
		for(n = 0; n < szOut.cx; n++) 
		{
			cxOut[m*szOut.cx+n][0] = cxIn[off+ m*szIn.cx +nWest +n][0] ;
			cxOut[m*szOut.cx+n][1] = cxIn[off+ m*szIn.cx +nWest +n][1] ;
		}
	}
	// return (after all objects have been freed!)
	return res;
}

/* UnPads a doubl image
 *
 */
int CImgPadder::imgUnPad(double* dbIn, long dbLin, IMGSIZE szIn, double* dbOut, long dbLout, IMGSIZE szOut)
{
	int res = 0;
	if(!dbIn)   return -1;					// basic error check
	if(dbLin != szIn.cx*szIn.cy*sizeof(double))  return -2;
	if(!dbOut)   return -3;					// basic error check
	if(dbLout != szOut.cx*szOut.cy*sizeof(double))  return -4;
	if( (szIn.cx <= szOut.cx) || (szIn.cy <= szOut.cy)) return -5;

	// compute size of pad ramps
	int nNorth = (int) ceil(  (szIn.cy-szOut.cy) /2.0);
	int nSouth = (int) floor( (szIn.cy-szOut.cy) /2.0);
	int nWest  = (int) ceil(  (szIn.cx-szOut.cx) /2.0);
	int nEast  = (int) floor( (szIn.cx-szOut.cx) /2.0);

	// CENTER part of the new image -> identical to the old image
	int off = ( nNorth * szIn.cx );
	int m = 0; int n = 0;
	for(m = 0; m < szOut.cy ; m++) // for all lines of input image
	{
		for(n = 0; n < szOut.cx; n++) 
		{
			dbOut[m*szOut.cx+n] = dbIn[off+ m*szIn.cx +nWest +n];
		}
	}
	// return (after all objects have been freed!)
	return res;
}

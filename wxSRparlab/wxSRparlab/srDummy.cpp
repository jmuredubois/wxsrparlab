/*
 * SR_Dummy.cpp
 * The goal of this dummy is to allow the build of an app
 * on systems where the libusbSR software was not installed.
 * The functions are noop-ed or replaced by appropriate operations.
 *
 * @author: James Mure-Dubois
 * @version: 2007.11.07
 */

#include "wxSRparlab.h"
#include "srDummy.h"

//--------- interfaces for C++, Delphi etc. ------------

//!Full documentation of the mode is not provided to the customer.
//!- To force the check to toe last available version set the mode to 3
//!- if mode = 0 it only does the check if the last check has been done a long time before
//!- if mode & 1 it always checks the version for new drivers
//! -if mode & 2 it also informs about version entries with no info tag
int SR_CheckForNewDllVersion(int mode)
{
	mode -=7;
	return -7;
};

//!returns the version of this libusbSR library.
int SR_GetVersion(unsigned short version[4])
{
	version[1] +=1;
	return -7;
};

MessageCB* SR_SetMessageCB(MessageCB* cb)
{
	//cb[0] = 0;
	return cb;
};

//!Returns a identification string of the device:
//!The string is formated as:
//!\verbatim
//!"VendorID:0x%04x, ProductID:0x%04x, Manufacturer:'%s', Product:'%s'" e.g.: 
//!VendorID:0x0852, ProductID:0x0071, Manufacturer:'CSEM SA', Product:'3D-SR2 16Bit'
//!\endverbatim 
int SR_GetDeviceString(SRCAM srCam, char* buf, int buflen)
{
	if(buflen > 12)
	{
		sprintf(buf,"SR-Dummy %i", -7); 
	}
	return -7;
};

//!sets the timout in ms. This timeout is used at reading and writing on usb port.
void SR_SetTimeout(SRCAM srCam, int ms)
{
};

//!returns the required buffersize for a SR_Acquire() call.
int SR_GetBufferSize(SRCAM srCam)
{
	return ((size_t)(176*144));
};

//!returns the number of rows the camera will deliver
unsigned int SR_GetRows(SRCAM srCam)
{
	return (unsigned int) 144;
};

//!returns the number of columns the camera will deliver
unsigned int  SR_GetCols(SRCAM srCam)
{
	return (unsigned int) 176;
};

//!returns the number of images the camera will deliver
unsigned int  SR_GetNumImg(SRCAM srCam)
{
	return (unsigned int) 2;
};

//!returns the number of byte each pixel will contain
unsigned int  SR_GetBytePerPix(SRCAM srCam)
{
	return (unsigned int) 2;
};

//!Set the buffer that will be used by functions like:
//!SR_Acquire(), SR_CoordTrfFlt(), SR_GetImage() etc.
//!The buffer is allocated and freed by the user.
//!Use SR_GetBufferSize() to query and allocate the required buffer size.
int SR_SetBuffer(SRCAM srCam, void *buffer,int size)
{
	return -7;
};

//! \deprecated Use SR_OpenUSB() instead
int SR_Open(SRCAM* srCam, unsigned int serialNumber) //depreciated: Use SR_OpenUSB, SR_OpenETH instead!
{
	return -7;
};
#pragma deprecated(SR_Open)
//!Opens the Swissranger USB device and retuns a device ID.
//!The serial number is only used, if more than one camera is connected to the computer
int SR_OpenUSB(SRCAM* srCam, unsigned int serialNumber)
{
	return -7;
};
//!Opens the Swissranger Ethernet device and retuns a device ID.
//!The addr is the ip address of the camera. e.g. "192.168.2.14"
//!THIS FUNCTION IS ONLY FOR TRIAL PURPOSE AND NOT FOR CUSTOMERS!
int SR_OpenETH(SRCAM* srCam,  const char* addr)
{
	return -7;
};

//!Closes a Swissranger device with the ID devID.
int SR_Close(SRCAM srCam)
{
	return -7;
};


//!SR_Acquire() acquires an image and correct the spherical distance image.
//!The possible corrections are static patterns and various spacial and temporal filters.\n
//!The mode selects the used filters. It is an ored value of the enumerator #AcquireMode.
//!Recommanded mode is: AM_COR_FIX_PTRN|AM_COR_LED_NON_LIN.\n
//!Further a temporal IIR filter can be applied. Use the function 
//!SR_SetTemporalIIR() to turn it on/off and to set the delay parameters.\n
//!The format of the memory buffer can be inquired with the functions:
//!SR_GetBufferSize(), SR_GetRows(), SR_GetCols(), SR_GetNumImg(), SR_GetBytePerPix().\n
//!Before SR_Acquire() is called, the buffer must be allocated with SR_SetBuffer().\n
//!After this function normally the sperical coordinates are transformed to cahesian coordinates.
//!for that purpose use one of the following functions:\n
//!SR_CoordTrfUint16(), SR_CoordTrfFlt(),SR_CoordTrfDbl()\n
//!\warning fix-pattern correction works only correctly, when no roi is set.
int SR_Acquire(SRCAM srCam, int mode)
{
	return -7;
};

//!SR_SetTemporalIIR() turns on/off and sets parameters of a temporal infinite impulse response filter.
//!This function also allocates or frees the appropriate temporal buffer.\n
//!The staticDelay  has a value between 0.0 and 1.0, where 0.0 turns the Temporal IIR off.\n
//!the dynamicDelay has a value between 0.0 and 1.0, where 0.0 turns the dynamicDelay off.\n
//!THIS FUNCTION IS ONLY FOR TRIAL PURPOSE AND NOT FOR CUSTOMERS!
int SR_SetTemporalIIR(SRCAM srCam, float staticDelay, float dynamicDelay)
{
	return -7;
};

//!SR_SetScatteringFilter() turns on/off the scattering filter.
//!setting blkSz=0 turns the filter off
//!THIS FUNCTION IS ONLY FOR TRIAL PURPOSE AND NOT FOR CUSTOMERS!
int SR_SetScatteringFilter(SRCAM srCam, unsigned char blkSzX, unsigned char blkSzY)
{
	return -7;
};

//!Transforms spherical coordinates to cathesian coordinates.
//!The pointers x,y,z are unsigned short arrays of the length SR_GetRows() * SR_GetCols().
//!The result values are in mm.
//!The pitches is the distance in byte from one value to the next. Default value is sizeof(unsigned short)=2
//!The memory management of x,y,z is handeled by the user.
//!\warning Coordinate Transformation works only correctly, when no roi is set
int SR_CoordTrfUint16(SRCAM srCam, short *x, short *y, unsigned short *z, int pitchX,int pitchY,int pitchZ)
{
	return -7;
};

//!Transforms spherical coordinates to cathesian coordinates.
//!The pointers x,y,z are float arrays of the length SR_GetRows() * SR_GetCols().
//!The pitches is the distance in byte from one value to the next. Default value is sizeof(float)=4
//!The memory management of x,y,z is handeled by the user.
//!\warning Coordinate Transformation works only correctly, when no roi is set
int SR_CoordTrfFlt(SRCAM srCam, float *x,float *y,float *z, int pitchX,int pitchY,int pitchZ)
{
	return -7;
};

//!Transforms spherical coordinates to cathesian coordinates.
//!The pitches is the distance in byte from one value to the next. Default value is sizeof(double)=8
//!The pointers x,y,z are double arrays of the length SR_GetRows() * SR_GetCols().
//!The memory management of x,y,z is handeled by the user.
//!\warning Coordinate Transformation works only correctly, when no roi is set
int SR_CoordTrfDbl(SRCAM srCam, double *x,double *y,double *z, int pitchX,int pitchY,int pitchZ)
{
	return -7;
};

//!Returns the pointer to the idx-th image. The idx has a value from 0 to SR_GetNumImg()-1.
//!SR_GetImage() is a helper function to navigate in the buffer set with the function SR_SetBuffer.
//!Usual index is 0 for distance image and 1 for intensity image
//!The distance image is the corrected image processed with SR_Acquire() (depending of the correction mode of source)
//!\sa SR_SetBuffer(),SR_GetBufferSize(), SR_GetRows(), SR_GetCols(), SR_GetNumImg(), SR_GetBytePerPix()
void* SR_GetImage(SRCAM srCam, unsigned char idx)
{
	return NULL;
};

//!Sets a register of the camera to a desired value
//!Ask MESA or read the manual for register description.
//!Be aware using this function. It can set the camera in unpredictible mode if wrong registers are set
//!and the registers can change in newer firmware versions!
//!Use, if possible, the driver specific function e.g. SR_SetIntegrationTime() for software compatibility 
//!and portability!
int SR_SetReg(SRCAM srCam, unsigned char reg, unsigned char val)
{
	return -7;
};

//!Returns a desired register value of the camera.
//!\sa SR_SetReg()
unsigned char  SR_GetReg(SRCAM srCam, unsigned char reg)
{
	return (unsigned char) 0x7;
};

//!Sets the Amplitude Threshold of the camera
int SR_SetAmplitudeThreshold(SRCAM srCam, unsigned short val)
{
	return -7;
};

//!Sets the Saturation Threshold of the camera
int SR_SetSaturationThreshold(SRCAM srCam, unsigned short val)
{
	return -7;
};

//!Sets the Integration time of the camera
//!The intTime is a value from 0 to 255.
//!The integration time in ms depends on the camera
//! \sa GetIntegrationTime()
int SR_SetIntegrationTime(SRCAM srCam, unsigned char intTime)
{
	return -7;
};

//!Sets the Integration time of the camera
//!The intTime is a value from 0 to 255.
//!The integration time in ms depends on the camera
//! \sa SR_SetIntegrationTime()
unsigned char SR_GetIntegrationTime(SRCAM srCam)
{
	return -7;
};


//!Sets the Distance Offset. This should not be used any more,
//!because offset correction is done by a camera specific calibration file
int SR_SetDistanceOffset(SRCAM srCam, unsigned char distOfs)
{
	return -7;
};

//!Sets the Region of Interrest of the camera.
//!This works only limited on some camera.
//!\warning FixPattern Correction and Coordinate Transformation actually works only correctly, when no roi is set.
//!This 'bug' will only be fixed on customer resuest, because ROI is rarly used.
int SR_SetROI(SRCAM srCam, unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
	return -7;
};

//!Opens a Dialog with various settings an information of the camera (Only on Windows)
int SR_OpenSettingsDlgModal(SRCAM srCam)
{
	return -7;
};

//Returns the serial number (if existing) of the camera
unsigned int SR_ReadSerial(SRCAM srCam)
{
	return -7;
};

//!Sets the modulation of the LEDs that will changes the measurement range.
//!\verbatim
//!30MHz ->  5.0 m
//!21MHz ->  7.1 m
//!20MHz ->  7.5 m
//!19MHz ->  7.9 m
//!\endverbatim
int SR_SetModulationFrequency(SRCAM srCam, ModulationFrq modFrq)
{
	return -7;
};

//!Gets the modulation of the LEDs that will changes the measurement range.
//!\sa SR_SetModulationFrequency()
ModulationFrq SR_GetModulationFrequency(SRCAM srCam)
{
	return MF_20MHz;
};

//!Turns the Autoillumination on/off and set the desired parameters.
//!if <tt>minIntTime=0xff</tt> the Autoillumination is turned off.<br>
//!good values are 5,255,10,45<br>
//!The function sets the integrationtime to a vallue between <tt>minIntTime</tt> and <tt>maxIntTime</tt>.<br>
//!Therefore a special position is searched in the intensity histogram (reduced from 16bit to 8bit) described as <tt>percentOverPos</tt>.<br>
//!\image html autoilum.png
//! <tt>percentOverPos</tt> (values 0-100) is the x-position of the histogram where <tt>percentOverPos</tt> percent of the points
//!are above. The intrgration time is adapted slowly to move this position to the <tt>desiredPos</tt>(value 0-255)intensity value.
int SR_SetAutoIllumination(SRCAM srCam, unsigned char minIntTime, unsigned char maxIntTime,
                                   unsigned char percentOverPos, unsigned char desiredPos)
{
	return -7;
};



//#define _countof(array) (sizeof(array)/sizeof(array[0]))

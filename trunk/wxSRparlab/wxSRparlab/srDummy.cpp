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

#ifdef DUMMYSR
#include "srDummy.h"

//--------- interfaces for C++, Delphi etc. ------------

//! Generic Open for any kind of camera.
//! This opens a dialog box and lists all available USB,ETH,1394 swissranger Cameras.
//! One of them can be selected and opened.
//!
//!the <tt>mode</tt> is used to select how the dialog is handled<br>
//!the mode bits are:
//! - 0 use registry if existing: tries to open the same device as last time
//! - 1 open dialog
//! - 2 open without configuration (internal usage only)
//!
//!so following modes make sense:
//! - 1 use registry, if it failed error is returned, no GUI opened
//! - 2 always open dialog
//! - 3 use registry, if it failed open the GUI dialog
//!
//!The parent is a HWND window handle.
//! \return 0: SUCCESSFUL, -1: FAILED
SR_API(int) SR_OpenDlg(SRCAM* srCam, int mode, HWND parent)
{
	return 0;
}

//!Opens the Swissranger USB device and returns a device ID.
//!The serial number is only used, if more than one camera is connected to the computer.\n
//!Setting the serialNumber to 0 opens the first found camera
SR_API(int) SR_OpenUSB(SRCAM* srCam, unsigned int serialNumber)
{
	return 0;
}

//!Opens the Swissranger Ethernet device and returns a device ID.
//!The addr is the ip address of the camera. e.g. "192.168.2.14"
//! \n\intDoc{function}
SR_API(int) SR_OpenETH(SRCAM* srCam,  const char* addr)
{
	return 0;
}

//!Opens the Swissranger Firewire device and returns a device ID.
//! \n\intDoc{function}
SR_API(int) SR_Open1394(SRCAM* srCam, int camID)
{
	return 0;
}

//!Opens the Swissranger File and returns a virtual device ID.
//!Such a file can be generated with with SR_StreamToFile().
//!Any settings that affect the data sent from the camera will be ignored.
SR_API(int) SR_OpenFile(SRCAM* srCam, const char* filename)
{
	return 0;
}

//!SR_StreamToFile opens a file and streams received Data from the camera to that file.
//!This file can subsequently be opened and treated as a virtual camera using SR_OpenFile().
//!The mode can be:
//!- 0 : Open-Create
//!- 1 : Open-Append NOT IMPLEMENTED
//!- 2 : Close
SR_API(int) SR_StreamToFile(SRCAM srCam, const char* filename, int mode)
{
	return 0;
}

//! Closes the swissranger device, first releasing the claimed
//! interface.
//!
//! @param srCam a valid device returned by the SR_Open()
//! function.
//! @return a negative number in case of error.
//!  0 success
//! -1 wrong device
//! -2 can't release interface
//! -3 can't close device
SR_API(int) SR_Close(SRCAM srCam)
{
	return 0;
}

//!Opens a dialog with various settings and information of the camera
//!The parent is a HWND.
//!The Window is Non Modal and will be destroyed, if it is closed or if the camera is closed.
SR_API(int) SR_OpenSettingsDlg(SRCAM srCam, HWND parent)
{
	return 0;
}

//!The mode selects the used filters. It is an ored value of the enumerator #AcquireMode.
//!The recommended mode is described in #AcquireMode.\n
SR_API(int) SR_SetMode(SRCAM srCam, int mode)
{
	return 0;
}

//!Returns the current mode setting. It is an ored value of the enumerator #AcquireMode.
//!\sa SR_SetMode\n
SR_API(int) SR_GetMode(SRCAM srCam)
{
	return 0;
}

//!SR_Acquire() triggers image acquisition on the camera and transfers the data from the camera to the PC.
//!The acquired images can be retrieved with SR_GetImage().
//!After this function normally the spherical coordinates are transformed to cartesian coordinates.
//!for that purpose use one of the following functions:\n
//!SR_CoordTrfUint16(), SR_CoordTrfFlt(),SR_CoordTrfDbl()\n
//!The #AcquisitionMode can be set with the function SR_SetMode().
//!\return the number of transfered bytes or a negative number if failed
//!\sa SR_GetImageList()
SR_API(int) SR_Acquire(SRCAM srCam)
{
	return 0;
}

//!Transforms spherical coordinates to cartesian coordinates.
//!The pointers x,y,z are unsigned short arrays of the length SR_GetRows() * SR_GetCols().
//!The result values are in mm.
//!The pitches is the distance in byte from one value to the next. Default value is sizeof(unsigned short)=2
//!\warning works only correct if AM_SHORT_RANGE is not set
//!\sa SR_CoordTrfFlt()
SR_API(int) SR_CoordTrfUint16(SRCAM srCam, short *x, short *y, unsigned short *z, int pitchX,int pitchY,int pitchZ)
{
	return 0;
}

//!Transforms spherical coordinates to cartesian coordinates.
//!The pointers x,y,z are float arrays of the length SR_GetRows() * SR_GetCols().
//!The pitches is the distance in byte from one value to the next. Default value is sizeof(float)=4
//!The memory management of x,y,z is handled by the user.
//!SR_CoordTrfFlt() operates on range data stored in the range buffer.
//!So if this data is modified by the user, the results of the SR_CoordTrfFlt() will be affected
//!This allows the user to perform processing on the range data before the coordinate transformation is calculated.
SR_API(int) SR_CoordTrfFlt(SRCAM srCam, float *x,float *y,float *z, int pitchX,int pitchY,int pitchZ)
{
	return 0;
}

//!Transforms spherical coordinates to cartesian coordinates.
//!The pitches is the distance in byte from one value to the next. Default value is sizeof(double)=8
//!The pointers x,y,z are double arrays of the length SR_GetRows() * SR_GetCols().
//!\warning works only correct if AM_SHORT_RANGE is not set
//!\sa SR_CoordTrfFlt()
SR_API(int) SR_CoordTrfDbl(SRCAM srCam, double *x,double *y,double *z, int pitchX,int pitchY,int pitchZ)
{
	return 0;
}

//!Full documentation of the mode is not provided to the customer.
//!- To force the check to toe last available version set the mode to 3
//!- if mode = 0 it only does the check if the last check has been done a long time before
//!- if mode & 1 it always checks the version for new drivers
//!-if mode & 2 it also informs about version entries with no info tag
SR_API(int) SR_CheckForNewDllVersion(int mode)
{
	return 0;
}

//!returns the version of this libusbSR library.
SR_API(int) SR_GetVersion(unsigned short version[4])
{
	return 0;
}

//!SR_SetCallback() can set a user callback function of type \ref SR_FuncCB that is called on special events.
//!Such special events can be:
//! - image buffers will be/have been changed.
//! - a message should be displayed to the user
//! - a register or buffer changes in the camera dialog box
//! - etc. (for a list of all events look at \ref SR_FuncCB)
//!This allows the main program to reallocate image pointers
//!and enter e.g. critical sections to avoid access of freed buffers.
//!
//!\return this function returns the previous function pointer.
//! This gives the possibility to forward not handled message to the default handling
//! \sa \ref SR_FuncCB, SR_GetDefaultCallback()
SR_API (SR_FuncCB*) SR_SetCallback(SR_FuncCB* cb);

//!\return the default callback function.
//! This gives the possibility to forward unhandled message to the API default handling function.
//! \sa \ref SR_FuncCB, SR_SetCallback()
SR_API (SR_FuncCB*) SR_GetDefaultCallback();

//!Returns a identification string of the device:
//!The string is formated as:
//!\verbatim
//!"VendorID:0x%04x, ProductID:0x%04x, Manufacturer:'%s', Product:'%s'" e.g.:
//!VendorID:0x0852, ProductID:0x0071, Manufacturer:'CSEM SA', Product:'3D-SR2 16Bit'
//!\endverbatim
SR_API(int) SR_GetDeviceString(SRCAM srCam, char* buf, int buflen)
{
	return 0;
}

//!sets the timeout in ms. This timeout is used at reading and writing on USB port.
SR_API(void) SR_SetTimeout(SRCAM srCam, int ms)
{
	return;
}

//!returns the number of rows the camera will deliver
SR_API(unsigned int) SR_GetRows(SRCAM srCam)
{
	return 144;
}

//!returns the number of columns the camera will deliver
SR_API(unsigned int)  SR_GetCols(SRCAM srCam)
{
	return 176;
}

//!Returns the pointer to the idx-th image. The idx has a value from 0 to SR_GetImageList()-1.
//!Details about which image contains what kind of data can be extracted with SR_GetImageList()
//!Usually index is 0 for distance image and 1 for amplitude image
//!\sa SR_GetRows(), SR_GetCols(), SR_GetImageList(), _ImgEntry::ImgType
SR_API(void*) SR_GetImage(SRCAM srCam, unsigned char idx)
{
	return NULL;
}

//!Returns the number of images and a pointer to an imgEntryArray that can be accessed with SR_GetImage().
//!\sa  SR_GetRows(), SR_GetCols(), SR_GetImage()
SR_API(int) SR_GetImageList(SRCAM srCam, ImgEntry** imgEntryArray)
{
	return 0;
}

//!Sets the Integration time of the camera
//!The intTime is a value from 0 to 255.
//!The integration time in ms depends on the camera:
//!
//! <TABLE>
//! <tr><td><tt> SR2A           </tt></td><td><tt> intTime*0.256 ms     </tt></td></tr>
//! <tr><td><tt> SR2B,SR3k </tt></td><td><tt> (intTime+1)*0.200 ms </tt></td></tr>
//! <tr><td><tt> SR4k </tt></td><td><tt> 0.300ms+(intTime)*0.100 ms </tt></td></tr>
//! </TABLE>
//!
//! \sa SR_GetIntegrationTime()
SR_API(int) SR_SetIntegrationTime(SRCAM srCam, unsigned char intTime)
{
	return 0;
}

//!Gets the Integration time of the camera
//!The intTime is a value from 0 to 255.
//!The integration time in ms depends on the camera
//! \sa SR_SetIntegrationTime()
SR_API(unsigned char) SR_GetIntegrationTime(SRCAM srCam)
{
	return 0;
}

//!Sets the Amplitude Threshold of the camera.
//!The default amplitude threshold is 0. Setting this value will set all distance values to 0 if
//!their amplitude is lower than the amplitude threshold
//! \sa SR_GetAmplitudeThreshold()
SR_API(int) SR_SetAmplitudeThreshold(SRCAM srCam, unsigned short val)
{
	return 0;
}

//!Gets the Amplitude Threshold of the camera
//! \sa SR_SetAmplitudeThreshold()
SR_API(unsigned short) SR_GetAmplitudeThreshold(SRCAM srCam)
{
	return 0;
}

//!Sets the modulation of the LEDs that will changes the measurement range.
//!\verbatim
//!30MHz ->  5.0 m
//!21MHz ->  7.1 m
//!20MHz ->  7.5 m
//!19MHz ->  7.9 m
//!\endverbatim
//!The supported frequencies depend on the camera
//!\return this function returns a negative value if it fails.
SR_API(int) SR_SetModulationFrequency(SRCAM srCam, ModulationFrq modFrq)
{
	return 0;
}

//!Gets the modulation frequency of the LEDs. This determins the measurement range.
//!\sa SR_SetModulationFrequency()
SR_API(ModulationFrq) SR_GetModulationFrequency(SRCAM srCam)
{
	return 0;
}

//!Sets the Distance Offset. This function should only be used on SR2 cameras.
//!On newer cameras the distance offset correction is handeled with a camera specific calibration file.
//!\sa SR_GetDistanceOffset()
SR_API(int) SR_SetDistanceOffset(SRCAM srCam, unsigned short distOfs)
{
	return 0;
}

//!Gets the Distance Offset.  This function should only be used on SR2 cameras.
//!On newer cameras the distance offset correction is handeled with a camera specific calibration file.
//!\sa SR_SetDistanceOffset()
SR_API(unsigned short) SR_GetDistanceOffset(SRCAM srCam)
{
	return 0;
}

//!Turns the Autoillumination on/off and set the desired parameters.
//!if <tt>minIntTime=0xff</tt> the Autoillumination is turned off.<br>
//!good values are 5,255,10,45<br>
//!The function sets the integration time to a value between <tt>minIntTime</tt> and <tt>maxIntTime</tt>.<br>
//!Therefore a special position is searched in the intensity histogram (reduced from 16bit to 8bit) described as <tt>percentOverPos</tt>.<br>
//!\image html autoillum.png
//! <tt>percentOverPos</tt> (values 0-100) is the x-position of the histogram where <tt>percentOverPos</tt> percent of the points
//!are above. The integration time is adapted slowly to move this position to the <tt>desiredPos</tt>(value 0-255)intensity value.
SR_API(int) SR_SetAutoIllumination(SRCAM srCam, unsigned char minIntTime, unsigned char maxIntTime,
                                   unsigned char percentOverPos, unsigned char desiredPos)
								   {
	return 0;
}

//!Sets a desired register value of the camera. The register values are given to the customer only for special usages.
//! @param srCam a valid device id returned by the SR_Open()
//! function.
//! @param reg the register of the descriptor to write to.
//! @param val the value to send to the given descriptor.
//! @return the number of bytes sent to the camera, which should be 2,
//! or a negative number if an error occurs.
//! -1 wrong device
//! -2 error in request frame (usb_bulk_write)
//!\sa SR_GetReg()
//!\warning <b>This must not be used without explicit instructions from Mesa-Imaging</b>
SR_API(int) SR_SetReg(SRCAM srCam, unsigned char reg, unsigned char val)
{
	return 0;
}

//!Returns a desired register value of the camera.
//!\sa SR_SetReg()
SR_API(unsigned char)  SR_GetReg(SRCAM srCam, unsigned char reg)
{
	return 0;
}

//!Returns the serial number (if existing) of the camera
SR_API(unsigned int) SR_ReadSerial(SRCAM srCam)
{
	return 0;
}
//!@}

//#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif /* DUMMYSR */
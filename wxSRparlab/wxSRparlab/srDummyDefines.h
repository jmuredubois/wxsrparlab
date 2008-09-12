/*
 * SR_DummyDefines.h
 * The goal of this dummy is to allow the build of an app
 * on systems where the libusbSR software was not installed.
 * The functions are noop-ed or replaced by appropriate operations.
 *
 * @author: James Mure-Dubois
 * @version: 2008.09.12
 */
/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Copyright (c) 2008 by MESA Imaging SA,                                */
/*                     http://www.mesa-imaging.ch                         */
/*------------------------------------------------------------------------*/
// $Author: tza $
//    $URL: svn://mesasrv01/libusbSR/trunk/libusbSR/definesSR.h $
//    $Rev: 505 $
//   $Date: 2008-07-22 18:18:16 +0200 (Di, 22 Jul 2008) $
/*------------------------------------------------------------------------*/
#pragma once
#if !defined(_WIN32) && !defined(MATLAB_PREPROC)
typedef short          __wchar_t;
typedef int            HWND;
typedef unsigned short WORD;
#endif

//!\addtogroup libusbSR
//!@{

//!Modulation frequency for function SR_SetModulationFrequency()
//!\warning some frequencies are only for internal usage and not released for customer.
enum ModulationFrq {MF_40MHz=0, //!<SR3k: maximal range 3.75m
                    MF_30MHz,   //!<SR3k, SR4k: maximal range 5m
                    MF_21MHz,   //!<SR3k: maximal range 7.14m
                    MF_20MHz,   //!<SR3k: maximal range 7.5m
                    MF_19MHz,   //!<SR3k: maximal range 7.89m
                    MF_Reserved0,
                    MF_Reserved1,
                    MF_Reserved2,
                    MF_LAST};

//!Enum used in the function SR_Acquire()
//!the proposed default setting are:
//! - SR2,SR3k: AM_COR_FIX_PTRN|AM_MEDIAN
//! - SR4k: AM_COR_FIX_PTRN|AM_CONV_GRAY|AM_DENOISE_ANF
//!\warning some settings are only for internal usage and not released for customer.
enum AcquireMode {
  AM_COR_FIX_PTRN=0x01,       //!< turns on fix pattern noise correction <b>this should always be enabled for good distance measurement</b>
  AM_MEDIAN=0x04,             //!< turns on a 3x3 median filter
  AM_Reserved0=0x08,
  AM_CONV_GRAY=0x10,          //!< Converts the amplitude image to a gray image like from a normal camera
  AM_Reserved1=0x20,
  AM_Reserved2=0x40,
  AM_SHORT_RANGE=0x80,        //!< For sr4k: this flag results in more precise coordinate transformations for small distances(<1m) <b>works only for SR_CoordTrfFlt()</b>
  AM_CONF_MAP=0x0100,         //!< For sr4k: process a confidencemap. this map is accesssible with SR_GetImageList
  AM_HW_TRIGGER=0x0200,       //!< For sr4k: Acquisition starts, when a Hardware Trigger is received (AM_SW_TRIGGER must also be set)
  AM_SW_TRIGGER=0x0400,       //!< For sr4k: Light source is only turned on, when an image is requested
  AM_DENOISE_ANF=0x0800,      //!< For sr4k: Turns on the 3x3 hardware adaptive neighborhood filter
  AM_Reserved3=0x1000,
};


#pragma pack(push,r1,4)
//!structure used in \ref SR_GetImageList()
typedef struct _ImgEntry
{
  //!this enum describes the image type received from the function SR_GetImageList()
  //!the images types relevant for the customer are IT_DISTANCE, IT_AMPLITUDE and IT_CONF_MAP
  //!Currently the data type of these images are DT_USHORT but may alter in the future.
  //!The 

#ifndef MATLAB_PREPROC
  enum ImgType {
    IT_DISTANCE,  //!< distance image. Values between 0..0xffff (0xffff=maximal range which depends on modulation frequency)
                  //!< the distance image is affected by the #AcquireMode flags.
    IT_AMPLITUDE, //!< amplitude image.
    IT_INTENSITY, //!< intensity image (only for SR2).
    IT_Reserved0,
    IT_Reserved1,
    IT_Reserved2,
    IT_Reserved3,
    IT_Reserved4,
    IT_CONF_MAP,  //!< confidence map image. Valu 0xffff is highest confidence.
    IT_UNDEFINED, //!< any other kind of image
    IT_LAST       //!< useful last entry
  };
  //!this enum describes the data type of the image
  enum DataType {
    DT_NONE,    //!< no data type
    DT_UCHAR,   //!< unsigned char data type
    DT_CHAR,    //!< char data type
    DT_USHORT,  //!< unsigned short data type
    DT_SHORT,   //!< short data type
    DT_UINT,    //!< unsigned int data type
    DT_INT,     //!< int data type
    DT_FLOAT,   //!< float data type
    DT_DOUBLE   //!< double data type
  };
#endif
  ImgType  imgType;  //!< image type
  DataType dataType; //!< image data type
  void* data;        //!< pointer to memory
  WORD  width;       //!< width of the image
  WORD  height;      //!< height of the image
}ImgEntry;//!<typedef struct _ImgEntry

#pragma pack(pop,r1)

#ifndef MATLAB_PREPROC
#ifndef __cplusplus
struct _CMesaDevice; //forward declaration
//!handle to camera
typedef struct _CMesaDevice SRCAM;
#else
class CMesaDevice; //forward declaration
typedef CMesaDevice* SRCAM;
#endif
#endif
//!function type used in SR_SetCallback()
//!\param funcData is the user data pointer that was set with SR_SetCallback.
//!\param msg is the received message of type CM_XXX as listed below
//!\param param is a message specific parameter as listed below
//!\param data is a message specific pointer as listed below
//!
//!defines used for the callback function SR_SetCallback() are:\n
//!
//! <HR> \par message: CM_CHANGING and CM_CHANGED
//!CM_CHANGING is sent before modification of register values or filtering functions\n
//!CM_CHANGED is sent after modification of register values or filtering functions\n
//!\par parameters:
//!CP_AFFECTS_BUFFER is set if the buffer and therefore the images pointers will change or changed
//!\par data:
//!no meaning
//!
//! <HR> \par message: CM_MSG_DISPLAY
//!CM_MSG_DISPLAY is sent to display a message
//!\par parameters:
//!the param is a kind ored with a category: \ref SRMsgKind|\ref SRMsgCategory.\n
//!the default handling will display Messages as followed:
//! - MK_DEBUG_STRING OutputDebugString
//! - MK_BOX_INFO open a ASTERISK MessageBox
//! - MK_BOX_WARN open a WARNING MessageBox
//! - MK_BOX_ERR open a HAND MessageBox
//!\par data:
//!the data is a pointer to the string to display
//!
//! <HR> \par message: CM_PROGRESS
//!CM_PROGRESS is sent to display a progress bar window.
//!the default handling will open a window with a progress bar and close this window on the param CP_DONE.
//!\par parameters:
//!the param split in a LOWORD and HIWORD part.\n
//! - LOWORD: indicates that type of progress message. E.g. CP_FLASH_ERASE means that the flash is beeing erased..
//! - HIWORD: indicates the percentage (0-100) of the grogress.
//!
//! <HR>
typedef int (SR_FuncCB)(SRCAM srCam, unsigned int msg, unsigned int param, void* data);

//defines used for the callback function SR_SetCallback
#define CM_MSG_DISPLAY  0x0001 //!< \#define for \ref SR_FuncCB
//------------------------------------------
#define CM_CHANGING     0x0010 //!< \#define for \ref SR_FuncCB
#define CM_CHANGED      0x0011 //!< \#define for \ref SR_FuncCB
//param
#define CP_AFFECTS_BUFFER 0x0001 //!< \#define for \ref SR_FuncCB
//------------------------------------------
#define CM_PROGRESS       0x0020 //!< \#define for \ref SR_FuncCB
//param LOWORD
#define CP_FLASH_ERASE  0x0000 //!< \#define for \ref SR_FuncCB
#define CP_FLASH_WRITE  0x0001 //!< \#define for \ref SR_FuncCB
#define CP_FLASH_READ   0x0002 //!< \#define for \ref SR_FuncCB
#define CP_FLASH_REBOOT 0x0003 //!< \#define for \ref SR_FuncCB
#define CP_DONE         0x0004 //!< \#define for \ref SR_FuncCB
//param HIWORD
//       progress value (0-100)
//------------------------------------------

//!Message Kind: enum for function \ref SR_FuncCB with message type CM_MSG_DISPLAY
typedef enum SRMsgKind
{
  MK_DEBUG_STRING=0x00,//!< OutputDebugString
  MK_BOX_INFO    =0x01,//!< MessageBox  ASTERISK
  MK_BOX_WARN    =0x02,//!< MessageBox  WARNING
  MK_BOX_ERR     =0x03 //!< MessageBox  HAND
};

//!Message Category: enum for function \ref SR_FuncCB with message type CM_MSG_DISPLAY
typedef enum SRMsgCategory
{
  MC_GENERAL   =0x0000,//!< general category
  MC_DLL_FUNC  =0x0100,//!< DllMain,DllFunction messages
  MC_CAM       =0x0200,//!< Camera, USB Info messages
  MC_USB       =0x0300,//!< USB messages -> is logged to NIL because it slowed down the frame rate!
  MC_CONFIG    =0x0400,//!< configuration messages (open, read registry etc.)
  MC_FIRMWARE  =0x0500,//!< Firmware upload messages
  MC_XML       =0x0600,//!< XML messages
  MC_CAM_DLG   =0x0700,//!< A CDlgCamSettings messages
  MC_HTTP      =0x0800,//!< HTTP messages
  MC_ETH       =0x0900,//!< Ethernet messages
  MC_1394      =0x0a00,//!< Firewire messages
  MC_FILEIO    =0x0b00 //!< File IO messages
};

//#pragma pack(show)
#pragma pack(push,r1,4)
//#pragma pack(show)

#ifndef MATLAB_PREPROC
//!Enum and struct used in the function SR_SetParam()
//! \intDoc{structure}
typedef struct _ParamVal
{
#ifndef MATLAB_PREPROC
  //!enum to describe which SR_SetParam() function should be executed
  enum ID {
    PR_ANF=1,        //!< adaptive neighborhood filter
    PR_SCATFILT,     //!< scattering filter
    PR_TEMPORAL_IIR, //!< temporal IIR filter
    PR_PSEUDO_FRAME, //!< pseudo frame acquisition/processing mode
    PR_EDGE_AMP_FLT, //!< Edge Amplitude Filter
    PR_CONF_MAP      //!< create a confidence map (new image in the image list)
  };
  //!enum to describe what kind of data is in the structure
  enum ParamType {
    PT_UINT8,     //!< unsigned char
    PT_INT8,      //!< char
    PT_UINT16,    //!< unsinged short
    PT_INT16,     //!< short
    PT_UINT32,    //!< unsinged int
    PT_INT32,     //!< int
    PT_FLOAT,     //!< float
    PT_DOUBLE,    //!< double
    PT_WCHAR,     //!< wide char
    PT_PTR_UINT8, //!< pointer to unsigned char
    PT_PTR_WCHAR  //!< pointer to wide char
  };
  
  ParamType t;   //!< the paramtype of this entry
  union
  {
    unsigned char  u8;
    char           s8;
    unsigned short u16;
    short          s16;
    unsigned int   u32;
    int            s32;
    float          f32;
    double         f64;
    __wchar_t      wc16;
    unsigned char* pu8;
    __wchar_t*     pwc16;
  };
#else
  ParamVal_ParamType t;   //!< the paramtype of this entry
  double             data;
#endif
}ParamVal;//!<typedef struct _ParamVal
#pragma pack(pop,r1)
#endif
//#pragma pack(show)
//!@}

//!\ingroup libusbSRinternal
//! \intDoc{enumeator}
enum ReadMode{RD_FLASH=0,  //!<reads flash memory by sending a usb_control_msg (USB only, up to 64 bytes)
              RD_USB_BULK  //!<reads bulk data from the USB device the address is the endpoint value (0x86 for sr3k,sr4k)
             };

//! \intDoc{enumeator}
enum WriteMode{WR_FLASH=0,  //!<writes flash memory by sending a usb_control_msg (USB only, up to 64 bytes)
               WR_USB_BULK  //!<writes bulk data to the USB device the address is the endpoint value (0x02 for sr3k,sr4k)
               };

//! \intDoc{enumeator}
enum CamType{CT_UNKNOWN=0,CT_SR2A,CT_SR2B,CT_SR3K,CT_SR4K,CT_LAST};
//!@}

//!\ingroup libusbSRdepreciated
//!this is a deprecated message callback
//!\deprecated
typedef int (MessageCB)(int level, const char* string);


/**
 * Class camVtkView :
 * a helper class for VTK visualization
 * @author : James Mure-Dubois
 * @Version :0.1.0.0 2008.07.22
 */

# pragma once

//#include "windows.h"

// to stop complaints about vtkConfigure.h, include $(JMU_VTKBINBASE)
// first include the required header files for the vtk classes we are using
#include "vtkCubeSource.h"			// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkLineSource.h"			// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkPolyDataMapper.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderWindow.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkDataSetMapper.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderWindowInteractor.h"	// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderer.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkFloatArray.h"		// requires $(JMU_VTKSRCBASE)/Common
#include "vtkDoubleArray.h"		// requires $(JMU_VTKSRCBASE)/Common

#include "vtkStructuredGridGeometryFilter.h"	// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkStructuredGrid.h"	// requires $(JMU_VTKSRCBASE)/Filtering
#include "vtkUnstructuredGrid.h"	// requires $(JMU_VTKSRCBASE)/Filtering
#include "vtkDataSetToStructuredGridFilter.h"	// requires $(JMU_VTKSRCBASE)/Filtering
#include "vtkQuad.h"	// requires $(JMU_VTKSRCBASE)/Filtering
#include "vtkXMLStructuredGridReader.h"	// requires $(JMU_VTKSRCBASE)/IO
#include "vtkProperty.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkOutlineFilter.h"	// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkLookupTable.h"		// requires $(JMU_VTKSRCBASE)/Common
#include "vtkScalarBarActor.h"	// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkExtractGrid.h"		// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkAxes.h"			// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkVectorText.h"		// requires $(JMU_VTKSRCBASE)/Hybrid
#include "vtkFollower.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkCamera.h"			// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkLight.h"			// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkPoints.h"			// requires $(JMU_VTKSRCBASE)/Common
#include "vtkPointData.h"		// requires $(JMU_VTKSRCBASE)/Filtering
//#include "vtkHedgeHog.h"		// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkStructuredPoints.h"	// requires $(JMU_VTKSRCBASE)/Filtering
#include "vtkTransform.h"		// requires $(JMU_VTKSRCBASE)/Common
#include "vtkTransformFilter.h"	// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkMatrix4x4.h"		// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkStructuredGridWriter.h"	// requires $(JMU_VTKSRCBASE)/Graphics

//#include "libusbSR.h"
//#include "libSRparlab.h" // for SRCAM
#include "wxSRparlab.h"
#include "CMainWnd.h"

#define TIXML_USE_TICPP
#include "ticpp.h" //Open source XML parser

/**
 * Class CViewSrVtk :
 * a class for VTK visualization
 */
class CamVtkView{
public:
	CamVtkView(int vtkSub, vtkRenderWindow* ParRenWin, vtkLookupTable* LUT);
	~CamVtkView();

	int setVtkSub(int vtkSub);
	int getVtkSub();
	int setTrfMat(char* fn);

	#ifdef JMU_TGTFOLLOW 
	  int setTgtFile(char* fn);
	  int updateTarget(float x, float y, float z);
	  int updateTarget(); //overloaded operator to reset to camera center
	  int updateTarget(float *ptsF, int nCoord);
	#endif

	//int updateTOFcurrent(SRCAM sr, SRPARLAB srPL, int vtkSub){};
	int updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x);
	int updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x, unsigned short* amp);
	//int updateTOFcurrent(SRCAM sr, SRPARLAB srPL, int vtkSub, char* fname){};

	//int updateTOFbg(SRCAM sr, SRPARLAB srPL, int vtkSub);
	//int updateTOFbg(SRCAM sr, SRPARLAB srPL, int vtkSub, char*fname);

	int changeDepthRange(float minVal, float maxVal);
	int changeAmpRange(float minAmp, float maxAmp);
	void SetDepthLUT(vtkLookupTable* LUT);
	void SetGrayLUT(vtkLookupTable* LUT);
	void SetPlainLUT(vtkLookupTable* LUTr, vtkLookupTable* LUTg, vtkLookupTable* LUTb, vtkLookupTable* LUTw, vtkLookupTable* LUTk);
	void hideDataAct(bool doHide);
	void setDataActColorRGB( double r, double g, double b);
	void setDataMapperColorDepth();
	void setDataMapperColorGray();
	void setDataMapperColorR();
	void setDataMapperColorG();
	void setDataMapperColorB();
	void setDataMapperColorW();
	void setDataMapperColorK();

private:
	static const int _vtkSubMax = NUMCAMS ; // VTKSUBMAX undeclared identifier bug 20080114
  int	_vtkSub; //!< unique vtk index identifier
  vtkRenderWindow			*renWin;		//!< render window
  vtkRenderer				*renderer;		//!< renderer

  vtkCubeSource			*srCube;			//!< cube to represent TOF camera
  vtkPolyDataMapper		*srCubeMapper;		//!< mapper for TOF camera
  vtkActor				*srCubeActor;		//!< actor for TOF camera
  vtkVectorText			*srLabel;			//!< TOF camera label (text)
  vtkPolyDataMapper		*srLabelMapper;		//!< mapper for TOF camera label (text)
  vtkFollower				*srLabelActor;		//!< actor for TOF camera label (text)
  int		addSrCam(); //!< adds the sr cam actor to the current view
  int		freeSrCam(); //!< adds the sr cam actor to the current view
  #ifdef JMU_TGTFOLLOW 
    vtkLineSource		*tgtLine; //! target line
	vtkPolyDataMapper	*tgtLineMapper; //!
	vtkActor			*tgtLineActor;
	vtkPoints			*tgtQuadPoints;
	vtkQuad				*tgtQuad;
	vtkUnstructuredGrid	*tgtQuadGrid;
	vtkDataSetMapper	*tgtQuadMapper;
	vtkActor			*tgtQuadActor;
	int addTgtAct();
	int freeTgtAct();
  #endif

  vtkLookupTable	*depthLUT;	//!< LUT for depth data display
  vtkLookupTable	*grayLUT;	//!< LUT for depth data display
  vtkLookupTable	*rLUT;	//!< LUT for depth data display
  vtkLookupTable	*gLUT;	//!< LUT for depth data display
  vtkLookupTable	*bLUT;	//!< LUT for depth data display
  vtkLookupTable	*wLUT;	//!< LUT for depth data display
  vtkLookupTable	*kLUT;	//!< LUT for depth data display

  vtkStructuredGrid					*data;			//!< read TOF depth data
  vtkPoints							*dataPoints;    //!< points vtk objects to store SR dara
  vtkStructuredGridGeometryFilter	*pdata;			//!< geometry filter to obtain PolyData
  vtkPolyDataMapper					*dataMapperZ;	//!< mapper for TOF data
  vtkPolyDataMapper					*dataMapperAmp;	//!< mapper for TOF data
  vtkActor							*dataActor;		//!< actor for TOF data

  // Create a float array which represents the points.
  vtkFloatArray* pcoords;
  vtkFloatArray* dData;
  vtkFloatArray* aData; //amplitude

  int addDataAct();
  int freeDataAct();

  
  short		*_x;
  short		*_y;
  unsigned short *_z;

  short		*_xBG;
  short		*_yBG;
  unsigned short *_zBG;

  int allocXYZ(int rows, int cols);
  int freeXYZ();


  int allocXYZbg(int rows, int cols);
  int freeXYZbg();
  int addBGDataAct();
  int freeBGDataAct();

  vtkStructuredGrid					*BGdata;			//!< read TOF depth data
  vtkPoints							*BGdataPoints;    //!< points vtk objects to store SR dara
  vtkStructuredGridGeometryFilter	*BGpdata;			//!< geometry filter to obtain PolyData
  vtkPolyDataMapper					*BGdataMapperZ;	//!< mapper for TOF data
  vtkActor							*BGdataActor;		//!< actor for TOF data

  // Create a float array which represents the points.
  vtkFloatArray* BGpcoords;
  vtkFloatArray* BGdData;

  vtkTransform			*camTran;			//!< transformation between camera coordinates
  vtkMatrix4x4			*camTranMat;		//!< 4D transformation matrix
  vtkTransformFilter	*camTranFilter;	//!< filter for transformation between camera coordinates
  vtkTransformFilter	*BGcamTranFilter;	//!< filter for transformation between camera coordinates

  vtkStructuredGridWriter	*dataWriter;	//! Structured Grid datawriter
  vtkStructuredGridWriter	*BGdataWriter;	//! Structured Grid datawriter
};
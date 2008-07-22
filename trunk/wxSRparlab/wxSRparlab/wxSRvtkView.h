/**
 * Class CViewSrVtk :
 * a class for VTK visualization
 * @author : James Mure-Dubois
 * @Version : 1.0.0.0 2007.05.30
 */

# pragma once

//#include "windows.h"

// to stop complaints about vtkConfigure.h, include $(JMU_VTKBINBASE)
// first include the required header files for the vtk classes we are using
#include "vtkCubeSource.h"			// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkPolyDataMapper.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderWindow.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderWindowInteractor.h"	// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderer.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkFloatArray.h"		// requires $(JMU_VTKSRCBASE)/Common
#include "vtkDoubleArray.h"		// requires $(JMU_VTKSRCBASE)/Common

#include "vtkStructuredGridGeometryFilter.h"	// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkStructuredGrid.h"	// requires $(JMU_VTKSRCBASE)/Filtering
#include "vtkDataSetToStructuredGridFilter.h"	// requires $(JMU_VTKSRCBASE)/Filtering
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

class CamVtkView; // a class to represent ONE camera of the scene

/**
 * Class CViewSrVtk :
 * a class for VTK visualization
 */
class CViewSrVtk{
public:
	CViewSrVtk(wxFrame* pWnd);
	~CViewSrVtk();

	//int updateTOFcurrent(SRCAM sr, SRPARLAB srPL, int vtkSub){};
	//int updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x, int vtkSub);
	int updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x, unsigned short* amp, int vtkSub);
	//int updateTOFcurrent(SRCAM sr, SRPARLAB srPL, int vtkSub, char* fname){};

	//int updateTOFbg(SRCAM sr, SRPARLAB srPL, int vtkSub);
	//int updateTOFbg(SRCAM sr, SRPARLAB srPL, int vtkSub, char*fname);

	int changeDepthRange(float minVal, float maxVal);
	int changeAmpRange(float minAmp, float maxAmp);
	int hideDataAct(int vtkSub, bool doHide);
	int setDataActColorRGB(int vtkSub, double r, double g, double b);
	int setDataMapperColorDepth(int vtkSub);
	int setDataMapperColorGray(int vtkSub);
	int setDataMapperColorR(int vtkSub);
	int setDataMapperColorG(int vtkSub);
	int setDataMapperColorB(int vtkSub);
	int setDataMapperColorW(int vtkSub);
	int setDataMapperColorK(int vtkSub);
	int Render();

private:
	static const int _vtkSubMax = NUMCAMS ; // VTKSUBMAX undeclared identifier bug 20080114
  vtkRenderWindow			*renWin;		//!< render window
  vtkRenderer				*renderer;		//!< renderer
  vtkRenderWindowInteractor *iren;			//!< renderer interactor
  vtkCamera					*cam0;			//! camera for view
  vtkLight					*light0;		//! light (ambient)
  std::vector<CamVtkView*>	cameras;

  vtkCubeSource			**srCube;			//!< cube to represent TOF camera
  vtkPolyDataMapper		**srCubeMapper;		//!< mapper for TOF camera
  vtkActor				**srCubeActor;		//!< actor for TOF camera
  vtkVectorText			**srLabel;			//!< TOF camera label (text)
  vtkPolyDataMapper		**srLabelMapper;		//!< mapper for TOF camera label (text)
  vtkFollower				**srLabelActor;		//!< actor for TOF camera label (text)
  int		addSrCam(int vtkSub); //!< adds the sr cam actor to the current view
  int		freeSrCam(int vtkSub); //!< adds the sr cam actor to the current view

  vtkCubeSource			*srBox;				//!< box to represent TOF camera FoV
  vtkOutlineFilter		*srBoxOutline;		//!< FoV box represented as outline
  vtkPolyDataMapper		*srBoxMapper;		//!< mapper for FoV box
  vtkActor				*srBoxActor;		//!< actor for FoV box
  int		addSrBox(int xLen, int yLen, int zLen, int xC, int yC, int zC); //!< adds the FoV box actor to the current view
  int		freeSrBox(); //!< adds the FoV box actor to the current view

  vtkAxes			*axes;			//!< axes for display
  vtkPolyDataMapper	*axesMapper;	//!< mapper for axes
  vtkActor			*axesActor;		//!< actor for axes
  int		addSrAxes(); //!< adds the sr cam actor to the current view
  int		freeSrAxes(); //!< adds the sr cam actor to the current view

  vtkLookupTable	*depthLUT;	//!< LUT for depth data display
  int addDepthLUT();
  int freeDepthLUT();
  vtkLookupTable	*grayLUT;	//!< LUT for depth data display
  int addGrayLUT();
  int freeGrayLUT();
  vtkLookupTable	*rLUT;	//!< LUT for depth data display
  vtkLookupTable	*gLUT;	//!< LUT for depth data display
  vtkLookupTable	*bLUT;	//!< LUT for depth data display
  vtkLookupTable	*wLUT;	//!< LUT for depth data display
  vtkLookupTable	*kLUT;	//!< LUT for depth data display
  int addPlainLUT();
  int freePlainLUT();

  vtkScalarBarActor	*depthSca;  //!< scalar bar for depth values
  int addScalarBar();
  int freeScalarBar();

  vtkStructuredGrid					**data;			//!< read TOF depth data
  //vtkStructuredPoints	**spoints;
  vtkPoints							**dataPoints;    //!< points vtk objects to store SR dara
  vtkStructuredGridGeometryFilter	**pdata;			//!< geometry filter to obtain PolyData
  vtkPolyDataMapper					**dataMapper;	//!< mapper for TOF data
  //vtkHedgeHog *hedgehog;
  vtkActor							**dataActor;		//!< actor for TOF data
  //vtkExtractGrid	**subSample;	//!< used to subsample structured grid (for FASTER rendering)

  // Create a float array which represents the points.
  vtkFloatArray** pcoords;
  vtkFloatArray** dData;
  vtkFloatArray** aData; //amplitude

  int addDataAct(int vtkSub);
  int freeDataAct(int vtkSub);

  
  short		**_x;
  short		**_y;
  unsigned short **_z;

  short		**_xBG;
  short		**_yBG;
  unsigned short **_zBG;

  int allocXYZ(int rows, int cols, int vtkSub);
  int freeXYZ(int vtkSub);


  int allocXYZbg(int rows, int cols,int vtkSub);
  int freeXYZbg(int vtkSub);
  int addBGDataAct(int vtkSub);
  int freeBGDataAct(int vtkSub);

  vtkStructuredGrid					**BGdata;			//!< read TOF depth data
  vtkPoints							**BGdataPoints;    //!< points vtk objects to store SR dara
  vtkStructuredGridGeometryFilter	**BGpdata;			//!< geometry filter to obtain PolyData
  vtkPolyDataMapper					**BGdataMapper;	//!< mapper for TOF data
  vtkActor							**BGdataActor;		//!< actor for TOF data

  // Create a float array which represents the points.
  vtkFloatArray** BGpcoords;
  vtkFloatArray** BGdData;

  vtkTransform			**camTran;			//!< transformation between camera coordinates
  vtkMatrix4x4			**camTranMat;		//!< 4D transformation matrix
  vtkTransformFilter	**camTranFilter;	//!< filter for transformation between camera coordinates
  vtkTransformFilter	**BGcamTranFilter;	//!< filter for transformation between camera coordinates

  vtkStructuredGridWriter	**dataWriter;	//! Structured Grid datawriter
  vtkStructuredGridWriter	**BGdataWriter;	//! Structured Grid datawriter
};
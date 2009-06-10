/**
 * Class CViewSrVtk :
 * a class for VTK visualization
 * @author : James Mure-Dubois
 * @Version : 1.0.0.0 2007.05.30
 */

# pragma once

//#include "windows.h"
#include "wxSRparlab.h"
#include "CMainWnd.h"

#define JMU_ICPVTK // testing experimental ICP from VTK
#define JMU_KDTREEVTK // testing KDTREE from VTK

// to stop complaints about vtkConfigure.h, include $(JMU_VTKBINBASE)
// first include the required header files for the vtk classes we are using
#include "vtkCubeSource.h"			// requires $(JMU_VTKSRCBASE)/Graphics
#include "vtkPolyDataMapper.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderWindow.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderWindowInteractor.h"	// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRenderer.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkRendererCollection.h"		// requires $(JMU_VTKSRCBASE)/Rendering
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
#include "vtkTextActor.h"		// requires $(JMU_VTKSRCBASE)/Rendering
#include "vtkPointSet.h"			// for ICP or kdDist, when segmenting dataset
#include "vtkUnstructuredGrid.h"	// for ICP or kdDist, when segmenting dataset
#include "vtkSmartPointer.h"		// for ICP or kdDist, when segmenting dataset
#ifdef JMU_ICPVTK
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLandmarkTransform.h" //to set type to ridgid body
#endif
#ifdef JMU_KDTREEVTK
#include "vtkKdTree.h"
#endif



//#include "libusbSR.h"
//#include "libSRparlab.h" // for SRCAM


class CamVtkView; // a class to represent ONE camera of the scene

/**
 * Class CViewSrVtk :
 * a class for VTK visualization
 */
class CViewSrVtk{
public:
	CViewSrVtk(wxFrame* pWnd);
	CViewSrVtk(wxFrame* pWnd, int x, int y, int w, int h);
	~CViewSrVtk();

	int changeDepthRange(float minVal, float maxVal);
	int changeAmpRange(float minAmp, float maxAmp);
	int changeSegmRange(float minSegm, float maxSegm);
	vtkLookupTable* GetDepthLUT(){return depthLUT;};
	vtkLookupTable* GetGrayLUT(){return grayLUT;};
	vtkLookupTable* GetSegmLUT(){return segmLUT;};
	vtkLookupTable* GetRLUT(){return rLUT;};
	vtkLookupTable* GetGLUT(){return gLUT;};
	vtkLookupTable* GetBLUT(){return bLUT;};
	vtkLookupTable* GetWLUT(){return wLUT;};
	vtkLookupTable* GetKLUT(){return kLUT;};
	vtkRenderWindow* GetRenWin(){return renWin;};
	int Render();
	double timeRender();
	void setParallelProj(bool para);
	int setFpsTxt(char* txt);
	int setReadFpsTxt(char* txt);
	void hideSegmCbar(bool doHide);
	void hideAmplCbar(bool doHide);
	void hideDepthCbar(bool doHide);
#ifdef JMU_ICPVTK
	vtkStructuredGrid* icpCam(vtkStructuredGrid* source, vtkStructuredGrid* target);
#endif
#ifdef JMU_KDTREEVTK
	double		kdDist(std::vector<CamFrame*>* camFrms, int idxSrc, int srcField, int idxTgt, int tgtField, double res[3]);
	double		kdTreeEps(vtkPointSet* source, vtkPointSet* target, double res[3]);
#endif

private:
	static const int _vtkSubMax = NUMCAMS ; // VTKSUBMAX undeclared identifier bug 20080114
  vtkRenderWindow			*renWin;		//!< render window
  vtkRenderer				*renderer;		//!< renderer
  vtkRenderWindowInteractor *iren;			//!< renderer interactor

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
  vtkTextActor		*fpsTxtActor;	//!< actor for fps display
  vtkTextActor		*fpsReadTxtActor;	//!< actor for fps display

  vtkLookupTable	*depthLUT;	//!< LUT for depth data display
  int addDepthLUT();
  int freeDepthLUT();
  vtkLookupTable	*grayLUT;	//!< LUT for depth data display
  int addGrayLUT();
  int freeGrayLUT();
  vtkLookupTable	*segmLUT;	//!< LUT for depth data display
  int addSegmLUT();
  int freeSegmLUT();
  vtkLookupTable	*rLUT;	//!< LUT for depth data display
  vtkLookupTable	*gLUT;	//!< LUT for depth data display
  vtkLookupTable	*bLUT;	//!< LUT for depth data display
  vtkLookupTable	*wLUT;	//!< LUT for depth data display
  vtkLookupTable	*kLUT;	//!< LUT for depth data display
  int addPlainLUT();
  int freePlainLUT();

  vtkScalarBarActor	*depthSca;  //!< scalar bar for depth values
  int addScalarBarDepth();
  int freeScalarBarDepth();

  vtkScalarBarActor	*ampSca;  //!< scalar bar for ampl. values
  int addScalarBarAmp();
  int freeScalarBarAmp();
  vtkScalarBarActor	*segmSca;  //!< scalar bar for segm. values
  int addScalarBarSegm();
  int freeScalarBarSegm();
#ifdef JMU_ICPVTK
	vtkPointSet* _icpPtsSrc;
	vtkPointSet* _icpPtsTgt;
#endif
#ifdef JMU_KDTREEVTK
	vtkPointSet* _kdPtsSrc;
	vtkPointSet* _kdPtsTgt;
#endif

};
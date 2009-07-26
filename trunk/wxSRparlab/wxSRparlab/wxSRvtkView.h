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
#include "vtkWindowToImageFilter.h" // to capture a screenshot of the render window
#include "vtkPNGWriter.h"			// to write a screeenshot to PNG
#ifdef JMU_ICPVTK
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLandmarkTransform.h" //to set type to ridgid body
#include "vtkUnstructuredGridToPolyDataFilter.h"	// requires $(JMU_VTKSRCBASE)/Graphics
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
	void savePNGimg(char* fname);
	void savePNGimg(wxString fname);
#ifdef JMU_ICPVTK
	vtkStructuredGrid* icpWork(vtkPointSet* source, vtkPointSet* target, int icpIter, int icpTrlCM, double mat[16]);
	wxString icpFct(std::vector<CamFrame*>* camFrms, int idxSrc, int srcField, int idxTgt, int tgtField, int icpIter, int icpTrlCM, double mat[16]);
	void hideICPact(bool doHide);
	void setICPColorDepth();
	void setICPColorGray();
	void setICPColorSegm();
	void setICPActOpacity(double alpha);
	void setICPActRepPts();
	void setICPActRepSurf();
#endif
#ifdef JMU_KDTREEVTK
	double		kdDist(std::vector<CamFrame*>* camFrms, int idxSrc, int srcField, int idxTgt, int tgtField, double res[3], double thr, int inliers[2], bool blankBadPts);
	//double		kdDist(std::vector<CamFrame*>* camFrms, int idxSrc, int srcField, int idxTgt, int tgtField, double res[3], double thr, int inliers[2], bool blankBadPts, char* fprefix);
	double		kdTreeEps(vtkPointSet* source, vtkPointSet* target, double res[3], double thr, int inliers[2]);
	double		kdTreeEps(vtkPointSet* source, vtkStructuredGrid* src2Blank, vtkStructuredGrid* srcTRF2Blank, vtkPointSet* target, double res[3], double thr, int inliers[2]);
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
	vtkStructuredGrid*	_icpGrid; 
	//vtkStructuredGridGeometryFilter* _icpToPoly;
	vtkDataSetMapper*		_icpMapperZ;		//!< mapper for icp
	vtkDataSetMapper*		_icpMapperAmp;		//!< mapper for icp
	vtkDataSetMapper*		_icpMapperSegm;		//!< mapper for icp
    vtkActor*				_icpActor;			//!< actor for icp
	vtkTextActor* _icpTxtActor;	//!< actor for icpRes display
	void setIcpTxt(char* txt);
#endif
#ifdef JMU_KDTREEVTK
	vtkPointSet* _kdPtsSrc;
	vtkPointSet* _kdPtsTgt;
	vtkTextActor* _kdTxtActor;	//!< actor for kdDist display
	void setKdDistTxt(char* txt);
#endif

};

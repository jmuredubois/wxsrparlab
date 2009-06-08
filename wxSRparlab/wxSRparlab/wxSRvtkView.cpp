#include "wxSRparlab.h"
#include "wxSRvtkView.h"
#include "camVtkView.h"

/**
 * Constructor for CViewSrVtk
 *
 */
CViewSrVtk::CViewSrVtk(wxFrame* pWnd)
{
	int x=300, y=50, w=800, h=600;
	CViewSrVtk(pWnd, x, y, w, h);
};
/**
 * Constructor for CViewSrVtk
 *
 */
CViewSrVtk::CViewSrVtk(wxFrame* pWnd, int x, int y, int w, int h)
{

	// instantiate class fields
	// setup the parent window
	renWin = vtkRenderWindow::New();
    renWin->SetParentId((void*)pWnd);
	// add a renderer
	renderer = vtkRenderer::New();
	renWin->AddRenderer(renderer);
	//renderer->LightFollowCameraOn();
	//renderer->AutomaticLightCreationOff();
	// add an interactor
    iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);


	  //we start the interactor so that event will be handled
    renWin->Render();

	// create a depth LUT
	addDepthLUT();
	addGrayLUT();
	addSegmLUT();
	addPlainLUT();

	// add axes
	addSrAxes();
	// add a FoV outline
	addSrBox(3000,3000,4000,0,0,2000);
	
	// add a scalar bar
	addScalarBarDepth();
	addScalarBarAmp();
	addScalarBarSegm();

    renderer->AddActor(srBoxActor);
    renderer->AddActor(depthSca);
	renderer->AddActor(ampSca);
	renderer->AddActor(segmSca);
	//BGdataActor->Register(renderer);
    renderer->AddActor(axesActor);
	renderer->AddActor(fpsTxtActor);
	renderer->AddActor(fpsReadTxtActor);
    renderer->SetBackground(0.2,0.2,0.4);		//!< HARDCODED BACKGROUND COLOR
    renWin->SetSize(w,h);					//!< HARDCODED RENDER WINDOW SIZE
	renWin->SetPosition(x, y);

	fpsReadTxtActor->SetDisplayPosition(3,3);
	fpsTxtActor->SetDisplayPosition(w-125,3);
	//renderer->GetActiveCamera()->SetParallelProjection(1);
	renderer->GetActiveCamera()->SetParallelScale(2000);
	//renderer->GetActiveCamera()->SetPosition(0,0,-5000);
	//renderer->GetActiveCamera()->;
    // Finally we start the interactor so that event will be handled
    renWin->Render();
	//renderer->AddActor(dataActor);

};

/**
 * Destructor for CViewSrVtk
 *
 */
CViewSrVtk::~CViewSrVtk()
{
	// delete the render window
	renWin->Delete();
	// delete the renderer
	renderer->Delete();
	// delete the interactor
	iren->Delete();

	int i = 0;
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++, i++ )
	{
		delete((*it));
	}
	int res = 0;
	// free axes
	res+= freeSrAxes();
	// free FoV box
	res+= freeSrBox();
	// free depth LUT
	res+= freeDepthLUT();
	res+= freeGrayLUT();
	res+= freeSegmLUT();
	res+= freePlainLUT();
	// free scalar bar
	res+= freeScalarBarDepth();
	res+= freeScalarBarAmp();
	res+= freeScalarBarSegm();
};



/**
 * Adds the FoV actor
 *  -> Creates new objects that must be deleted
 *  --> freeSrBox() must be called
 */
int CViewSrVtk::addSrBox(int xLen, int yLen, int zLen, int xC, int yC, int zC)
{
	srBox = vtkCubeSource::New();
    srBox->SetXLength( (double) xLen );			//!< HARDCODED FoV OUTLINE BOX DIMENSIONS
    srBox->SetYLength( (double) yLen );
    srBox->SetZLength( (double) zLen );
    srBox->SetCenter( (double) xC, (double) yC, (double) zC);	//!< HARDCODED FoV OUTLINE BOX POSITION
    srBoxOutline = vtkOutlineFilter::New();
    srBoxOutline->SetInputConnection( srBox->GetOutputPort());
    srBoxMapper = vtkPolyDataMapper::New();
    srBoxMapper->SetInputConnection(srBoxOutline->GetOutputPort());
    srBoxActor = vtkActor::New();
    srBoxActor->SetMapper(srBoxMapper);			//!< HARDCODED SR CAMERA LABEL TEXT POSITION
	return srBoxActor->GetReferenceCount();
}

/**
 * Frees the FoV actor objects
 */
int CViewSrVtk::freeSrBox()
{
	int res = srBoxActor->GetReferenceCount();
	srBox->Delete();
	srBoxOutline->Delete();
	srBoxMapper->Delete();
	srBoxActor->Delete();
	return res-1;
}

/**
 * Adds the axes in the 3D plot
 *  -> Creates new objects that must be deleted
 *  --> freeSrAxes() must be called
 */
int CViewSrVtk::addSrAxes()
{
	axes = vtkAxes::New();
    axes->SetOrigin(0.0,0.0,0.0);
    axesMapper = vtkPolyDataMapper::New();
    axesMapper->SetInputConnection(axes->GetOutputPort());
    axesActor = vtkActor::New();
    axesActor->SetMapper(axesMapper);
	fpsTxtActor = vtkTextActor::New();
	fpsReadTxtActor = vtkTextActor::New();
	return axesActor->GetReferenceCount();
}

/**
 * Frees the axes in the 3D plot
 */
int CViewSrVtk::freeSrAxes()
{
	int res = axesActor->GetReferenceCount();
	axes->Delete();
	axesMapper->Delete();
	axesActor->Delete();
	fpsTxtActor->Delete();
	fpsReadTxtActor->Delete();
	return res-1;
}

/**
 * Creates the depth LUT
 *  -> Creates new objects that must be deleted
 *  --> freeDepthLUT() must be called
 */
int CViewSrVtk::addDepthLUT()
{
	depthLUT = vtkLookupTable::New();
	depthLUT->SetRampToLinear();
	return depthLUT->GetReferenceCount();
}

/**
 * Creates the gray LUT
 *  -> Creates a new object that must be deleted
 *  --> freeGrayLUT() must be called
 */
int CViewSrVtk::addGrayLUT()
{
	grayLUT = vtkLookupTable::New();
	grayLUT->SetRampToLinear();
	grayLUT->SetHueRange(0.0, 0.0);
	grayLUT->SetSaturationRange(0.0, 0.0);
	grayLUT->SetValueRange(0.0, 1.0);
	grayLUT->SetNumberOfTableValues(256);
	return grayLUT->GetReferenceCount();
}
/**
 * Creates the segm LUT
 *  -> Creates a new object that must be deleted
 *  --> freeSegmLUT() must be called
 */
int CViewSrVtk::addSegmLUT()
{
	segmLUT = vtkLookupTable::New();
	segmLUT->SetHueRange(0.7, 0.0);
	//segmLUT->SetSaturationRange(0.0, 1.0);
	//segmLUT->SetValueRange(0.0, 1.0);
	segmLUT->SetRampToLinear();
	segmLUT->SetNumberOfTableValues(256);
	return segmLUT->GetReferenceCount();
}
/**
 * Creates the plain LUTs
 *  -> Creates new objects that must be deleted
 *  --> freePlainLUT() must be called
 */
int CViewSrVtk::addPlainLUT()
{
	rLUT = vtkLookupTable::New();
	gLUT = vtkLookupTable::New();
	bLUT = vtkLookupTable::New();
	wLUT = vtkLookupTable::New();
	kLUT = vtkLookupTable::New();

	rLUT->SetRampToLinear();
	rLUT->SetHueRange(1.0, 1.0);
	rLUT->SetSaturationRange(1.0, 1.0);
	rLUT->SetValueRange(1.0, 1.0);
	rLUT->SetNumberOfTableValues(1);

	gLUT->SetRampToLinear();
	gLUT->SetHueRange(0.33333, 0.33333);
	gLUT->SetSaturationRange(1.0, 1.0);
	gLUT->SetValueRange(1.0, 1.0);
	gLUT->SetNumberOfTableValues(1);

	bLUT->SetRampToLinear();
	bLUT->SetHueRange(0.66666, 0.66666);
	bLUT->SetSaturationRange(1.0, 1.0);
	bLUT->SetValueRange(1.0, 1.0);
	bLUT->SetNumberOfTableValues(1);

	wLUT->SetRampToLinear();
	wLUT->SetHueRange(0.0, 0.0);
	wLUT->SetSaturationRange(0.0, 0.0);
	wLUT->SetValueRange(1.0, 1.0);
	wLUT->SetNumberOfTableValues(1);

	kLUT->SetRampToLinear();
	kLUT->SetHueRange(0.0, 0.0);
	kLUT->SetSaturationRange(1.0, 1.0);
	kLUT->SetValueRange(0.0, 0.0);
	kLUT->SetNumberOfTableValues(1);

	return rLUT->GetReferenceCount();
}

/**
 * Changes the depth range
 */
int CViewSrVtk::changeDepthRange(float minVal, float maxVal)
{
	int res = _vtkSubMax;
	int i = 0;
	//renWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
	return res-i;
}

/**
 * Changes the amplitude range
 */
int CViewSrVtk::changeAmpRange(float minAmp, float maxAmp)
{
	int res = _vtkSubMax;
	int i = 0;
	//renWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
	return res-i;
}

/**
 * Changes the segmentation range
 */
int CViewSrVtk::changeSegmRange(float minSegm, float maxSegm)
{
	int res = _vtkSubMax;
	int i = 0;
	//renWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
	return res-i;
}

/**
 * Frees the depth LUT
 */
int CViewSrVtk::freeDepthLUT()
{
	int res = depthLUT->GetReferenceCount();
	depthLUT->Delete();
	return res-1;
}

/**
 * Frees the gray LUT
 */
int CViewSrVtk::freeGrayLUT()
{
	int res = grayLUT->GetReferenceCount();
	grayLUT->Delete();
	return res-1;
}

/**
 * Frees the segm LUT
 */
int CViewSrVtk::freeSegmLUT()
{
	int res = segmLUT->GetReferenceCount();
	segmLUT->Delete();
	return res-1;
}

/**
 * Frees the plain LUTs
 */
int CViewSrVtk::freePlainLUT()
{
	int res = rLUT->GetReferenceCount();;
	rLUT->Delete();
	gLUT->Delete();
	bLUT->Delete();
	wLUT->Delete();
	kLUT->Delete();
	return res-1;
}

/**
 * Creates the scalar bar
 *  -> Creates new objects that must be deleted
 *  --> freeScalarBar() must be called
 */
int CViewSrVtk::addScalarBarDepth()
{
	depthSca = vtkScalarBarActor::New();
    depthSca->SetLookupTable(depthLUT);
    depthSca->SetTitle("Depth (mm)");
    depthSca->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    depthSca->GetPositionCoordinate()->SetValue(0.05,0.05);
    depthSca->SetOrientationToHorizontal();
    depthSca->SetWidth(0.9);
    depthSca->SetHeight(0.05);
	depthSca->SetLabelFormat("%4.0f");
	return depthSca->GetReferenceCount();
}

/**
 * Frees the scalar bar
 */
int CViewSrVtk::freeScalarBarDepth()
{
	int res = depthSca->GetReferenceCount();
	depthSca->Delete();
	return res-1;
}
/**
 * Creates the scalar bar
 *  -> Creates new objects that must be deleted
 *  --> freeScalarBar() must be called
 */
int CViewSrVtk::addScalarBarAmp()
{
	ampSca = vtkScalarBarActor::New();
    ampSca->SetLookupTable(grayLUT);
    ampSca->SetTitle("Amplitude (a.u.)");
    ampSca->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    ampSca->GetPositionCoordinate()->SetValue(0.05,0.90);
    ampSca->SetOrientationToHorizontal();
    ampSca->SetWidth(0.9);
    ampSca->SetHeight(0.05);
	ampSca->SetLabelFormat("%5.0f");
	return ampSca->GetReferenceCount();
}

/**
 * Frees the scalar bar
 */
int CViewSrVtk::freeScalarBarAmp()
{
	int res = ampSca->GetReferenceCount();
	ampSca->Delete();
	return res-1;
}

/**
 * Creates the scalar bar
 *  -> Creates new objects that must be deleted
 *  --> freeScalarBarSegm() must be called
 */
int CViewSrVtk::addScalarBarSegm()
{
	segmSca = vtkScalarBarActor::New();
    segmSca->SetLookupTable(segmLUT);
    segmSca->SetTitle("Segm. (a.u.)");
    segmSca->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    segmSca->GetPositionCoordinate()->SetValue(0.9,0.15);
    segmSca->SetOrientationToVertical();
	/*segmSca->SetTextPositionToSucceedScalarBar();*/
    segmSca->SetWidth(0.05);
    segmSca->SetHeight(0.7);
	segmSca->SetLabelFormat("%03.0f");
	return segmSca->GetReferenceCount();
}
/**
 * Frees the scalar bar
 */
int CViewSrVtk::freeScalarBarSegm()
{
	int res = segmSca->GetReferenceCount();
	segmSca->Delete();
	return res-1;
}

/**
 * Hides a data actor
 */
int CViewSrVtk::hideDataAct(int vtkSub, bool doHide)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++ )
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->hideDataAct(doHide);
		}
	}
	return vtkSub;
}

/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataActColorRGB(int vtkSub, double r, double g, double b)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataActColorRGB(r, g, b);
		}
	}
	return vtkSub;
}

/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorDepth(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataMapperColorDepth();
		}
	}
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorGray(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataMapperColorGray();
		}
	}
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorSegm(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataMapperColorSegm();
		}
	}
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorR(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataMapperColorR();
		}
	}
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorG(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataMapperColorG();
		}
	}
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorB(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataMapperColorB();
		}
	}
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorW(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataMapperColorW();
		}
	}
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorK(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	std::vector<CamVtkView*>::iterator it;  // get iterator on the cameras
	for ( it=cameras.begin() ; it != cameras.end(); it++)
	{
		if( (*it)->getVtkSub() == vtkSub )
		{
			(*it)->setDataMapperColorK();
		}
	}
	return vtkSub;
}
/**
 * Renders
 */
int CViewSrVtk::Render()
{
	int res = 0;
	renWin->Render(); //JMU20081110 rendering should be handeld by top-most window to avoid too many renderings
	return res;
}
/**
 * Renders
 */
double CViewSrVtk::timeRender()
{
	return renderer->GetLastRenderTimeInSeconds();
}

void CViewSrVtk::setParallelProj(bool para)
{
	if(para)
	{
		renderer->GetActiveCamera()->ParallelProjectionOn();
	}
	else
	{
		renderer->GetActiveCamera()->ParallelProjectionOff();
	}
	return;
}

int CViewSrVtk::setFpsTxt(char* txt)
{
	int res = 0;
	fpsTxtActor->SetInput(txt);
	return res;
}

int CViewSrVtk::setReadFpsTxt(char* txt)
{
	int res = 0;
	fpsReadTxtActor->SetInput(txt);
	return res;
}

void CViewSrVtk::hideSegmCbar(bool doHide)
{
	if(doHide)
	{
		segmSca->VisibilityOff();
	}
	else
	{
		segmSca->VisibilityOn();
	}
	return;
}

void CViewSrVtk::hideAmplCbar(bool doHide)
{
	if(doHide)
	{
		ampSca->VisibilityOff();
	}
	else
	{
		ampSca->VisibilityOn();
	}
	return;
}

void CViewSrVtk::hideDepthCbar(bool doHide)
{
	if(doHide)
	{
		depthSca->VisibilityOff();
	}
	else
	{
		depthSca->VisibilityOn();
	}
	return;
}

#ifdef JMU_ICPVTK
vtkStructuredGrid* CViewSrVtk::icpCam(vtkStructuredGrid* source, vtkStructuredGrid* target)
{
	int res = 0;

	// ripped off from : http://www.vtk.org/Wiki/Iterative_Closest_Points_(ICP)_Transform

	//setup ICP transform
	vtkIterativeClosestPointTransform* icp = vtkIterativeClosestPointTransform::New();
	icp->SetSource(source);
	icp->SetTarget(target);
	icp->GetLandmarkTransform()->SetModeToRigidBody();
	//icp->DebugOn();
	icp->SetMaximumNumberOfIterations(20);
	icp->StartByMatchingCentroidsOn();
	icp->Modified();
	icp->Update();

	//transform the source points by the ICP solution
	vtkTransformFilter* ICPTransFilter = vtkTransformFilter::New();
	ICPTransFilter->SetInput(source);
	ICPTransFilter->SetTransform(icp);
	ICPTransFilter->Update();

	return ICPTransFilter->GetStructuredGridOutput();

	//return res;
}
#endif

#ifdef JMU_KDTREEVTK
double CViewSrVtk::kdTreeEps(vtkStructuredGrid* source, vtkStructuredGrid* target)
{
	double eps = 0;
	vtkKdTree* kdtree = vtkKdTree::New();
	if(source==NULL){return -1;};
	if(target==NULL){return -1;};
	vtkPoints* srcPoints = source->GetPoints();
	vtkPoints* tgtPoints = target->GetPoints();
	kdtree->BuildLocatorFromPoints(srcPoints);
	vtkIdType numPoints = target->GetNumberOfPoints();
	double* ptXYZ = NULL;	// pointer for current point coordinates
	double dist2=0;			// container for squared distance
	double distSum = 0;		// sum  of UNSQUARED distances
	double* dists = NULL;	// list of UNSQUARED distances
	dists = (double*) malloc( numPoints *sizeof(double));
	if(dists == NULL){ return -1;};
	memset( dists, 0x0, numPoints *sizeof(double));

	// loop on all target points
	for(vtkIdType pt=0; pt < numPoints; pt++)
	{
		ptXYZ = tgtPoints->GetPoint(pt);
		kdtree->FindClosestPoint(ptXYZ, dist2);
		dist2 = sqrt(dist2);
		distSum += dist2;
		dists[pt] = dist2;
	}
	double avg = distSum / ( (double) numPoints);
	double diffSqSum = 0;
	for(vtkIdType pt=0; pt < numPoints; pt++)
	{
		diffSqSum += ( (dists[pt]-avg) * (dists[pt]-avg) );
	}
	double std = sqrt(diffSqSum / ( (double) numPoints));
	eps = std + avg;
	if(dists != NULL){ free(dists); dists=NULL;};
	return eps;
}
#endif

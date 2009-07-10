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
	#ifdef JMU_ICPVTK
		_icpTxtActor = vtkTextActor::New();	//!< actor for icp res. display
		renderer->AddActor(_icpTxtActor);
		_icpTxtActor->SetDisplayPosition(w-300, h-80);

		// new ICP actor
		_icpGrid = vtkStructuredGrid::New(); // to be replaced
		//_icpToPoly = vtkStructuredGridGeometryFilter::New();
		//_icpToPoly->SetInput(_icpGrid);
		_icpMapperZ = vtkDataSetMapper::New();
		_icpMapperZ->SetLookupTable(depthLUT);  // sets color
		_icpMapperZ->SetInput(_icpGrid);
		//_icpMapperZ->SetInputConnection(_icpToPoly->GetOutputPort());
		_icpMapperAmp = vtkDataSetMapper::New();
		_icpMapperAmp->SetLookupTable(grayLUT);  // sets color
		_icpMapperAmp->SetInput(_icpGrid);
		//_icpMapperAmp->SetInputConnection(_icpToPoly->GetOutputPort());
		_icpMapperSegm = vtkDataSetMapper::New();
		_icpMapperSegm->SetLookupTable(segmLUT);  // sets color
		_icpMapperSegm->SetInput(_icpGrid);
		//_icpMapperSegm->SetInputConnection(_icpToPoly->GetOutputPort());
		_icpActor = vtkActor::New();
		_icpActor->SetMapper(_icpMapperAmp);
		_icpMapperZ->Register(_icpActor);
		_icpMapperAmp->Register(_icpActor);
		_icpMapperSegm->Register(_icpActor);
		_icpActor->GetProperty()->SetRepresentationToPoints();
		_icpActor->GetProperty()->SetPointSize(3.0);			//! HARD CODED POINT SIZE
		_icpActor->GetProperty()->SetDiffuse(0.0);
		_icpActor->GetProperty()->SetSpecular(0.0);
		_icpActor->GetProperty()->SetAmbient(1.0);
		renderer->AddActor(_icpActor);
	#endif
	#ifdef JMU_KDTREEVTK
		_kdTxtActor = vtkTextActor::New() ;	//!< actor for kdDist display
		renderer->AddActor(_kdTxtActor);
		_kdTxtActor->SetDisplayPosition(3, h-20);
	#endif
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

	#ifdef JMU_ICPVTK
		_icpTxtActor->Delete();
		// new ICP actor
		_icpActor->Delete();
		_icpMapperZ->Delete();
		_icpMapperAmp->Delete();
		_icpMapperSegm->Delete();
		//_icpToPoly->Delete();
		//if(_icpGrid != NULL){_icpGrid->Delete();};
	#endif
	#ifdef JMU_KDTREEVTK
		_kdTxtActor->Delete();
	#endif

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
	#ifdef JMU_ICPVTK
		_icpMapperZ->SetScalarRange((double) minVal, (double) maxVal);
		_icpMapperZ->Modified();
	#endif
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
	#ifdef JMU_ICPVTK
		_icpMapperAmp->SetScalarRange((double) minAmp, (double) maxAmp);
		_icpMapperAmp->Modified();
	#endif
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
	#ifdef JMU_ICPVTK
		_icpMapperSegm->SetScalarRange((double) minSegm, (double) maxSegm);
		_icpMapperSegm->Modified();
	#endif
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
vtkStructuredGrid* CViewSrVtk::icpWork(vtkPointSet* source, vtkPointSet* target, int icpIter, int icpTrlCM, double mat[16])
{
	// ripped off from : http://www.vtk.org/Wiki/Iterative_Closest_Points_(ICP)_Transform

	//setup ICP transform
	vtkIterativeClosestPointTransform* icp = vtkIterativeClosestPointTransform::New();
	icp->SetSource(source);
	icp->SetTarget(target);
	icp->GetLandmarkTransform()->SetModeToRigidBody();
	//icp->DebugOn();
	icp->SetMaximumNumberOfIterations(icpIter);
	if(icpTrlCM==1)
	{
		icp->StartByMatchingCentroidsOn();
	}
	icp->Modified();
	icp->Update();

	//transform the source points by the ICP solution
	vtkTransformFilter* ICPTransFilter = vtkTransformFilter::New();
	ICPTransFilter->SetInput(source);
	ICPTransFilter->SetTransform(icp);
	ICPTransFilter->Update();

	vtkMatrix4x4* mat4 = icp->GetMatrix();
	int k=0;
	for(int col =0; col<4; col++ ) 
	{
		for(int row =0; row<4; row++ ) 
		{
			mat[k] = mat4->GetElement(row, col);
			k+=1;
		}
	}
	vtkStructuredGrid* output; output = NULL;
	output = ICPTransFilter->GetStructuredGridOutput();
	return output;
}
#endif
#ifdef JMU_ICPVTK
wxString CViewSrVtk::icpFct(std::vector<CamFrame*>* camFrms, int idxSrc, int srcField, int idxTgt, int tgtField, int icpIter, int icpTrlCM, double mat[16])
{
	wxString strRes; strRes.clear();
	if( camFrms == NULL) { return strRes;};
	std::vector<CamFrame*>::iterator it; 
	CamFrame* srcVtk = NULL; 
	CamFrame* tgtVtk = NULL;
	for ( it=camFrms->begin() ; it != camFrms->end(); it++)
	{
		int camSub = (*it)->GetVtkSub();
		if(camSub == idxSrc) { srcVtk = (*it); };
		if(camSub == idxTgt) { tgtVtk = (*it); };
	}
	if( srcVtk == NULL){ return strRes; };
	if( tgtVtk == NULL){ return strRes; };
	vtkPointSet* target = NULL;
	switch(tgtField){
		case 0:
			target = tgtVtk->GetCamVtk()->GetTransformedStructGrid();
			break;
		case 1:
			target = tgtVtk->GetCamBGVtk()->GetTransformedStructGrid();
			break;
		case 2:
			target = tgtVtk->GetCamFGVtk()->GetTransformedStructGrid();
			break;
		case 3:
			{
			vtkStructuredGrid* tgt = tgtVtk->GetCamVtk()->GetTransformedStructGrid();
			vtkSmartPointer<vtkPoints> TargetPoints = vtkSmartPointer<vtkPoints>::New();
			for (vtkIdType i=0; i < tgt->GetNumberOfPoints(); i++)
			{
				if( tgt->IsPointVisible(i) )
				{
					TargetPoints->InsertNextPoint( tgt->GetPoint(i));
				}
			}
			target = vtkUnstructuredGrid::New();
			target->SetPoints(TargetPoints);
			}
			break;
		default:
			break;
	}
	vtkPointSet* source = NULL;
	switch(srcField){
		case 0:
			source = srcVtk->GetCamVtk()->GetTransformedStructGrid();
			break;
		case 1:
			source = srcVtk->GetCamBGVtk()->GetTransformedStructGrid();
			break;
		case 2:
			source = srcVtk->GetCamFGVtk()->GetTransformedStructGrid();
			break;
		case 3:
			{
			vtkStructuredGrid* src = srcVtk->GetCamVtk()->GetTransformedStructGrid();
			vtkSmartPointer<vtkPoints> SourcePoints = vtkSmartPointer<vtkPoints>::New();
			for (vtkIdType i=0; i < src->GetNumberOfPoints(); i++)
			{
				if( src->IsPointVisible(i) )
				{
					SourcePoints->InsertNextPoint( src->GetPoint(i));
				}
			}
			source = vtkUnstructuredGrid::New();
			source->SetPoints(SourcePoints);
			}
			break;
		default:
			break;
	}

	vtkStructuredGrid* ptsSetICP = NULL;
	ptsSetICP = icpWork(source, target, icpIter, icpTrlCM, mat);
	if(ptsSetICP != NULL)
	{
		//if(_icpGrid != NULL){_icpGrid->Delete();};
		_icpGrid = ptsSetICP;
		_icpMapperZ->SetInput(_icpGrid); 
		_icpMapperAmp->SetInput(_icpGrid); 
		_icpMapperSegm->SetInput(_icpGrid);
		_icpMapperZ->Modified();
		_icpMapperAmp->Modified();
		_icpMapperSegm->Modified();
		_icpActor->SetMapper(_icpMapperAmp); // ? necessary -> seems so
		_icpActor->Modified();
		//_icpToPoly->SetInput(ptsSetICP);
		//_icpToPoly->Modified();
	}

	strRes.Printf(wxT("ICP: target: cam%i field%i (%i pts) \n source: cam%i field%i (%i pts) \n [ %g %g %g %g \n %g %g %g %g \n %g %g %g %g \n %g %g %g %g ]"),
		idxTgt, tgtField, (int)target->GetNumberOfPoints(), idxSrc, srcField, (int)source->GetNumberOfPoints(),
		mat[0], mat[4], mat[8], mat[12],mat[1], mat[5], mat[9], mat[13],mat[2], mat[6], mat[10], mat[14], mat[3], mat[7], mat[11], mat[15]);
	char icpText[512];
	if(strRes.length()<512)
	{
		strcpy(icpText, (const char*) strRes.char_str());
	}
	setIcpTxt(icpText);
	return strRes;
}
#endif // JMU_ICPVTK
#ifdef JMU_ICPVTK
void CViewSrVtk::hideICPact(bool doHide)
{
	if(doHide)
	{
		_icpActor->VisibilityOff();
	}
	else
	{
		_icpActor->VisibilityOn();
	}
	return;
}
#endif // JMU_ICPVTK
#ifdef JMU_ICPVTK
void CViewSrVtk::setICPColorDepth()
{
	//_icpMapperZ->GetInput()->GetPointData()->SetActiveScalars("Depth");// does not work
	//_icpMapperZ->SetLookupTable(depthLUT);
	_icpActor->SetMapper(_icpMapperZ);
}
void CViewSrVtk::setICPColorGray()
{
	//_icpMapperAmp->GetInput()->GetPointData()->SetActiveScalars("Amplitude"); // does not work
	//_icpMapperAmp->SetLookupTable(grayLUT);
	_icpActor->SetMapper(_icpMapperAmp);
}
void CViewSrVtk::setICPColorSegm()
{
	//_icpMapperSegm->GetInput()->GetPointData()->SetActiveScalars("Segmentation"); // does not work
	//_icpMapperSegm->SetLookupTable(segmLUT);
	_icpActor->SetMapper(_icpMapperSegm);
}
#endif // JMU_ICPVTK
#ifdef JMU_ICPVTK
void CViewSrVtk::setICPActOpacity(double alpha)
{
	if( (alpha >= 0.0) && (alpha <= 1.0))
	{
		_icpActor->GetProperty()->SetOpacity(alpha);
	}
}
void CViewSrVtk::setICPActRepPts()
{
	_icpActor->GetProperty()->SetRepresentationToPoints();
}
void CViewSrVtk::setICPActRepSurf()
{
	_icpActor->GetProperty()->SetRepresentationToSurface();
}
#endif // JMU_ICPVTK
#ifdef JMU_ICPVTK
void CViewSrVtk::setIcpTxt(char* txt)
{
	if(_icpTxtActor==NULL){return;};
	_icpTxtActor->SetInput(txt);
}
#endif // JMU_ICPVTK

#ifdef JMU_KDTREEVTK
double CViewSrVtk::kdTreeEps(vtkPointSet* source, vtkPointSet* target, double res[3], double thr, int inliers[2])
{
	double eps = 0;
	vtkKdTree* kdtree = vtkKdTree::New();
	if(source==NULL){return -1;};
	if(target==NULL){return -1;};
	vtkPoints* srcPoints = source->GetPoints();
	vtkPoints* tgtPoints = target->GetPoints();
	kdtree->BuildLocatorFromPoints(tgtPoints);
	vtkIdType numPoints = source->GetNumberOfPoints();
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
		ptXYZ = srcPoints->GetPoint(pt);
		kdtree->FindClosestPoint(ptXYZ, dist2);
		dist2 = sqrt(dist2);
		dists[pt] = dist2;
		if(thr==0.0)
		{
			distSum += dist2;
			dists[pt] = dist2;
		}
		else
		{
			if(dist2 <thr)
			{
				distSum += dist2;
				dists[pt] = dist2;
			}
			else // outlier if distance is above threshold
			{
				distSum+= 0;
				numPoints -=1;
			}
		}
	}
	double avg = distSum / ( (double) numPoints);
	double diffSqSum = 0;
	for(vtkIdType pt=0; pt < source->GetNumberOfPoints(); pt++)
	{
		if(thr==0.0)
		{
			diffSqSum += ( (dists[pt]-avg) * (dists[pt]-avg) );
		}
		else
		{
			if(dists[pt] <thr)
			{
				diffSqSum += ( (dists[pt]-avg) * (dists[pt]-avg) );
			}
		}
	}
	double std = sqrt(diffSqSum / ( (double) numPoints));
	eps = std + avg;
	res[0] = eps; res[1] = avg; res[2] = std;
	inliers[0] = (int) target->GetNumberOfPoints();
	inliers[1] = (int) numPoints;
	if(dists != NULL){ free(dists); dists=NULL;};
	return eps;
}
#endif
#ifdef JMU_KDTREEVTK
/** acting on "kdDist" button \n
 * - bug: for now only first and last cam are used \n
 * - todo: make dataset choice configurable
 */
double CViewSrVtk::kdDist(std::vector<CamFrame*>* camFrms, int idxSrc, int srcField, int idxTgt, int tgtField, double res[3], double thr, int inliers[2])
{
	if( camFrms == NULL) { return -1;};
	//if( (int) camFrm.size() < idxSrc+1) { return -1;};
	//if( (int) camFrm.size() < idxTgt+1) { return -1;};
	std::vector<CamFrame*>::iterator it; 
	CamFrame* srcVtk = NULL; 
	CamFrame* tgtVtk = NULL;
	for ( it=camFrms->begin() ; it != camFrms->end(); it++)
	{
		int camSub = (*it)->GetVtkSub();
		if(camSub == idxSrc) { srcVtk = (*it); };
		if(camSub == idxTgt) { tgtVtk = (*it); };
	}
	if( srcVtk == NULL){ return -1;};
	if( tgtVtk == NULL){ return -1;};
	vtkPointSet* target = NULL;
	switch(tgtField){
		case 0:
			target = tgtVtk->GetCamVtk()->GetTransformedStructGrid();
			break;
		case 1:
			target = tgtVtk->GetCamBGVtk()->GetTransformedStructGrid();
			break;
		case 2:
			target = tgtVtk->GetCamFGVtk()->GetTransformedStructGrid();
			break;
		case 3:
			{
			vtkStructuredGrid* tgt = tgtVtk->GetCamVtk()->GetTransformedStructGrid();
			vtkSmartPointer<vtkPoints> TargetPoints = vtkSmartPointer<vtkPoints>::New();
			for (vtkIdType i=0; i < tgt->GetNumberOfPoints(); i++)
			{
				if( tgt->IsPointVisible(i) )
				{
					TargetPoints->InsertNextPoint( tgt->GetPoint(i));
				}
			}
			target = vtkUnstructuredGrid::New();
			target->SetPoints(TargetPoints);
			}
			break;
		default:
			break;
	}
	vtkPointSet* source = NULL;
	switch(srcField){
		case 0:
			source = srcVtk->GetCamVtk()->GetTransformedStructGrid();
			break;
		case 1:
			source = srcVtk->GetCamBGVtk()->GetTransformedStructGrid();
			break;
		case 2:
			source = srcVtk->GetCamFGVtk()->GetTransformedStructGrid();
			break;
		case 3:
			{
			vtkStructuredGrid* src = srcVtk->GetCamVtk()->GetTransformedStructGrid();
			vtkSmartPointer<vtkPoints> SourcePoints = vtkSmartPointer<vtkPoints>::New();
			for (vtkIdType i=0; i < src->GetNumberOfPoints(); i++)
			{
				if( src->IsPointVisible(i) )
				{
					SourcePoints->InsertNextPoint( src->GetPoint(i));
				}
			}
			source = vtkUnstructuredGrid::New();
			source->SetPoints(SourcePoints);
			}
			break;
		default:
			break;
	}
	double eps = kdTreeEps(source, target, res, thr, inliers);
	char kdText[512];
	sprintf(kdText, "kdDist with thr%g: target: cam%i field%i (%i pts) / source: cam%i field%i (%i pts) -> avg=%g - std=%g - eps=%g",
		thr, idxTgt, tgtField, inliers[0], idxSrc, srcField, inliers[1],
		res[1], res[2], res[0]);
	setKdDistTxt(kdText);
	return eps;
}
#endif // JMU_KDTREEVTK
#ifdef JMU_KDTREEVTK
void CViewSrVtk::setKdDistTxt(char* txt)
{
	if(_kdTxtActor==NULL){return;};
	_kdTxtActor->SetInput(txt);
}
#endif // JMU_KDTREEVTK
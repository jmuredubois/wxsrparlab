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
	addPlainLUT();

	// add axes
	addSrAxes();
	// add a FoV outline
	addSrBox(3000,3000,4000,0,0,2000);
	
	// add a scalar bar
	addScalarBarDepth();
	addScalarBarAmp();

    renderer->AddActor(srBoxActor);
    renderer->AddActor(depthSca);
	renderer->AddActor(ampSca);
	//BGdataActor->Register(renderer);
    renderer->AddActor(axesActor);
    renderer->SetBackground(0.2,0.2,0.4);		//!< HARDCODED BACKGROUND COLOR
    renWin->SetSize(w,h);					//!< HARDCODED RENDER WINDOW SIZE
	renWin->SetPosition(x, y);

	//renderer->GetActiveCamera()->SetParallelProjection(1);
	//renderer->GetActiveCamera()->SetParallelScale(2000);
	//renderer->GetActiveCamera()->SetPosition(0,0,-5000);
	//renderer->GetActiveCamera()->SetViewPlaneNormal(0,0,-1);
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
	res+= freePlainLUT();
	// free scalar bar
	res+= freeScalarBarDepth();
	res+= freeScalarBarAmp();
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
	renWin->Render();
	return res-i;
}

/**
 * Changes the depth range
 */
int CViewSrVtk::changeAmpRange(float minAmp, float maxAmp)
{
	int res = _vtkSubMax;
	int i = 0;
	renWin->Render();
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
 * Updates the TOF points
 */
int CViewSrVtk::updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x, unsigned short* amp, int vtkSub)
{
	//if(!sr){return -1;};
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	renWin->Render();
	return vtkSub;
}

///**
// * Updates the TOF points
// */
//int CViewSrVtk::updateTOFcurrent(SRCAM sr, SRPARLAB srPL, int vtkSub, char* fname)
//{
//	int res = 0;
//	if(!fname){return -1;};
//	res += updateTOFcurrent(sr, srPL, vtkSub);
//	if(res!=0){return -2;};
//	dataWriter[vtkSub]->SetFileName(fname);
//	res+=dataWriter[vtkSub]->Write();
//
//	return res;
//}



/**
 * Updates the TOF background points
 */
//int CViewSrVtk::updateTOFbg(SRCAM sr, SRPARLAB srPL, int vtkSub)
//{
//	int res = 0;
//	if(!sr){return -1;};
//	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
//
//	if((!_xBG[vtkSub]) || (!_yBG[vtkSub]) || (!_zBG[vtkSub]) ){
//		allocXYZbg(sr, vtkSub); };
//	if(!(srPL)) return -1;
//
//	int bgCnt =PL_GetBGcnt(srPL);
//	if(bgCnt==0) {return res;};
//
//	//freeBGDataAct();
//
//	int rows=(int)SR_GetRows(sr);
//	int cols=(int)SR_GetCols(sr);
//	int num=(int)SR_GetRows(sr)*(int)SR_GetCols(sr);
//
//	WORD* pha = (WORD*)SR_GetImage(sr,0);
//	WORD* amp = (WORD*)SR_GetImage(sr,1);
//
//	WORD* phaBg = PL_GetBGImage(srPL, 0);
//	WORD* ampBg = PL_GetBGImage(srPL, 1);
//	WORD* zBG = PL_GetZbgImage(srPL);
//
//	SR_SetBuffer(sr, (LPVOID) phaBg, SR_GetBufferSize(sr)); //touchy
//	SR_CoordTrfUint16(sr, _xBG[vtkSub],_yBG[vtkSub],0, sizeof(short),sizeof(short), sizeof(WORD));
//	///SR_CoordTrfUint16(sr, xBG,yBG,zBG2, sizeof(short),sizeof(short), sizeof(WORD));
//	SR_SetBuffer(sr, (LPVOID) pha, SR_GetBufferSize(sr)); //touchy
//
//	// WATCH OUT, THE BG DATA MUST BE IN SR BUFFER WHEN THIS FCT IS CALLED !!!
//	//SR_CoordTrfUint16(sr, _xBG,_yBG,_zBG, sizeof(short),sizeof(short), sizeof(WORD));
//
//
//	// We ask pcoords to allocate room for at least 25344 tuples
//	// and set the number of tuples to 4.
//	if(num!=25344)
//	{
//	BGpcoords[vtkSub]->SetNumberOfTuples(num);
//	BGdData[vtkSub]->SetNumberOfTuples(num);
//	}
//
//	// Assign each tuple
//	float pt[3];
//	int row = 0; int col = 0;
//	int i = 0; int iv1 = 0; int iv2 = 0; int iv3 = 0;
//	for (row = 0 ; row <rows; row++)
//    {
//		for (col = 0; col<cols; col++)
//		{
//			pt[2] = (float)zBG[i];
//			pt[1] = (float)((_yBG[vtkSub])[i]);
//			pt[0] = (float)((_xBG[vtkSub])[i]);
//			BGpcoords[vtkSub]->SetTuple((iv1+iv2), pt);
//			BGdData[vtkSub]->SetValue((iv1+iv2),(float)zBG[i]);
//			i++; // le i++ doit être ici, il faut commencer à zéro !!!
//			iv2+=rows;
//			iv3++;
//			if(iv3>=cols)
//			{
//				iv2 = 0;
//				iv3 = 0;
//				iv1 += 1;
//			}
//		}
//    }
//	BGdataActor[vtkSub]->VisibilityOn();
//	BGdataPoints[vtkSub]->Modified();
//
//	i = 0;iv1 = 0; iv2 = 0; iv3 = 0;
//	for (row = 0 ; row <rows; row++)
//    {
//		for (col = 0; col<cols; col++)
//		{
//			vtkPolyData* totoPoly = BGpdata[vtkSub]->GetOutput();
//			vtkPoints* totoPoints = totoPoly->GetPoints();
//			if(!totoPoints) {
//				return -3; // WHY WHY WHY IS A NULLPOINTER RETURNED ?
//			};
//			double* totoDouble = totoPoints->GetPoint(iv1+iv2);
//			BGdData[vtkSub]->SetValue((iv1+iv2),(float)(BGpdata[vtkSub]->GetOutput()->GetPoints()->GetPoint(iv1+iv2)[2]));		// make sure that depth data is the transformed value; :-( unable to avoid loop yet :-(
//			i++; // le i++ doit être ici, il faut commencer à zéro !!!
//			iv2+=rows;
//			iv3++;
//			if(iv3>=cols)
//			{
//				iv2 = 0;
//				iv3 = 0;
//				iv1 += 1;
//			}
//		}
//    }
//
//	BGdataActor[vtkSub]->VisibilityOn();
//	renWin->Render();
//	return res;
//}


/**
 * Updates the TOF background points
 */
//int CViewSrVtk::updateTOFbg(SRCAM sr, SRPARLAB srPL, int vtkSub, char* fname)
//{
//	int res = 0;
//	if(!fname){return -1;};
//	res += updateTOFbg(sr, srPL, vtkSub);
//	if(res!=0){return -2;};
//	BGdataWriter[vtkSub]->SetFileName(fname);
//	res+=BGdataWriter[vtkSub]->Write();
//
//	return res;
//}

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
	renWin->Render();
	return res;
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
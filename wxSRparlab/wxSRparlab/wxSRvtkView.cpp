#include "wxSRparlab.h"
#include "wxSRvtkView.h"
#include "camVtkView.h"


/**
 * Constructor for CViewSrVtk
 *
 */
CViewSrVtk::CViewSrVtk(wxFrame* pWnd)
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

	// add a shape representing the camera
	for(int i=0; i<_vtkSubMax; i++)
	{
		CamVtkView* cam = new CamVtkView(i, renWin, renderer, depthLUT);
		cam->SetDepthLUT(depthLUT);
		cam->SetGrayLUT(grayLUT);
		cam->SetPlainLUT(rLUT, gLUT, bLUT, wLUT, kLUT);
		cameras.push_back(cam);
	};
	// add axes
	addSrAxes();
	// add a FoV outline
	addSrBox(3000,3000,4000,0,0,2000);
	
	// add a scalar bar
	addScalarBar();

    renderer->AddActor(srBoxActor);
    renderer->AddActor(depthSca);
	//BGdataActor->Register(renderer);
    renderer->AddActor(axesActor);
    renderer->SetBackground(0.2,0.2,0.4);		//!< HARDCODED BACKGROUND COLOR
    renWin->SetSize(800,600);					//!< HARDCODED RENDER WINDOW SIZE

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
	// free axes
	freeSrAxes();
	// free FoV box
	freeSrBox();
	// free depth LUT
	freeDepthLUT();
	freeGrayLUT();
	freePlainLUT();
	// free scalar bar
	freeScalarBar();
};



/**
 * Adds the FoV actor
 *  -> Creates new objects that must be deleted
 *  --> freeSrBox() must be called
 */
int CViewSrVtk::addSrBox(int xLen, int yLen, int zLen, int xC, int yC, int zC)
{
	int res = 0;

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

	return res;
}

/**
 * Frees the FoV actor objects
 */
int CViewSrVtk::freeSrBox()
{
	int res = 0;

	srBox->Delete();
	srBoxOutline->Delete();
	srBoxMapper->Delete();
	srBoxActor->Delete();

	return res;
}

/**
 * Adds the axes in the 3D plot
 *  -> Creates new objects that must be deleted
 *  --> freeSrAxes() must be called
 */
int CViewSrVtk::addSrAxes()
{
	int res = 0;

	axes = vtkAxes::New();
    axes->SetOrigin(0.0,0.0,0.0);
    axesMapper = vtkPolyDataMapper::New();
    axesMapper->SetInputConnection(axes->GetOutputPort());
    axesActor = vtkActor::New();
    axesActor->SetMapper(axesMapper);

	return res;
}

/**
 * Frees the axes in the 3D plot
 */
int CViewSrVtk::freeSrAxes()
{
	int res = 0;

	axes->Delete();
	axesMapper->Delete();
	axesActor->Delete();

	return res;
}

/**
 * Creates the depth LUT
 *  -> Creates new objects that must be deleted
 *  --> freeDepthLUT() must be called
 */
int CViewSrVtk::addDepthLUT()
{
	int res = 0;
	depthLUT = vtkLookupTable::New();
	depthLUT->SetRampToLinear();

	//for(int i=0; i<_vtkSubMax; i++)
	//{
	//	dataMapper[i]->SetScalarRange(0.0,4500.0);		//!< HARDCODED SCALAR RANGE FOR DEPTH LUT
	//	dataMapper[i]->SetLookupTable(depthLUT);
	//}

	return res;
}

/**
 * Creates the gray LUT
 *  -> Creates a new object that must be deleted
 *  --> freeGrayLUT() must be called
 */
int CViewSrVtk::addGrayLUT()
{
	int res = 0;
	grayLUT = vtkLookupTable::New();
	grayLUT->SetRampToLinear();
	grayLUT->SetHueRange(0.0, 0.0);
	grayLUT->SetSaturationRange(0.0, 0.0);
	grayLUT->SetValueRange(0.0, 1.0);
	grayLUT->SetNumberOfTableValues(256);
	return res;
}
/**
 * Creates the plain LUTs
 *  -> Creates new objects that must be deleted
 *  --> freePlainLUT() must be called
 */
int CViewSrVtk::addPlainLUT()
{
	int res = 0;
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
	return res;
}

/**
 * Changes the depth range
 */
int CViewSrVtk::changeDepthRange(float minVal, float maxVal)
{
	int res = 0;

	for(int i=0; i< _vtkSubMax; i++)
	{
		//TODO = iterate on cameras vector
		//dataMapper[i]->SetScalarRange((double) minVal, (double) maxVal);
		//dataMapper[i]->Modified();
		//BGdataMapper[i]->SetScalarRange((double) minVal, (double) maxVal);
		//BGdataMapper[i]->Modified();
	}
	renWin->Render();
	return res;
}

/**
 * Changes the depth range
 */
int CViewSrVtk::changeAmpRange(float minAmp, float maxAmp)
{
	int res = 0;

	for(int i=0; i< _vtkSubMax; i++)
	{
		//TODO = iterate on cameras vector
		//dataMapper[i]->SetScalarRange((double) minAmp, (double) maxAmp);
		//dataMapper[i]->Modified();
	}
	renWin->Render();
	return res;
}


/**
 * Frees the depth LUT
 */
int CViewSrVtk::freeDepthLUT()
{
	int res = 0;
	depthLUT->Delete();
	return res;
}

/**
 * Frees the gray LUT
 */
int CViewSrVtk::freeGrayLUT()
{
	int res = 0;
	grayLUT->Delete();
	return res;
}

/**
 * Frees the plain LUTs
 */
int CViewSrVtk::freePlainLUT()
{
	int res = 0;
	rLUT->Delete();
	gLUT->Delete();
	bLUT->Delete();
	wLUT->Delete();
	kLUT->Delete();
	return res;
}

/**
 * Creates the scalar bar
 *  -> Creates new objects that must be deleted
 *  --> freeScalarBar() must be called
 */
int CViewSrVtk::addScalarBar()
{
	int res = 0;

	depthSca = vtkScalarBarActor::New();
    depthSca->SetLookupTable(depthLUT);
    depthSca->SetTitle("Depth (mm)");
    depthSca->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    depthSca->GetPositionCoordinate()->SetValue(0.05,0.05);
    depthSca->SetOrientationToHorizontal();
    depthSca->SetWidth(0.9);
    depthSca->SetHeight(0.05);
	depthSca->SetLabelFormat("%4.0f");

	return res;
}

/**
 * Frees the scalar bar
 */
int CViewSrVtk::freeScalarBar()
{
	int res = 0;
	depthSca->Delete();
	return res;
}


/**
 * Updates the TOF points
 */
int CViewSrVtk::updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x, unsigned short* amp, int vtkSub)
{
	//if(!sr){return -1;};
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	// TODO -> call good camera update

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
//			i++; // le i++ doit �tre ici, il faut commencer � z�ro !!!
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
//			i++; // le i++ doit �tre ici, il faut commencer � z�ro !!!
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
	// TODO: iterate on cameras vector
	//if(doHide){ dataActor[vtkSub]->VisibilityOff();}
	//else{ dataActor[vtkSub]->VisibilityOn();}
	return vtkSub;
}

/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataActColorRGB(int vtkSub, double r, double g, double b)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	// TODO: iterate on cameras vector
	//dataActor[vtkSub]->GetProperty()->SetColor(r,g,b);  // sets color
	return vtkSub;
}

/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorDepth(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	// TODO: iterate on cameras vector
	//data[vtkSub]->GetPointData()->SetScalars(dData[vtkSub]);
	//dataMapper[vtkSub]->SetLookupTable(depthLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorGray(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	// TODO: iterate on cameras vector
	//data[vtkSub]->GetPointData()->SetScalars(aData[vtkSub]);
	//dataMapper[vtkSub]->SetLookupTable(grayLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorR(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	// TODO: iterate on cameras vector
	//dataMapper[vtkSub]->SetLookupTable(rLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorG(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	// TODO: iterate on cameras vector
	//dataMapper[vtkSub]->SetLookupTable(gLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorB(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	// TODO: iterate on cameras vector
	//dataMapper[vtkSub]->SetLookupTable(bLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorW(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	// TODO: iterate on cameras vector
	//dataMapper[vtkSub]->SetLookupTable(wLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorK(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	// TODO: iterate on cameras vector
	//dataMapper[vtkSub]->SetLookupTable(kLUT);  // sets color
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

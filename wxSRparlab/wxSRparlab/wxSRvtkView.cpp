#include "wxSRparlab.h"
#include "wxSRvtkView.h"


/**
 * Constructor for CViewSrVtk
 *
 */
CViewSrVtk::CViewSrVtk(wxFrame* pWnd)
{
	_x = (short**) malloc( _vtkSubMax * sizeof(short*));
	_y = (short**) malloc( _vtkSubMax* sizeof(short*));
	_z = (unsigned short**) malloc( _vtkSubMax* sizeof(unsigned short*));

	_xBG = (short**) malloc( _vtkSubMax* sizeof(short*));
	_yBG = (short**) malloc( _vtkSubMax* sizeof(short*));
	_zBG = (unsigned short**) malloc( _vtkSubMax* sizeof(unsigned short*));

	srCube = (vtkCubeSource**) malloc( _vtkSubMax * sizeof(vtkCubeSource*));
	srCubeMapper = (vtkPolyDataMapper**) malloc( _vtkSubMax * sizeof(vtkPolyDataMapper*));
	srCubeActor = (vtkActor**) malloc( _vtkSubMax * sizeof(vtkActor*));
	srLabel = (vtkVectorText**) malloc( _vtkSubMax * sizeof(vtkVectorText*));
	srLabelMapper = (vtkPolyDataMapper**) malloc( _vtkSubMax * sizeof(vtkPolyDataMapper*));
	srLabelActor = (vtkFollower**) malloc( _vtkSubMax * sizeof(vtkFollower*));


	data = (vtkStructuredGrid**) malloc( _vtkSubMax * sizeof(vtkStructuredGrid*));
	dataMapper = (vtkPolyDataMapper**) malloc( _vtkSubMax * sizeof(vtkPolyDataMapper*));
	dataActor = (vtkActor**) malloc( _vtkSubMax * sizeof(vtkActor*));

	//spoints = (vtkStructuredPoints**) malloc( _vtkSubMax * sizeof(vtkStructuredPoints*));
	dataPoints = (vtkPoints**) malloc( _vtkSubMax * sizeof(vtkPoints*));
	pdata = (vtkStructuredGridGeometryFilter**) malloc( _vtkSubMax * sizeof(vtkStructuredGridGeometryFilter*));
	pcoords = (vtkFloatArray**) malloc( _vtkSubMax * sizeof(vtkFloatArray*));
	dData = (vtkFloatArray**) malloc( _vtkSubMax * sizeof(vtkFloatArray*));
	aData = (vtkFloatArray**) malloc( _vtkSubMax * sizeof(vtkFloatArray*));


	BGdata = (vtkStructuredGrid**) malloc( _vtkSubMax * sizeof(vtkStructuredGrid*));
	BGdataMapper = (vtkPolyDataMapper**) malloc( _vtkSubMax * sizeof(vtkPolyDataMapper*));
	BGdataActor = (vtkActor**) malloc( _vtkSubMax * sizeof(vtkActor*));


	BGdataPoints = (vtkPoints**) malloc( _vtkSubMax * sizeof(vtkPoints*));
	BGpdata = (vtkStructuredGridGeometryFilter**) malloc( _vtkSubMax * sizeof(vtkStructuredGridGeometryFilter*));
	BGpcoords = (vtkFloatArray**) malloc( _vtkSubMax * sizeof(vtkFloatArray*));
	BGdData = (vtkFloatArray**) malloc( _vtkSubMax * sizeof(vtkFloatArray*));


	camTran = (vtkTransform**) malloc( _vtkSubMax * sizeof(vtkTransform*));
	camTranMat = (vtkMatrix4x4**) malloc( _vtkSubMax * sizeof(vtkMatrix4x4*));
	camTranFilter = (vtkTransformFilter**) malloc( _vtkSubMax * sizeof(vtkTransformFilter*));
	BGcamTranFilter = (vtkTransformFilter**) malloc( _vtkSubMax * sizeof(vtkTransformFilter*));

	dataWriter = (vtkStructuredGridWriter**) malloc(_vtkSubMax * sizeof(vtkStructuredGridWriter*));
	BGdataWriter = (vtkStructuredGridWriter**) malloc(_vtkSubMax * sizeof(vtkStructuredGridWriter*));


	for(int i=0; i<_vtkSubMax; i++)
	{
		_x[i] = NULL;
		_y[i] = NULL;
		_z[i] = NULL;

		_xBG[i] = NULL;
		_yBG[i] = NULL;
		_zBG[i] = NULL;
		camTranMat[i] = vtkMatrix4x4::New();

		dataWriter[i] = vtkStructuredGridWriter::New();
		BGdataWriter[i] = vtkStructuredGridWriter::New();
	}

#ifndef VTKNOTRANSFORM
#include "camTranMats.cpp"
#endif// VTKNOTRANSFORM


	for(int i=0; i<_vtkSubMax; i++)
	{
		camTran[i] = vtkTransform::New();
		camTran[i]->SetMatrix(camTranMat[i]);
		camTranFilter[i] = vtkTransformFilter::New();
		camTranFilter[i]->SetTransform(camTran[i]);
		BGcamTranFilter[i] = vtkTransformFilter::New();
		BGcamTranFilter[i]->SetTransform(camTran[i]);
	}

	// instantiate class fields
	// setup the parent window
	renWin = vtkRenderWindow::New();
    renWin->SetParentId((void*)pWnd);
	// add a renderer
	renderer = vtkRenderer::New();
	renWin->AddRenderer(renderer);
	cam0 = renderer->GetActiveCamera();
	//light0 = vtkLight::New();
	//light0->
	//renderer->LightFollowCameraOn();
	//renderer->AutomaticLightCreationOff();
	// add an interactor
    iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);


	  //we start the interactor so that event will be handled
    renWin->Render();

	// add a shape representing the camera
	for(int i=0; i<_vtkSubMax; i++)
	{
		addSrCam(i);
		srLabelActor[i]->SetCamera(cam0);
	};
	// add axes
	addSrAxes();
	// add a FoV outline
	addSrBox(3000,3000,4000,0,0,2000);
	// add a data actor
	for(int i=0; i<_vtkSubMax; i++)
	{
		addDataAct(i);
		// add a BG data actor
		addBGDataAct(i);
	};
	// create a depth LUT
	addDepthLUT();
	addGrayLUT();
	addPlainLUT();
	// add a scalar bar
	addScalarBar();


	// add actors
	for(int i=0; i<_vtkSubMax; i++)
	{
		renderer->AddActor(srCubeActor[i]);
		renderer->AddActor(dataActor[i]);
		renderer->AddActor(BGdataActor[i]);
		renderer->AddActor(srLabelActor[i]);
	};
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

	for(int i=0; i<_vtkSubMax; i++)
	{
		camTran[i]->Delete();
		camTranMat[i]->Delete();
		camTranFilter[i]->Delete();
		BGcamTranFilter[i]->Delete();
	};
	free(camTran);
	free(camTranFilter);
	free(BGcamTranFilter);

	// free camera shape
	for(int i=0; i<_vtkSubMax; i++)
	{
		freeSrCam(i);
	};
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

	for(int i=0; i<_vtkSubMax; i++)
	{
		BGdataWriter[i]->Delete();
		dataWriter[i]->Delete();
		// free data actor
		freeBGDataAct(i);
		// free data actor
		freeDataAct(i);
	};

	free(BGdataWriter);
	free(dataWriter);
	free(srCube);
	free(srCubeMapper);
	free(srCubeActor);
	free(srLabel);
	free(srLabelMapper);
	free(srLabelActor);

	free(data);
	free(dataMapper);
	free(dataActor);

	//free(spoints);
	free(dataPoints);
	free(pdata);
	free(pcoords);
	free(dData);
	free(aData);


	free(BGdata);
	free(BGdataMapper);
	free(BGdataActor);


	free(BGdataPoints);
	free(BGpdata);
	free(BGpcoords);
	free(BGdData);
};


/**
 * Adds the camera actor
 *  -> Creates new objects that must be deleted
 *  --> freeSrCam() must be called
 */
int CViewSrVtk::addSrCam(int vtkSub)
{
	int res = 0;
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	srCube[vtkSub] = vtkCubeSource::New();
    srCube[vtkSub]->SetXLength( 55.0 );			//!< HARDCODED SR CAMERA DIMENSIONS
    srCube[vtkSub]->SetYLength( 75.0 );
    srCube[vtkSub]->SetZLength( 55.0 );
	srCube[vtkSub]->SetCenter( camTranMat[vtkSub]->GetElement(0,3),  camTranMat[vtkSub]->GetElement(1,3),  camTranMat[vtkSub]->GetElement(2,3));
    srCubeMapper[vtkSub] = vtkPolyDataMapper::New();
    srCubeMapper[vtkSub]->SetInputConnection(srCube[vtkSub]->GetOutputPort());
    srCubeActor[vtkSub] = vtkActor::New();
    srCubeActor[vtkSub]->SetMapper(srCubeMapper[vtkSub]);
    srCubeActor[vtkSub]->GetProperty()->SetColor(0.0,0.0,1.0);	//!< HARDCODED SR CAMERA COLOR

    srLabel[vtkSub] = vtkVectorText::New();
    srLabel[vtkSub]->SetText("TOF camera");						//!< HARDCODED SR CAMERA LABEL TEXT
    srLabelMapper[vtkSub] = vtkPolyDataMapper::New();
    srLabelMapper[vtkSub]->SetInputConnection(srLabel[vtkSub]->GetOutputPort());
    srLabelActor[vtkSub] = vtkFollower::New();
    srLabelActor[vtkSub]->SetMapper(srLabelMapper[vtkSub]);
    srLabelActor[vtkSub]->SetScale(50.0,50.0,50.0);				//!< HARDCODED SR CAMERA LABEL TEXT SIZE
	srLabelActor[vtkSub]->SetPosition( camTranMat[vtkSub]->GetElement(0,3),  camTranMat[vtkSub]->GetElement(1,3),  camTranMat[vtkSub]->GetElement(2,3));
    srLabelActor[vtkSub]->AddPosition(-50.0,-50.0,-50.0);				//!< HARDCODED SR CAMERA LABEL TEXT POSITION
	//srLabelActor[vtkSub]->SetCamera(cam0);
	if(vtkSub==0){ srLabelActor[vtkSub]->GetProperty()->SetColor(0.0,0.0,1.0); }; // BLUE for 0-th cam
	if(vtkSub==1){ srLabelActor[vtkSub]->GetProperty()->SetColor(1.0,0.0,0.0); }; // RED  for 1-st add cam
	if(vtkSub==2){ srLabelActor[vtkSub]->GetProperty()->SetColor(0.0,1.0,0.0); }; // GREENfor 2-nd add cam
	if(vtkSub==3){ srLabelActor[vtkSub]->GetProperty()->SetColor(0.7,0.0,0.7); }; // PURPLfor 3-rd add cam

	return res;
}

/**
 * Frees the camera actor objects
 */
int CViewSrVtk::freeSrCam(int vtkSub)
{
	int res = 0;

	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	srCube[vtkSub]->Delete();
	srCubeMapper[vtkSub]->Delete();
	srCubeActor[vtkSub]->Delete();
	srLabel[vtkSub]->Delete();
	srLabelMapper[vtkSub]->Delete();
	srLabelActor[vtkSub]->Delete();

	return res;
}

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

	for(int i=0; i<_vtkSubMax; i++)
	{
		dataMapper[i]->SetScalarRange(0.0,4500.0);		//!< HARDCODED SCALAR RANGE FOR DEPTH LUT
		dataMapper[i]->SetLookupTable(depthLUT);
	}

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
		dataMapper[i]->SetScalarRange((double) minVal, (double) maxVal);
		dataMapper[i]->Modified();
		BGdataMapper[i]->SetScalarRange((double) minVal, (double) maxVal);
		BGdataMapper[i]->Modified();
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
		dataMapper[i]->SetScalarRange((double) minAmp, (double) maxAmp);
		dataMapper[i]->Modified();
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
 * Creates a data actor
 *  ->
 *  --> freeDataAct must be called in cleanup
 */
int CViewSrVtk::addDataAct(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	dataPoints[vtkSub] = vtkPoints::New();
	// Create a float array which represents the points.
    pcoords[vtkSub] = vtkFloatArray::New();
	dData[vtkSub] = vtkFloatArray::New();	// scalar depth associated to each point (for coloring)
	aData[vtkSub] = vtkFloatArray::New();	// scalar depth associated to each point (for coloring)
	// Note that by default, an array has 1 component.
    // We have to change it to 3 for points
    pcoords[vtkSub]->SetNumberOfComponents(3);
	dData[vtkSub]->SetNumberOfComponents(1);
	aData[vtkSub]->SetNumberOfComponents(1);
	// We ask pcoords to allocate room for at least 25344 tuples
	// and set the number of tuples to 4.
	pcoords[vtkSub]->SetNumberOfTuples(25344);
	dData[vtkSub]->SetNumberOfTuples(25344);
	aData[vtkSub]->SetNumberOfTuples(25344);

	// Assign each tuple
	int row,col;
	int rows = 144; int cols = 176; // HARDCODED BAD BAD BAD or not in first add?
	float x,y,z;
	float xMin, xMax, yMin, yMax;
	xMin = -22.0f; xMax = 22.0f;	// some bogus init value
	yMin = -32.0f; yMax = 32.0f;	// some bogus init value
	z = 25.0f;							// some bogus init value
	float pt[3];
	int i = 0;int iv1 = 0; int iv2 = 0; int iv3 = 0; //loop counters
	for (row = 0 ; row <rows; row++)	// loop on all rows
    {
		y =  (  ((float)row )/144.0f * (yMax-yMin) + yMin);	//bogus value Y init
		for (col = 0; col<cols; col++)	// loop on all columns
		{
			x =  (  ((float)col )/176.0f * (xMax-xMin) + xMin);	// bogus value X init
			pt[2] = z;
			pt[1] = y;
			pt[0] = x;
			pcoords[vtkSub]->SetTuple((iv1+iv2), pt);
			dData[vtkSub]->SetValue((iv1+iv2),z);
			aData[vtkSub]->SetValue((iv1+iv2),100.0f);
			i++; // le i++ doit être ici, il faut commencer à zéro !!!
			iv2+=rows;
			iv3++;
			if(iv3>=cols)
			{
				iv2 = 0;
				iv3 = 0;
				iv1 += 1;
			}
		}
    }

	dataPoints[vtkSub]->SetData(pcoords[vtkSub]);


	data[vtkSub] = vtkStructuredGrid::New();
	vtkPointData *ptdata = data[vtkSub]->GetPointData();
	data[vtkSub]->SetPoints(dataPoints[vtkSub]);
	data[vtkSub]->SetWholeExtent(0,143,0,175,0,0);
	ptdata->SetScalars(dData[vtkSub]);
	data[vtkSub]->ComputeBounds();
	int dim[3];
	data[vtkSub]->GetDimensions(dim);
	data[vtkSub]->SetDimensions(144,176,1);

	camTranFilter[vtkSub]->SetInput(data[vtkSub]);	// 20080118 transf

	pdata[vtkSub] = vtkStructuredGridGeometryFilter::New();
	// 20080118 transf	pdata[vtkSub]->SetInput(data[vtkSub]);
	pdata[vtkSub]->SetInput(camTranFilter[vtkSub]->GetOutputDataObject(0)); // 20080118 transf
    dataMapper[vtkSub] = vtkPolyDataMapper::New();

	dataMapper[vtkSub]->SetInputConnection(pdata[vtkSub]->GetOutputPort());

    dataActor[vtkSub] = vtkActor::New();
    dataActor[vtkSub]->SetMapper(dataMapper[vtkSub]);
    dataActor[vtkSub]->GetProperty()->SetRepresentationToPoints();
	dataActor[vtkSub]->GetProperty()->SetPointSize(3.0);			//! HARD CODED POINT SIZE
	dataActor[vtkSub]->GetProperty()->SetDiffuse(0.0);
	dataActor[vtkSub]->GetProperty()->SetSpecular(0.0);
	dataActor[vtkSub]->GetProperty()->SetAmbient(1.0);

	//dataWriter[vtkSub]->SetInput(data[vtkSub]); // write StructuredGrid data
	dataWriter[vtkSub]->SetInput(camTranFilter[vtkSub]->GetOutputDataObject(0)); // 20080118 transf

#ifdef _DEBUG
	dataWriter[vtkSub]->SetFileTypeToASCII();
#else
	dataWriter[vtkSub]->SetFileTypeToBinary();
#endif

	return vtkSub;
}

/**
 * Updates the TOF points
 */
int CViewSrVtk::updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x, int vtkSub)
{
	//if(!sr){return -1;};
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	if((!_x[vtkSub]) || (!_y[vtkSub]) || (!_z[vtkSub]) ){ allocXYZ(rows, cols, vtkSub); };

	int num=rows*cols;
	//SR_CoordTrfUint16(sr, _x[vtkSub],_y[vtkSub],_z[vtkSub], sizeof(short),sizeof(short), sizeof(WORD));
	memcpy((void*)_x[vtkSub], (void*) x, num*sizeof(short) );
	memcpy((void*)_y[vtkSub], (void*) y, num*sizeof(short) );
	memcpy((void*)_z[vtkSub], (void*) z, num*sizeof(unsigned short) );


	//pcoords->Reset();
	//pcoords->SetNumberOfComponents(3);

	// We ask pcoords to allocate room for at least 25344 tuples
	// and set the number of tuples to 4.
	if(num != 25344)
	{
		pcoords[vtkSub]->SetNumberOfTuples(num);
	}

	float pt[3];
	int row = 0; int col = 0;
	int i = 0; int iv1 = 0; int iv2 = 0; int iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		for (col = 0; col<cols; col++)
		{
			pt[2] = (float) ((_z[vtkSub])[i]);
			pt[1] = (float) ((_y[vtkSub])[i]);
			pt[0] = (float) ((_x[vtkSub])[i]);
			pcoords[vtkSub]->SetTuple((iv1+iv2), pt);
			//dData[vtkSub]->SetValue((iv1+iv2),(float)((_z[vtkSub])[i]));
			i++; // le i++ doit être ici, il faut commencer à zéro !!!
			iv2+=rows;
			iv3++;
			if(iv3>=cols)
			{
				iv2 = 0;
				iv3 = 0;
				iv1 += 1;
			}
		}
    }

	dataPoints[vtkSub]->Modified();
	pdata[vtkSub]->Update();


	i = 0;iv1 = 0; iv2 = 0; iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		for (col = 0; col<cols; col++)
		{
			dData[vtkSub]->SetValue((iv1+iv2),(float)(pdata[vtkSub]->GetOutput()->GetPoints()->GetPoint(iv1+iv2)[2]));		// make sure that depth data is the transformed value; :-( unable to avoid loop yet :-(
			i++; // le i++ doit être ici, il faut commencer à zéro !!!
			iv2+=rows;
			iv3++;
			if(iv3>=cols)
			{
				iv2 = 0;
				iv3 = 0;
				iv1 += 1;
			}
		}
    }

	data[vtkSub]->Modified();
	renWin->Render();
	return vtkSub;
}

/**
 * Updates the TOF points
 */
int CViewSrVtk::updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x, unsigned short* amp, int vtkSub)
{
	//if(!sr){return -1;};
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	if((!_x[vtkSub]) || (!_y[vtkSub]) || (!_z[vtkSub]) ){ allocXYZ(rows, cols, vtkSub); };

	int num=rows*cols;
	//SR_CoordTrfUint16(sr, _x[vtkSub],_y[vtkSub],_z[vtkSub], sizeof(short),sizeof(short), sizeof(WORD));
	memcpy((void*)_x[vtkSub], (void*) x, num*sizeof(short) );
	memcpy((void*)_y[vtkSub], (void*) y, num*sizeof(short) );
	memcpy((void*)_z[vtkSub], (void*) z, num*sizeof(unsigned short) );


	//pcoords->Reset();
	//pcoords->SetNumberOfComponents(3);

	// We ask pcoords to allocate room for at least 25344 tuples
	// and set the number of tuples to 4.
	if(num != 25344)
	{
		pcoords[vtkSub]->SetNumberOfTuples(num);
	}

	float pt[3];
	int row = 0; int col = 0;
	int i = 0; int iv1 = 0; int iv2 = 0; int iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		for (col = 0; col<cols; col++)
		{
			pt[2] = (float) ((_z[vtkSub])[i]);
			pt[1] = (float) ((_y[vtkSub])[i]);
			pt[0] = (float) ((_x[vtkSub])[i]);
			pcoords[vtkSub]->SetTuple((iv1+iv2), pt);
			//dData[vtkSub]->SetValue((iv1+iv2),(float)((_z[vtkSub])[i]));
			i++; // le i++ doit être ici, il faut commencer à zéro !!!
			iv2+=rows;
			iv3++;
			if(iv3>=cols)
			{
				iv2 = 0;
				iv3 = 0;
				iv1 += 1;
			}
		}
    }

	dataPoints[vtkSub]->Modified();
	pdata[vtkSub]->Update();


	i = 0;iv1 = 0; iv2 = 0; iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		for (col = 0; col<cols; col++)
		{
			dData[vtkSub]->SetValue((iv1+iv2),(float)(pdata[vtkSub]->GetOutput()->GetPoints()->GetPoint(iv1+iv2)[2]));		// make sure that depth data is the transformed value; :-( unable to avoid loop yet :-(
			aData[vtkSub]->SetValue((iv1+iv2),(float) amp[i]) ;		// make sure that depth data is the transformed value; :-( unable to avoid loop yet :-(
			i++; // le i++ doit être ici, il faut commencer à zéro !!!
			iv2+=rows;
			iv3++;
			if(iv3>=cols)
			{
				iv2 = 0;
				iv3 = 0;
				iv1 += 1;
			}
		}
    }

	data[vtkSub]->Modified();
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
 * Frees the scalar bar
 */
int CViewSrVtk::freeDataAct(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	//pdata[vtkSub]->SetInputConnection(data[vtkSub]->GetOutputPort());
	dataPoints[vtkSub]->Delete();
	pcoords[vtkSub]->Delete();
	freeXYZ(vtkSub);
	data[vtkSub]->Delete();
	pdata[vtkSub]->Delete();
	dataMapper[vtkSub]->Delete();
	dataActor[vtkSub]->Delete();
	//subSample->Delete();
	return vtkSub;
}

/**
 * alloc buffers for xyz values
 */
int CViewSrVtk::allocXYZ(int rows, int cols, int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	if((rows>512) || (rows<0) || (cols>512) || (cols<0)){ return -2;};

	if((_x[vtkSub]) || (_y[vtkSub]) || (_z[vtkSub]) ){ freeXYZ(vtkSub);};

	int num=rows*cols;
	_x[vtkSub] = (short*) malloc(num*sizeof(short)); memset( (void*) _x[vtkSub], 0x00, rows*cols*sizeof(short));
	_y[vtkSub] = (short*) malloc(num*sizeof(short)); memset( (void*) _y[vtkSub], 0x00, rows*cols*sizeof(short));
	_z[vtkSub] = (unsigned short*)  malloc(num*sizeof(unsigned short)); memset( (void*) _z[vtkSub], 0x00, rows*cols*sizeof(unsigned short));

	data[vtkSub]->SetWholeExtent(0,rows-1,0,cols-1,0,0);
	data[vtkSub]->ComputeBounds();
	int dim[3];
	data[vtkSub]->GetDimensions(dim);
	data[vtkSub]->SetDimensions(rows,cols,1);

	return vtkSub;
}

/**
 * free buffers for xyz values
 */
int CViewSrVtk::freeXYZ(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	SAFE_FREE(_x[vtkSub]);
	SAFE_FREE(_y[vtkSub]);
	SAFE_FREE(_z[vtkSub]);
	return vtkSub;
}

/**
 * alloc BG buffers for xyz values
 */
int CViewSrVtk::allocXYZbg(int rows, int cols, int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	if((_xBG[vtkSub]) || (_yBG[vtkSub]) || (_zBG[vtkSub]) ){ freeXYZbg(vtkSub);};

	int num=rows*cols;
	_xBG[vtkSub] = (short*) malloc(num*sizeof(short)); memset( (void*) _xBG[vtkSub], 0x00, rows*cols*sizeof(short));
	_yBG[vtkSub] = (short*) malloc(num*sizeof(short)); memset( (void*) _yBG[vtkSub], 0x00, rows*cols*sizeof(short));
	_zBG[vtkSub] = (unsigned short*)  malloc(num*sizeof(unsigned short)); memset( (void*) _zBG[vtkSub], 0x00, rows*cols*sizeof(unsigned short));

	return vtkSub;
}

/**
 * free buffers for xyz values
 */
int CViewSrVtk::freeXYZbg(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	SAFE_FREE(_xBG[vtkSub]);
	SAFE_FREE(_yBG[vtkSub]);
	SAFE_FREE(_zBG[vtkSub]);
	return vtkSub;
}
/**
 * Updates the TOF background points
 */
int CViewSrVtk::addBGDataAct(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	int rows = 144; // HARDCODED BAD BAD BAD
	int cols = 176;

	if((!_xBG[vtkSub]) || (!_yBG[vtkSub]) || (!_zBG[vtkSub]) ){ allocXYZbg(rows, cols, vtkSub); };

	int num = rows * cols;

	BGdataPoints[vtkSub] = vtkPoints::New();
	// Create a float array which represents the points.
    BGpcoords[vtkSub] = vtkFloatArray::New();
	BGdData[vtkSub] = vtkFloatArray::New();
	// Note that by default, an array has 1 component.
    // We have to change it to 3 for points
    BGpcoords[vtkSub]->SetNumberOfComponents(3);
	BGdData[vtkSub]->SetNumberOfComponents(1);
	// We ask pcoords to allocate room for at least 25344 tuples
	// and set the number of tuples to 4.
	BGpcoords[vtkSub]->SetNumberOfTuples(num);
	BGdData[vtkSub]->SetNumberOfTuples(num);

	// Assign each tuple
	float x,y,z;
	float xMin, xMax, yMin, yMax;
	xMin = -3000.0f; xMax = 3000.0f;	// some bogus init value
	yMin = -2000.0f; yMax = 2000.0f;	// some bogus init value
	z = 250.0f;							// some bogus init value
	float pt[3];
	int row = 0; int col = 0;
	int i = 0; int iv1 = 0; int iv2 = 0; int iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		y =  (  ((float)row )/144.0f * (yMax-yMin) + yMin);	//bogus value Y init
		for (col = 0; col<cols; col++)
		{
			x =  (  ((float)col )/176.0f * (xMax-xMin) + xMin);	// bogus value X init
			pt[2] = z;
			pt[1] = y;
			pt[0] = x;
			//pt[2] = (float) ((_zBG[vtkSub])[i]);
			//pt[1] = (float) ((_yBG[vtkSub])[i]);
			//pt[0] = (float) ((_xBG[vtkSub])[i]);
			BGpcoords[vtkSub]->SetTuple((iv1+iv2), pt);
			BGdData[vtkSub]->SetValue((iv1+iv2),(float)((_zBG[vtkSub])[i]));
			i++; // le i++ doit être ici, il faut commencer à zéro !!!
			iv2+=rows;
			iv3++;
			if(iv3>=cols)
			{
				iv2 = 0;
				iv3 = 0;
				iv1 += 1;
			}
		}
    }

	BGdataPoints[vtkSub]->SetData(BGpcoords[vtkSub]);

	BGdata[vtkSub] = vtkStructuredGrid::New();
	vtkPointData *BGptdata = BGdata[vtkSub]->GetPointData();
	BGdata[vtkSub]->SetPoints(BGdataPoints[vtkSub]);
	BGdata[vtkSub]->SetWholeExtent(0,rows-1,0,cols-1,0,0);
	BGptdata->SetScalars(BGdData[vtkSub]);
	BGdata[vtkSub]->ComputeBounds();
	int dim[3];
	BGdata[vtkSub]->GetDimensions(dim);
	BGdata[vtkSub]->SetDimensions(rows,cols,1);

	BGcamTranFilter[vtkSub]->SetInput(BGdata[vtkSub]);	// 20080118 transf

	BGpdata[vtkSub] = vtkStructuredGridGeometryFilter::New();
	// 20080118 transf	pBGpdata[vtkSub]->SetInput(BGdata[vtkSub]);
	BGpdata[vtkSub]->SetInput(BGcamTranFilter[vtkSub]->GetOutputDataObject(0)); // 20080118 transf
    BGdataMapper[vtkSub] = vtkPolyDataMapper::New();

	BGdataMapper[vtkSub]->SetInputConnection(BGpdata[vtkSub]->GetOutputPort());
	//BGdataMapper->ScalarVisibilityOff();

    BGdataActor[vtkSub] = vtkActor::New();
    BGdataActor[vtkSub]->SetMapper(BGdataMapper[vtkSub]);
    BGdataActor[vtkSub]->GetProperty()->SetOpacity(.2);
	//BGdataActor[vtkSub]->VisibilityOff();
	BGdataActor[vtkSub]->GetProperty()->SetRepresentationToSurface();
	BGdataActor[vtkSub]->GetProperty()->SetDiffuse(0.0);
	BGdataActor[vtkSub]->GetProperty()->SetSpecular(0.0);
	BGdataActor[vtkSub]->GetProperty()->SetAmbient(1.0);

	if(vtkSub==0){ BGdataActor[vtkSub]->GetProperty()->SetColor(0.0,0.0,1.0); }; // BLUE for 0-th cam
	if(vtkSub==1){ BGdataActor[vtkSub]->GetProperty()->SetColor(1.0,0.0,0.0); }; // RED  for 1-st add cam
	if(vtkSub==2){ BGdataActor[vtkSub]->GetProperty()->SetColor(0.0,1.0,0.0); }; // GREENfor 2-nd add cam
	if(vtkSub==3){ BGdataActor[vtkSub]->GetProperty()->SetColor(0.7,0.0,0.7); }; // PURPLfor 3-rd add cam

	//BGdataWriter[vtkSub]->SetInput(BGdata[vtkSub]); // write StructuredGrid data
	BGdataWriter[vtkSub]->SetInput(BGcamTranFilter[vtkSub]->GetOutputDataObject(0)); // 20080118 transf

#ifdef _DEBUG
	BGdataWriter[vtkSub]->SetFileTypeToASCII();
#else
	BGdataWriter[vtkSub]->SetFileTypeToBinary();
#endif

	renWin->Render();
	BGdataActor[vtkSub]->VisibilityOff();
	return vtkSub;
}

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
 * Frees the background data actor
 */
int CViewSrVtk::freeBGDataAct(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};

	BGdataPoints[vtkSub]->Delete();
	BGpcoords[vtkSub]->Delete();
	freeXYZbg(vtkSub);
	BGdata[vtkSub]->Delete();
	BGpdata[vtkSub]->Delete();
	BGdataMapper[vtkSub]->Delete();
	BGdataActor[vtkSub]->Delete();

	return vtkSub;
}

/**
 * Hides a data actor
 */
int CViewSrVtk::hideDataAct(int vtkSub, bool doHide)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	if(doHide){ dataActor[vtkSub]->VisibilityOff();}
	else{ dataActor[vtkSub]->VisibilityOn();}
	return vtkSub;
}

/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataActColorRGB(int vtkSub, double r, double g, double b)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	dataActor[vtkSub]->GetProperty()->SetColor(r,g,b);  // sets color
	return vtkSub;
}

/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorDepth(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	data[vtkSub]->GetPointData()->SetScalars(dData[vtkSub]);
	dataMapper[vtkSub]->SetLookupTable(depthLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorGray(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	data[vtkSub]->GetPointData()->SetScalars(aData[vtkSub]);
	dataMapper[vtkSub]->SetLookupTable(grayLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorR(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	dataMapper[vtkSub]->SetLookupTable(rLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorG(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	dataMapper[vtkSub]->SetLookupTable(gLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorB(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	dataMapper[vtkSub]->SetLookupTable(bLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorW(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	dataMapper[vtkSub]->SetLookupTable(wLUT);  // sets color
	return vtkSub;
}
/**
 * Sets a data actor color
 */
int CViewSrVtk::setDataMapperColorK(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return -1;};
	dataMapper[vtkSub]->SetLookupTable(kLUT);  // sets color
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

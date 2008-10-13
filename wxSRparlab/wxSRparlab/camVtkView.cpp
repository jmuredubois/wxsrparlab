#include "wxSRparlab.h"
#include "camVtkView.h"


/**
 * Constructor for CViewSrVtk
 *
 */
CamVtkView::CamVtkView(int vtkSub, vtkRenderWindow* ParRenWin, vtkLookupTable* LUT)
{
	_vtkSub = vtkSub;
	_x = NULL;
	_y = NULL;
	_z = NULL;

	_xBG = NULL;
	_yBG = NULL;
	_zBG = NULL;
	camTranMat = vtkMatrix4x4::New();

	dataWriter = vtkStructuredGridWriter::New();
	BGdataWriter = vtkStructuredGridWriter::New();

	camTran = vtkTransform::New();
	camTran->SetMatrix(camTranMat);
	camTranFilter = vtkTransformFilter::New();
	camTranFilter->SetTransform(camTran);
	BGcamTranFilter = vtkTransformFilter::New();
	BGcamTranFilter->SetTransform(camTran);

	// instantiate class fields
	// setup the parent window
	renWin = ParRenWin;
	// add a renderer
	renderer = ParRenWin->GetRenderers()->GetFirstRenderer();
	// add an interactor

	// add a shape representing the camera
	addSrCam();
	srLabelActor->SetCamera(renderer->GetActiveCamera());

	// add a data actor
	addDataAct();
	// add a BG data actor
	addBGDataAct();
	// create a depth LUT
	depthLUT = LUT;
	grayLUT = LUT;
	rLUT = LUT; gLUT = LUT; bLUT = LUT; wLUT = LUT; kLUT = LUT;

	#ifdef JMU_TGTFOLLOW
		// add a target actor
		addTgtAct();
	#endif

#ifndef VTKNOTRANSFORM
	char strTrfFile[512];
	sprintf(strTrfFile, "camTranMats/camTrfMat_mostRecent_cam%02i.xml", _vtkSub);
	setTrfMat(strTrfFile);
#endif// VTKNOTRANSFORM

	// add actors
	renderer->AddActor(srCubeActor);
	renderer->AddActor(dataActor);
	renderer->AddActor(BGdataActor);
	renderer->AddActor(srLabelActor);
};

/**
 * Destructor for CamVtkView
 *
 */
CamVtkView::~CamVtkView()
{
	camTran->Delete();
	camTranMat->Delete();
	camTranFilter->Delete();
	BGcamTranFilter->Delete();

	int res = 0;
	// free camera shape
	res+= freeSrCam();

	BGdataWriter->Delete();
	dataWriter->Delete();
	#ifdef JMU_TGTFOLLOW
	  // free tgt actor
	  res+= freeTgtAct();
	#endif
	// free data actor
	res+= freeBGDataAct();
	// free data actor
	res+= freeDataAct();
};

int CamVtkView::setVtkSub(int vtkSub)
{
	if((vtkSub >= _vtkSubMax) || (vtkSub<0)){ return _vtkSub;};
	_vtkSub = vtkSub;
	return _vtkSub;
}
int CamVtkView::getVtkSub()
{
	return _vtkSub;
}

int CamVtkView::setTrfMat(char* fn)
{
	int res = 0;

	try
	{
		ticpp::Document doc( fn );
		doc.LoadFile();

		char rowStr[64]; char attrStr[64]; double val=0.0;
		for(int row = 0; row <4; row++)
		{
			sprintf(rowStr, "Row%i", row);
			ticpp::Element* pRow = doc.FirstChildElement()->FirstChildElement(rowStr);
			for(int col = 0; col <4 ; col++)
			{
				sprintf(attrStr, "val%i", col);
				pRow->GetAttribute(attrStr, &val);
				camTranMat->SetElement(row,col,val);
			}
		}

	}
	catch( ticpp::Exception& ex )
	{
		std::cout << ex.what();
		return -1;
	}
	catch(...)
	{
		camTranMat->Identity(); // if a problem occured, set TrfMat to Identity
		return -1;
	}
	camTranMat->Modified();
	camTran->SetMatrix(camTranMat);
	camTran->Modified();
	srCube->SetCenter( camTranMat->GetElement(0,3),  camTranMat->GetElement(1,3),  camTranMat->GetElement(2,3));
	srLabelActor->SetPosition( camTranMat->GetElement(0,3),  camTranMat->GetElement(1,3),  camTranMat->GetElement(2,3));
	#ifdef JMU_TGTFOLLOW
		if( tgtLine != NULL)
		{
			tgtLine->SetPoint1(camTranMat->GetElement(0,3),camTranMat->GetElement(1,3),camTranMat->GetElement(2,3));
			tgtLine->SetPoint2(camTranMat->GetElement(0,3),camTranMat->GetElement(1,3),camTranMat->GetElement(2,3));
		}
	#endif
	return res;
}

/**
 * Adds the camera actor
 *  -> Creates new objects that must be deleted
 *  --> freeSrCam() must be called
 */
int CamVtkView::addSrCam()
{
	srCube = vtkCubeSource::New();
    srCube->SetXLength( 55.0 );			//!< HARDCODED SR CAMERA DIMENSIONS
    srCube->SetYLength( 75.0 );
    srCube->SetZLength( 55.0 );
	srCube->SetCenter( camTranMat->GetElement(0,3),  camTranMat->GetElement(1,3),  camTranMat->GetElement(2,3));
    srCubeMapper = vtkPolyDataMapper::New();
    srCubeMapper->SetInputConnection(srCube->GetOutputPort());
    srCubeActor = vtkActor::New();
    srCubeActor->SetMapper(srCubeMapper);
    srCubeActor->GetProperty()->SetColor(0.0,0.0,1.0);	//!< HARDCODED SR CAMERA COLOR

    srLabel = vtkVectorText::New();
	char camLab[64];
	sprintf(camLab, "TOF cam.%d", _vtkSub);
    srLabel->SetText(camLab);						//!< HARDCODED SR CAMERA LABEL TEXT
    srLabelMapper = vtkPolyDataMapper::New();
    srLabelMapper->SetInputConnection(srLabel->GetOutputPort());
    srLabelActor = vtkFollower::New();
    srLabelActor->SetMapper(srLabelMapper);
    srLabelActor->SetScale(50.0,50.0,50.0);				//!< HARDCODED SR CAMERA LABEL TEXT SIZE
	srLabelActor->SetPosition( camTranMat->GetElement(0,3),  camTranMat->GetElement(1,3),  camTranMat->GetElement(2,3));
    srLabelActor->AddPosition(-50.0,-50.0,-50.0);				//!< HARDCODED SR CAMERA LABEL TEXT POSITION
	srLabelActor->SetCamera(renderer->GetActiveCamera());
	if(_vtkSub==0){ srLabelActor->GetProperty()->SetColor(0.0,0.0,1.0); }; // BLUE for 0-th cam
	if(_vtkSub==1){ srLabelActor->GetProperty()->SetColor(1.0,0.0,0.0); }; // RED  for 1-st add cam
	if(_vtkSub==2){ srLabelActor->GetProperty()->SetColor(0.0,1.0,0.0); }; // GREENfor 2-nd add cam
	if(_vtkSub==3){ srLabelActor->GetProperty()->SetColor(0.7,0.0,0.7); }; // PURPLfor 3-rd add cam

	return srLabelActor->GetReferenceCount();
}

/**
 * Frees the camera actor objects
 */
int CamVtkView::freeSrCam()
{
	int res = srLabelActor->GetReferenceCount();
	srCube->Delete();
	srCubeMapper->Delete();
	srCubeActor->Delete();
	srLabel->Delete();
	srLabelMapper->Delete();
	srLabelActor->Delete();
	return res-1;
}

/**
 * Changes the depth range
 */
int CamVtkView::changeDepthRange(float minVal, float maxVal)
{
	int res = 0;
	dataMapperZ->SetScalarRange((double) minVal, (double) maxVal);
	dataMapperZ->Modified();
	BGdataMapperZ->SetScalarRange((double) minVal, (double) maxVal);
	BGdataMapperZ->Modified();
	return res;
}

/**
 * Changes the amplitude range
 */
int CamVtkView::changeAmpRange(float minAmp, float maxAmp)
{
	int res = 0;
	dataMapperAmp->SetScalarRange((double) minAmp, (double) maxAmp);
	dataMapperAmp->Modified();
	return res;
}

void CamVtkView::SetDepthLUT(vtkLookupTable* LUT)
{
	if(LUT==NULL){return;};
	depthLUT = LUT;
}
void CamVtkView::SetGrayLUT(vtkLookupTable* LUT)
{
	if(LUT==NULL){return;};
	grayLUT = LUT;
}
void CamVtkView::SetPlainLUT(vtkLookupTable* LUTr, vtkLookupTable* LUTg, vtkLookupTable* LUTb, vtkLookupTable* LUTw, vtkLookupTable* LUTk)
{
	if((LUTr==NULL) || (LUTg==NULL) || (LUTb==NULL) || (LUTw==NULL) || (LUTk==NULL) ){return;};
	rLUT = LUTr;
	gLUT = LUTg;
	bLUT = LUTb;
	wLUT = LUTw;
	kLUT = LUTk;
}

/**
 * Creates a data actor
 *  ->
 *  --> freeDataAct must be called in cleanup
 */
int CamVtkView::addDataAct()
{
	int res = 0;

	dataPoints = vtkPoints::New();
	// Create a float array which represents the points.
    pcoords = vtkFloatArray::New();
	dData = vtkFloatArray::New();	// scalar depth associated to each point (for coloring)
	aData = vtkFloatArray::New();	// scalar depth associated to each point (for coloring)
	// Note that by default, an array has 1 component.
    // We have to change it to 3 for points
    pcoords->SetNumberOfComponents(3);
	dData->SetNumberOfComponents(1);
	aData->SetNumberOfComponents(1);
	// We ask pcoords to allocate room for at least 25344 tuples
	// and set the number of tuples to 4.
	pcoords->SetNumberOfTuples(25344);
	dData->SetNumberOfTuples(25344);
	aData->SetNumberOfTuples(25344);

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
			pcoords->SetTuple((iv1+iv2), pt);
			dData->SetValue((iv1+iv2),z);
			aData->SetValue((iv1+iv2),100.0f);
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

	dataPoints->SetData(pcoords);


	data = vtkStructuredGrid::New();
	vtkPointData *ptdata = data->GetPointData();
	data->SetPoints(dataPoints);
	data->SetWholeExtent(0,143,0,175,0,0);
	ptdata->SetScalars(dData);
	data->ComputeBounds();
	int dim[3];
	data->GetDimensions(dim);
	data->SetDimensions(144,176,1);

	camTranFilter->SetInput(data);	// 20080118 transf

	pdata = vtkStructuredGridGeometryFilter::New();
	// 20080118 transf	pdata->SetInput(data);
	pdata->SetInput(camTranFilter->GetOutputDataObject(0)); // 20080118 transf
    dataMapperZ = vtkPolyDataMapper::New();
	dataMapperZ->SetInputConnection(pdata->GetOutputPort());

	dataMapperAmp = vtkPolyDataMapper::New();
	dataMapperAmp->SetInputConnection(pdata->GetOutputPort());

    dataActor = vtkActor::New();
    dataActor->SetMapper(dataMapperZ);
	dataMapperZ->Register(dataActor);
	dataMapperAmp->Register(dataActor);
    dataActor->GetProperty()->SetRepresentationToPoints();
	dataActor->GetProperty()->SetPointSize(3.0);			//! HARD CODED POINT SIZE
	dataActor->GetProperty()->SetDiffuse(0.0);
	dataActor->GetProperty()->SetSpecular(0.0);
	dataActor->GetProperty()->SetAmbient(1.0);

	//dataWriter->SetInput(data); // write StructuredGrid data
	dataWriter->SetInput(camTranFilter->GetOutputDataObject(0)); // 20080118 transf

#ifdef _DEBUG
	dataWriter->SetFileTypeToASCII();
#else
	dataWriter->SetFileTypeToBinary();
#endif

	return res;
}

/**
 * Updates the TOF points
 */
int CamVtkView::updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x)
{
	int res = 0;

	if((!_x) || (!_y) || (!_z) ){ allocXYZ(rows, cols); };

	int num=rows*cols;
	//SR_CoordTrfUint16(sr, _x,_y,_z, sizeof(short),sizeof(short), sizeof(WORD));
	memcpy((void*)_x, (void*) x, num*sizeof(short) );
	memcpy((void*)_y, (void*) y, num*sizeof(short) );
	memcpy((void*)_z, (void*) z, num*sizeof(unsigned short) );


	//pcoords->Reset();
	//pcoords->SetNumberOfComponents(3);

	// We ask pcoords to allocate room for at least 25344 tuples
	// and set the number of tuples to 4.
	if(num != 25344)
	{
		pcoords->SetNumberOfTuples(num);
	}

	float pt[3];
	int row = 0; int col = 0;
	int i = 0; int iv1 = 0; int iv2 = 0; int iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		for (col = 0; col<cols; col++)
		{
			pt[2] = (float) ((_z)[i]);
			pt[1] = (float) ((_y)[i]);
			pt[0] = (float) ((_x)[i]);
			pcoords->SetTuple((iv1+iv2), pt);
			//dData->SetValue((iv1+iv2),(float)((_z)[i]));
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

	dataPoints->Modified();
	pdata->Update();


	i = 0;iv1 = 0; iv2 = 0; iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		for (col = 0; col<cols; col++)
		{
			dData->SetValue((iv1+iv2),(float)(pdata->GetOutput()->GetPoints()->GetPoint(iv1+iv2)[2]));		// make sure that depth data is the transformed value; :-( unable to avoid loop yet :-(
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

	data->Modified();
	renWin->Render();
	return res;
}

/**
 * Updates the TOF points
 */
int CamVtkView::updateTOFcurrent(int rows, int cols, unsigned short *z, short *y, short *x, unsigned short* amp)
{
	int res = 0;
	//if(!sr){return -1;};

	if((!_x) || (!_y) || (!_z) ){ allocXYZ(rows, cols); };

	int num=rows*cols;
	//SR_CoordTrfUint16(sr, _x,_y,_z, sizeof(short),sizeof(short), sizeof(WORD));
	memcpy((void*)_x, (void*) x, num*sizeof(short) );
	memcpy((void*)_y, (void*) y, num*sizeof(short) );
	memcpy((void*)_z, (void*) z, num*sizeof(unsigned short) );


	//pcoords->Reset();
	//pcoords->SetNumberOfComponents(3);

	// We ask pcoords to allocate room for at least 25344 tuples
	// and set the number of tuples to 4.
	if(num != 25344)
	{
		pcoords->SetNumberOfTuples(num);
	}

	float pt[3];
	int row = 0; int col = 0;
	int i = 0; int iv1 = 0; int iv2 = 0; int iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		for (col = 0; col<cols; col++)
		{
			pt[2] = (float) ((_z)[i]);
			pt[1] = (float) ((_y)[i]);
			pt[0] = (float) ((_x)[i]);
			pcoords->SetTuple((iv1+iv2), pt);
			//dData->SetValue((iv1+iv2),(float)((_z)[i]));
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

	dataPoints->Modified();
	pdata->Update();


	i = 0;iv1 = 0; iv2 = 0; iv3 = 0;
	for (row = 0 ; row <rows; row++)
    {
		for (col = 0; col<cols; col++)
		{
			dData->SetValue((iv1+iv2),(float)(pdata->GetOutput()->GetPoints()->GetPoint(iv1+iv2)[2]));		// make sure that depth data is the transformed value; :-( unable to avoid loop yet :-(
			aData->SetValue((iv1+iv2),(float) amp[i]) ;		// make sure that depth data is the transformed value; :-( unable to avoid loop yet :-(
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

	data->Modified();
	renWin->Render();
	return res;
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
int CamVtkView::freeDataAct()
{
	int res = dataActor->GetReferenceCount();
	//pdata->SetInputConnection(data->GetOutputPort());
	dataPoints->Delete();
	pcoords->Delete();
	freeXYZ();
	data->Delete();
	pdata->Delete();
	dData->Delete();
	aData->Delete();
	/*while(dataMapperZ->GetReferenceCount()>1){
		dataMapperZ->Delete();
	};*/
	dataMapperZ->Delete();
	/*while(dataMapperAmp->GetReferenceCount()>1){
		dataMapperAmp->Delete();
	};*/
	dataMapperAmp->Delete();
	dataActor->Delete();
	return res-1;
}

/**
 * alloc buffers for xyz values
 */
int CamVtkView::allocXYZ(int rows, int cols)
{
	int res = 0;
	if((rows>512) || (rows<0) || (cols>512) || (cols<0)){ return -2;};

	if((_x) || (_y) || (_z) ){ freeXYZ();};

	int num=rows*cols;
	_x = (short*) malloc(num*sizeof(short)); memset( (void*) _x, 0x00, rows*cols*sizeof(short));
	_y = (short*) malloc(num*sizeof(short)); memset( (void*) _y, 0x00, rows*cols*sizeof(short));
	_z = (unsigned short*)  malloc(num*sizeof(unsigned short)); memset( (void*) _z, 0x00, rows*cols*sizeof(unsigned short));

	data->SetWholeExtent(0,rows-1,0,cols-1,0,0);
	data->ComputeBounds();
	int dim[3];
	data->GetDimensions(dim);
	data->SetDimensions(rows,cols,1);

	return res;
}

/**
 * free buffers for xyz values
 */
int CamVtkView::freeXYZ()
{
	int res = 0;
	SAFE_FREE(_x);
	SAFE_FREE(_y);
	SAFE_FREE(_z);
	return res;
}

/**
 * alloc BG buffers for xyz values
 */
int CamVtkView::allocXYZbg(int rows, int cols)
{
	int res = 0;
	if((_xBG) || (_yBG) || (_zBG) ){ freeXYZbg();};

	int num=rows*cols;
	_xBG = (short*) malloc(num*sizeof(short)); memset( (void*) _xBG, 0x00, rows*cols*sizeof(short));
	_yBG = (short*) malloc(num*sizeof(short)); memset( (void*) _yBG, 0x00, rows*cols*sizeof(short));
	_zBG = (unsigned short*)  malloc(num*sizeof(unsigned short)); memset( (void*) _zBG, 0x00, rows*cols*sizeof(unsigned short));

	return res;
}

/**
 * free buffers for xyz values
 */
int CamVtkView::freeXYZbg()
{
	int res = 0;
	SAFE_FREE(_xBG);
	SAFE_FREE(_yBG);
	SAFE_FREE(_zBG);
	return res;
}
/**
 * Updates the TOF background points
 */
int CamVtkView::addBGDataAct()
{
	int res = 0;

	int rows = 144; // HARDCODED BAD BAD BAD
	int cols = 176;

	if((!_xBG) || (!_yBG) || (!_zBG) ){ allocXYZbg(rows, cols); };

	int num = rows * cols;

	BGdataPoints = vtkPoints::New();
	// Create a float array which represents the points.
    BGpcoords = vtkFloatArray::New();
	BGdData = vtkFloatArray::New();
	// Note that by default, an array has 1 component.
    // We have to change it to 3 for points
    BGpcoords->SetNumberOfComponents(3);
	BGdData->SetNumberOfComponents(1);
	// We ask pcoords to allocate room for at least 25344 tuples
	// and set the number of tuples to 4.
	BGpcoords->SetNumberOfTuples(num);
	BGdData->SetNumberOfTuples(num);

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
			//pt[2] = (float) ((_zBG)[i]);
			//pt[1] = (float) ((_yBG)[i]);
			//pt[0] = (float) ((_xBG)[i]);
			BGpcoords->SetTuple((iv1+iv2), pt);
			BGdData->SetValue((iv1+iv2),(float)((_zBG)[i]));
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

	BGdataPoints->SetData(BGpcoords);

	BGdata = vtkStructuredGrid::New();
	vtkPointData *BGptdata = BGdata->GetPointData();
	BGdata->SetPoints(BGdataPoints);
	BGdata->SetWholeExtent(0,rows-1,0,cols-1,0,0);
	BGptdata->SetScalars(BGdData);
	BGdata->ComputeBounds();
	int dim[3];
	BGdata->GetDimensions(dim);
	BGdata->SetDimensions(rows,cols,1);

	BGcamTranFilter->SetInput(BGdata);	// 20080118 transf

	BGpdata = vtkStructuredGridGeometryFilter::New();
	// 20080118 transf	pBGpdata->SetInput(BGdata);
	BGpdata->SetInput(BGcamTranFilter->GetOutputDataObject(0)); // 20080118 transf
    BGdataMapperZ = vtkPolyDataMapper::New();

	BGdataMapperZ->SetInputConnection(BGpdata->GetOutputPort());
	//BGdataMapperZ->ScalarVisibilityOff();

    BGdataActor = vtkActor::New();
    BGdataActor->SetMapper(BGdataMapperZ);
    BGdataActor->GetProperty()->SetOpacity(.2);
	//BGdataActor->VisibilityOff();
	BGdataActor->GetProperty()->SetRepresentationToSurface();
	BGdataActor->GetProperty()->SetDiffuse(0.0);
	BGdataActor->GetProperty()->SetSpecular(0.0);
	BGdataActor->GetProperty()->SetAmbient(1.0);

	if(_vtkSub==0){ BGdataActor->GetProperty()->SetColor(0.0,0.0,1.0); }; // BLUE for 0-th cam
	if(_vtkSub==1){ BGdataActor->GetProperty()->SetColor(1.0,0.0,0.0); }; // RED  for 1-st add cam
	if(_vtkSub==2){ BGdataActor->GetProperty()->SetColor(0.0,1.0,0.0); }; // GREENfor 2-nd add cam
	if(_vtkSub==3){ BGdataActor->GetProperty()->SetColor(0.7,0.0,0.7); }; // PURPLfor 3-rd add cam

	//BGdataWriter->SetInput(BGdata); // write StructuredGrid data
	BGdataWriter->SetInput(BGcamTranFilter->GetOutputDataObject(0)); // 20080118 transf

#ifdef _DEBUG
	BGdataWriter->SetFileTypeToASCII();
#else
	BGdataWriter->SetFileTypeToBinary();
#endif

	renWin->Render();
	BGdataActor->VisibilityOff();
	return res;
}

/**
 * Updates the TOF background points
 */
//int CViewSrVtk::updateTOFbg(SRCAM sr, SRPARLAB srPL)
//{
//	int res = 0;
//	if(!sr){return -1;};
//
//	if((!_xBG) || (!_yBG) || (!_zBG) ){
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
//	SR_CoordTrfUint16(sr, _xBG,_yBG,0, sizeof(short),sizeof(short), sizeof(WORD));
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
//	BGpcoords->SetNumberOfTuples(num);
//	BGdData->SetNumberOfTuples(num);
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
//			pt[1] = (float)((_yBG)[i]);
//			pt[0] = (float)((_xBG)[i]);
//			BGpcoords->SetTuple((iv1+iv2), pt);
//			BGdData->SetValue((iv1+iv2),(float)zBG[i]);
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
//	BGdataActor->VisibilityOn();
//	BGdataPoints->Modified();
//
//	i = 0;iv1 = 0; iv2 = 0; iv3 = 0;
//	for (row = 0 ; row <rows; row++)
//    {
//		for (col = 0; col<cols; col++)
//		{
//			vtkPolyData* totoPoly = BGpdata->GetOutput();
//			vtkPoints* totoPoints = totoPoly->GetPoints();
//			if(!totoPoints) {
//				return -3; // WHY WHY WHY IS A NULLPOINTER RETURNED ?
//			};
//			double* totoDouble = totoPoints->GetPoint(iv1+iv2);
//			BGdData->SetValue((iv1+iv2),(float)(BGpdata->GetOutput()->GetPoints()->GetPoint(iv1+iv2)[2]));		// make sure that depth data is the transformed value; :-( unable to avoid loop yet :-(
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
//	BGdataActor->VisibilityOn();
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
//	BGdataWriter->SetFileName(fname);
//	res+=BGdataWriter->Write();
//
//	return res;
//}

/**
 * Frees the background data actor
 */
int CamVtkView::freeBGDataAct()
{
	int res = BGdataActor->GetReferenceCount();
	BGdataPoints->Delete();
	BGpcoords->Delete();
	freeXYZbg();
	BGdata->Delete();
	BGpdata->Delete();
	BGdataMapperZ->Delete();
	BGdataActor->Delete();
	return res-1;
}

/**
 * Hides a data actor
 */
void CamVtkView::hideDataAct(bool doHide)
{
	if(doHide)
	{ 
		dataActor->VisibilityOff();
		srCubeActor->VisibilityOff();
		srLabelActor->VisibilityOff();
		#ifdef JMU_TGTFOLLOW 
			tgtLineActor->VisibilityOff();
		#endif

	}
	else
	{ 
		dataActor->VisibilityOn();
		srCubeActor->VisibilityOn();
		srLabelActor->VisibilityOn();
		#ifdef JMU_TGTFOLLOW 
			tgtLineActor->VisibilityOn(); 
		#endif
	}
}

/**
 * Sets a data actor color
 */
void CamVtkView::setDataActColorRGB(double r, double g, double b)
{
	dataActor->GetProperty()->SetColor(r,g,b);  // sets color
}

/**
 * Sets a data actor color
 */
void CamVtkView::setDataMapperColorDepth()
{
	data->GetPointData()->SetScalars(dData);
	dataMapperZ->SetLookupTable(depthLUT);  // sets color
	dataActor->SetMapper(dataMapperZ);

}
/**
 * Sets a data actor color
 */
void CamVtkView::setDataMapperColorGray()
{
	data->GetPointData()->SetScalars(aData);
	dataMapperAmp->SetLookupTable(grayLUT);  // sets color
	dataActor->SetMapper(dataMapperAmp);
}
/**
 * Sets a data actor color
 */
void CamVtkView::setDataMapperColorR()
{
	dataMapperAmp->SetLookupTable(rLUT);  // sets color
	dataActor->SetMapper(dataMapperAmp);
}
/**
 * Sets a data actor color
 */
void CamVtkView::setDataMapperColorG()
{
	dataMapperAmp->SetLookupTable(gLUT);  // sets color
	dataActor->SetMapper(dataMapperAmp);
}
/**
 * Sets a data actor color
 */
void CamVtkView::setDataMapperColorB()
{
	dataMapperAmp->SetLookupTable(bLUT);  // sets color
	dataActor->SetMapper(dataMapperAmp);
}
/**
 * Sets a data actor color
 */
void CamVtkView::setDataMapperColorW()
{
	dataMapperAmp->SetLookupTable(wLUT);  // sets color
	dataActor->SetMapper(dataMapperAmp);
}
/**
 * Sets a data actor color
 */
void CamVtkView::setDataMapperColorK()
{
	dataMapperAmp->SetLookupTable(kLUT);  // sets color
	dataActor->SetMapper(dataMapperAmp);
}

#ifdef JMU_TGTFOLLOW
int CamVtkView::setTgtFile(char* fn)
{
	int res = 0;
	//#ifdef JMU_TGTFOLLOW
	//#endif
	return res;
}
#endif

/**
 * Updates the tracking end point
 */
#ifdef JMU_TGTFOLLOW
int CamVtkView::updateTarget(float x, float y, float z)
{
	int res = 0;
	if((tgtLine != NULL))
	{
		tgtLine->SetPoint2((double) x,(double) y,(double) z);
		tgtLine->Modified();
		//renWin->Render();
	}
	return res;
}
int CamVtkView::updateTarget()
{
	int res = 0;
	if((tgtLine != NULL) && (camTranMat != NULL) )
	{
		tgtLine->SetPoint2(camTranMat->GetElement(0,3),camTranMat->GetElement(1,3),camTranMat->GetElement(2,3));
		tgtLine->Modified();
		//renWin->Render();
	}
	return res;
}
#endif
#ifdef JMU_TGTFOLLOW
int CamVtkView::updateTarget(float *ptsF, int nCoord)
{
	int res = 0;
	if ( (nCoord != 3) && (nCoord !=15)) {return -1;} ; // return if wrong number of coord is passed
	if((tgtLine != NULL) && (ptsF != NULL))
	{
		tgtLine->SetPoint2((double) ptsF[0],(double) ptsF[1],(double) ptsF[2]);
		tgtLine->Modified();
		//renWin->Render();
		if(nCoord==15)
		{
			for(int k=0; k<4; k++)
			{
				tgtQuadPoints->SetPoint(k, ptsF[(3*(k+1))+0],(double) ptsF[(3*(k+1))+1],(double) ptsF[(3*(k+1))+2]);
			}
			tgtQuadPoints->Modified();
		}
	}
	return res;
}
#endif
#ifdef JMU_TGTFOLLOW
int CamVtkView::addTgtAct()
{
	int res = 0;
	tgtLine = vtkLineSource::New();
	tgtLine->SetPoint1(camTranMat->GetElement(0,3),camTranMat->GetElement(1,3),camTranMat->GetElement(2,3));
	tgtLine->SetPoint2(camTranMat->GetElement(0,3),camTranMat->GetElement(1,3),camTranMat->GetElement(2,3));
	tgtLineMapper = vtkPolyDataMapper::New();
	tgtLineMapper->SetInputConnection(tgtLine->GetOutputPort());
	tgtLineActor = vtkActor::New();
	tgtLineActor->SetMapper(tgtLineMapper);
	tgtLineActor->GetProperty()->SetColor(0.0,0.0,1.0);	//!< HARDCODED SR CAMERA COLOR
	tgtLineActor->GetProperty()->SetLineWidth(5.0f);
	if(_vtkSub==0){ tgtLineActor->GetProperty()->SetColor(0.0,0.0,1.0); }; // BLUE for 0-th cam
	if(_vtkSub==1){ tgtLineActor->GetProperty()->SetColor(1.0,0.0,0.0); }; // RED  for 1-st add cam
	if(_vtkSub==2){ tgtLineActor->GetProperty()->SetColor(0.0,1.0,0.0); }; // GREENfor 2-nd add cam
	if(_vtkSub==3){ tgtLineActor->GetProperty()->SetColor(0.7,0.0,0.7); }; // PURPLfor 3-rd add cam
	renderer->AddActor(tgtLineActor);

	tgtQuadPoints = vtkPoints::New();
	tgtQuadPoints->SetNumberOfPoints(4);
	tgtQuadPoints->InsertPoint(0, 0.0, 0.0,0.0);
	tgtQuadPoints->InsertPoint(1, 1.0, 0.0,0.0);
	tgtQuadPoints->InsertPoint(2, 1.0, 1.0,0.0);
	tgtQuadPoints->InsertPoint(3, 0.0, 1.0,0.0);
	tgtQuad = vtkQuad::New();
	tgtQuad->GetPointIds()->SetNumberOfIds(4);
	for(int k=0; k<4; k++)
	{
		tgtQuad->GetPointIds()->SetId(k, k);
	}
	tgtQuadGrid = vtkUnstructuredGrid::New();
	tgtQuadGrid->Allocate(1,1);
	tgtQuadGrid->InsertNextCell(tgtQuad->GetCellType(), tgtQuad->GetPointIds());
	tgtQuadGrid->SetPoints(tgtQuadPoints);
	tgtQuadMapper = vtkDataSetMapper::New();
	tgtQuadMapper->SetInput(tgtQuadGrid);
	tgtQuadActor = vtkActor::New();
	tgtQuadActor->SetMapper(tgtQuadMapper);
	tgtQuadActor->GetProperty()->SetOpacity(0.5); //transparency
	if(_vtkSub==0){ tgtQuadActor->GetProperty()->SetColor(0.0,0.0,1.0); }; // BLUE for 0-th cam
	if(_vtkSub==1){ tgtQuadActor->GetProperty()->SetColor(1.0,0.0,0.0); }; // RED  for 1-st add cam
	if(_vtkSub==2){ tgtQuadActor->GetProperty()->SetColor(0.0,1.0,0.0); }; // GREENfor 2-nd add cam
	if(_vtkSub==3){ tgtQuadActor->GetProperty()->SetColor(0.7,0.0,0.7); }; // PURPLfor 3-rd add cam
	renderer->AddActor(tgtQuadActor);
	
	return res;
}
#endif
#ifdef JMU_TGTFOLLOW
int CamVtkView::freeTgtAct()
{
	int res = tgtLineActor->GetReferenceCount();
	tgtLine->Delete();
	tgtLineMapper->Delete();
	tgtLineActor->Delete();

	tgtQuadPoints->Delete();
	tgtQuad->Delete();
	tgtQuadMapper->Delete();
	tgtQuadActor->Delete();
	return res-1;
}
#endif
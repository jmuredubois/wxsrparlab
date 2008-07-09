	// //  // HARDCODED VALUES BAD BAD BAD 20080115
	camTranMat[1]->SetElement(0,0, 0.9986);
	camTranMat[1]->SetElement(0,1, 0.0055);
	camTranMat[1]->SetElement(0,2,-0.0529);
	camTranMat[1]->SetElement(0,3, 243.4947);
	camTranMat[1]->SetElement(1,0,-0.0129);
	camTranMat[1]->SetElement(1,1, 0.9901);
	camTranMat[1]->SetElement(1,2,-0.1397);
	camTranMat[1]->SetElement(1,3,-006.3524);
	camTranMat[1]->SetElement(2,0, 0.0516);
	camTranMat[1]->SetElement(2,1, 0.1402);
	camTranMat[1]->SetElement(2,2, 0.9888);
	camTranMat[1]->SetElement(2,3, 030.6955);
	camTranMat[1]->SetElement(3,0,0);
	camTranMat[1]->SetElement(3,1,0);
	camTranMat[1]->SetElement(3,2,0);
	camTranMat[1]->SetElement(3,3,1);
	
	camTranMat[1]->Invert();
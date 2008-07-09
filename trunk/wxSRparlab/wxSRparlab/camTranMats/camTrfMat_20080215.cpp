		// //  // HARDCODED VALUES BAD BAD BAD 20080215
	camTranMat[1]->SetElement(0,0, 0.9672);
	camTranMat[1]->SetElement(0,1,-0.0581);
	camTranMat[1]->SetElement(0,2, 0.2043);
	camTranMat[1]->SetElement(0,3,-231.78);
	camTranMat[1]->SetElement(1,0, 0.1083);
	camTranMat[1]->SetElement(1,1, 0.9189);
	camTranMat[1]->SetElement(1,2,-0.2373);
	camTranMat[1]->SetElement(1,3,-079.38);
	camTranMat[1]->SetElement(2,0,-0.0967);
	camTranMat[1]->SetElement(2,1, 0.3464);
	camTranMat[1]->SetElement(2,2, 0.7947);
	camTranMat[1]->SetElement(2,3, 275.37);
	camTranMat[1]->SetElement(3,0,0);
	camTranMat[1]->SetElement(3,1,0);
	camTranMat[1]->SetElement(3,2,0);
	camTranMat[1]->SetElement(3,3,1);

	camTranMat[1]->SetElement(0,3,-231.78); // +0
	camTranMat[1]->SetElement(1,3, 050.62); // +130
	camTranMat[1]->SetElement(2,3, 055.37); // -220

	camTranMat[1]->SetElement(0,3,-231.78); // +0
	camTranMat[1]->SetElement(1,3,-079.38);  // +0
	camTranMat[1]->SetElement(2,3, 400.37); // +0
	camTranMat[1]->Invert();
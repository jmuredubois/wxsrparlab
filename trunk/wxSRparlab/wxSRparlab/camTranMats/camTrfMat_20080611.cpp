	// //  // HARDCODED VALUES BAD BAD BAD 20080611
/*
Transformation matrix for SR3100 camera (left) :
    0.9985   -0.0134    0.0526         0
         0    0.9689    0.2474         0
   -0.0543   -0.2471    0.9675         0
         0         0         0    1.0000

Transformation matrix for SR3000 camera (right) :
    0.9987    0.0123   -0.0486  391.5607
         0    0.9695    0.2450   49.4331
    0.0501   -0.2447    0.9683  119.7802
         0         0         0    1.0000
*/
	camTranMat[0]->SetElement(0,0, 0.9985);
	camTranMat[0]->SetElement(0,1,-0.0134);
	camTranMat[0]->SetElement(0,2, 0.0526);
	camTranMat[0]->SetElement(0,3, 0);
	camTranMat[0]->SetElement(1,0, 0.0000);
	camTranMat[0]->SetElement(1,1, 0.9689);
	camTranMat[0]->SetElement(1,2, 0.2474);
	camTranMat[0]->SetElement(1,3, 0);
	camTranMat[0]->SetElement(2,0,-0.0543);
	camTranMat[0]->SetElement(2,1,-0.2471);
	camTranMat[0]->SetElement(2,2, 0.9675);
	camTranMat[0]->SetElement(2,3, 0);
	camTranMat[0]->SetElement(3,0,0);
	camTranMat[0]->SetElement(3,1,0);
	camTranMat[0]->SetElement(3,2,0);
	camTranMat[0]->SetElement(3,3,1);

	camTranMat[1]->SetElement(0,0, 0.9987);
	camTranMat[1]->SetElement(0,1, 0.0123);
	camTranMat[1]->SetElement(0,2,-0.0486);
	camTranMat[1]->SetElement(0,3, 391.5607);
	camTranMat[1]->SetElement(1,0, 0.0000);
	camTranMat[1]->SetElement(1,1, 0.9695);
	camTranMat[1]->SetElement(1,2, 0.2450);
	camTranMat[1]->SetElement(1,3, 049.4331);
	camTranMat[1]->SetElement(2,0, 0.0501);
	camTranMat[1]->SetElement(2,1,-0.2447);
	camTranMat[1]->SetElement(2,2, 0.9683);
	camTranMat[1]->SetElement(2,3, 119.7802);
	camTranMat[1]->SetElement(3,0,0);
	camTranMat[1]->SetElement(3,1,0);
	camTranMat[1]->SetElement(3,2,0);
	camTranMat[1]->SetElement(3,3,1);

//	// //  // HARDCODED VALUES BAD BAD BAD 200800707 cmpSteRansac
/*
Transformation matrix for SR3100 camera (left) :
    0.8860   -0.1263    0.4462         0
         0    0.9622    0.2724         0
   -0.4638   -0.2413    0.8525         0
         0         0         0    1.0000

Transformation matrix for SR3000 camera (right) :
    0.9422    0.0836   -0.3246  723.7250
         0    0.9684    0.2495  130.9202
    0.3352   -0.2351    0.9124   82.0860
         0         0         0    1.0000
*/
camTranMat[0]->SetElement(0,0,+0000.886);
camTranMat[0]->SetElement(0,1,-000.1263);
camTranMat[0]->SetElement(0,2,+000.4462);
camTranMat[0]->SetElement(0,3,+00000000);
camTranMat[0]->SetElement(1,0,+00000000);
camTranMat[0]->SetElement(1,1,+000.9622);
camTranMat[0]->SetElement(1,2,+000.2724);
camTranMat[0]->SetElement(1,3,+00000000);
camTranMat[0]->SetElement(2,0,-000.4638);
camTranMat[0]->SetElement(2,1,-000.2413);
camTranMat[0]->SetElement(2,2,+000.8525);
camTranMat[0]->SetElement(2,3,+00000000);
camTranMat[0]->SetElement(3,0,+00000000);
camTranMat[0]->SetElement(3,1,+00000000);
camTranMat[0]->SetElement(3,2,+00000000);
camTranMat[0]->SetElement(3,3,+00000001);
camTranMat[1]->SetElement(0,0,+000.9422);
camTranMat[1]->SetElement(0,1,+00.08362);
camTranMat[1]->SetElement(0,2,-000.3246);
camTranMat[1]->SetElement(0,3,+000723.7);
camTranMat[1]->SetElement(1,0,+00000000);
camTranMat[1]->SetElement(1,1,+000.9684);
camTranMat[1]->SetElement(1,2,+000.2495);
camTranMat[1]->SetElement(1,3,+000130.9);
camTranMat[1]->SetElement(2,0,+000.3352);
camTranMat[1]->SetElement(2,1,-000.2351);
camTranMat[1]->SetElement(2,2,+000.9124);
camTranMat[1]->SetElement(2,3,+00082.09);
camTranMat[1]->SetElement(3,0,+00000000);
camTranMat[1]->SetElement(3,1,+00000000);
camTranMat[1]->SetElement(3,2,+00000000);
camTranMat[1]->SetElement(3,3,+00000001);
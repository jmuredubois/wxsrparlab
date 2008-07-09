//	// //  // HARDCODED VALUES BAD BAD BAD 20080618 paralax2
/*
Transformation matrix for SR3100 camera (left) :
    0.9523   -0.0783    0.2951         0
         0    0.9666    0.2565         0
   -0.3053   -0.2442    0.9204         0
         0         0         0    1.0000

Transformation matrix for SR3000 camera (right) :
    0.9502    0.0797   -0.3012  659.1207
         0    0.9667    0.2558   50.6574
    0.3115   -0.2431    0.9186   94.3331
         0         0         0    1.0000
*/
camTranMat[0]->SetElement(0,0,+000.9523);
camTranMat[0]->SetElement(0,1,-00.07829);
camTranMat[0]->SetElement(0,2,+000.2951);
camTranMat[0]->SetElement(0,3,+00000000);
camTranMat[0]->SetElement(1,0,+00000000);
camTranMat[0]->SetElement(1,1,+000.9666);
camTranMat[0]->SetElement(1,2,+000.2565);
camTranMat[0]->SetElement(1,3,+00000000);
camTranMat[0]->SetElement(2,0,-000.3053);
camTranMat[0]->SetElement(2,1,-000.2442);
camTranMat[0]->SetElement(2,2,+000.9204);
camTranMat[0]->SetElement(2,3,+00000000);
camTranMat[0]->SetElement(3,0,+00000000);
camTranMat[0]->SetElement(3,1,+00000000);
camTranMat[0]->SetElement(3,2,+00000000);
camTranMat[0]->SetElement(3,3,+00000001);
camTranMat[1]->SetElement(0,0,+000.9502);
camTranMat[1]->SetElement(0,1,+00.07969);
camTranMat[1]->SetElement(0,2,-000.3012);
camTranMat[1]->SetElement(0,3,+000659.1);
camTranMat[1]->SetElement(1,0,+00000000);
camTranMat[1]->SetElement(1,1,+000.9667);
camTranMat[1]->SetElement(1,2,+000.2558);
camTranMat[1]->SetElement(1,3,+00050.66);
camTranMat[1]->SetElement(2,0,+000.3115);
camTranMat[1]->SetElement(2,1,-000.2431);
camTranMat[1]->SetElement(2,2,+000.9186);
camTranMat[1]->SetElement(2,3,+00094.33);
camTranMat[1]->SetElement(3,0,+00000000);
camTranMat[1]->SetElement(3,1,+00000000);
camTranMat[1]->SetElement(3,2,+00000000);
camTranMat[1]->SetElement(3,3,+00000001);
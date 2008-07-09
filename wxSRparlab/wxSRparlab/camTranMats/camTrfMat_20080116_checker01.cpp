/*
Transformation matrix for SR3100 camera (left) :
    0.8375    0.1896   -0.5125         0
         0    0.9379    0.3470         0
    0.5464   -0.2906    0.7855         0
         0         0         0    1.0000

Transformation matrix for SR3000 camera (right) :
  1.0e+003 *

    0.0010    0.0001    0.0002   -2.0336
         0    0.0009   -0.0003    1.7415
   -0.0002    0.0003    0.0009   -0.1891
         0         0         0    0.0010

*/
camTranMat[0]->SetElement(0,0,+0.83751);
camTranMat[0]->SetElement(0,1,+0.18961);
camTranMat[0]->SetElement(0,2,-0.51247);
camTranMat[0]->SetElement(0,3,+000000);
camTranMat[0]->SetElement(1,0,+000000);
camTranMat[0]->SetElement(1,1,+0.93786);
camTranMat[0]->SetElement(1,2,+0.34701);
camTranMat[0]->SetElement(1,3,+000000);
camTranMat[0]->SetElement(2,0,+0.54643);
camTranMat[0]->SetElement(2,1,-0.29062);
camTranMat[0]->SetElement(2,2,+0.78547);
camTranMat[0]->SetElement(2,3,+000000);
camTranMat[0]->SetElement(3,0,+000000);
camTranMat[0]->SetElement(3,1,+000000);
camTranMat[0]->SetElement(3,2,+000000);
camTranMat[0]->SetElement(3,3,+000001);
camTranMat[1]->SetElement(0,0,+0.97974);
camTranMat[1]->SetElement(0,1,+0.065393);
camTranMat[1]->SetElement(0,2,+0.18931);
camTranMat[1]->SetElement(0,3,-2033.6);
camTranMat[1]->SetElement(1,0,+000000);
camTranMat[1]->SetElement(1,1,+0.9452);
camTranMat[1]->SetElement(1,2,-0.3265);
camTranMat[1]->SetElement(1,3,+1741.5);
camTranMat[1]->SetElement(2,0,-0.20028);
camTranMat[1]->SetElement(2,1,+0.31989);
camTranMat[1]->SetElement(2,2,+0.92604);
camTranMat[1]->SetElement(2,3,-189.09);
camTranMat[1]->SetElement(3,0,+000000);
camTranMat[1]->SetElement(3,1,+000000);
camTranMat[1]->SetElement(3,2,+000000);
camTranMat[1]->SetElement(3,3,+000001);
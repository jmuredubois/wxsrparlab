/*Transformation matrix for SR3100 camera (left) :
    0.9938    0.0326   -0.1063         0
         0    0.9560    0.2933         0
    0.1112   -0.2914    0.9501         0
         0         0         0    1.0000

Transformation matrix for SR3000 camera (right) :
    0.9967    0.0004    0.0811 -203.3422
         0    1.0000   -0.0048   45.8914
   -0.0811    0.0048    0.9967   53.5204
         0         0         0    1.0000
*/

camTranMat[0]->SetElement(0,0,+000.9938);
camTranMat[0]->SetElement(0,1,+00.03261);
camTranMat[0]->SetElement(0,2,-000.1063);
camTranMat[0]->SetElement(0,3,+00000000);
camTranMat[0]->SetElement(1,0,+00000000);
camTranMat[0]->SetElement(1,1,+0000.956);
camTranMat[0]->SetElement(1,2,+000.2933);
camTranMat[0]->SetElement(1,3,+00000000);
camTranMat[0]->SetElement(2,0,+000.1112);
camTranMat[0]->SetElement(2,1,-000.2914);
camTranMat[0]->SetElement(2,2,+000.9501);
camTranMat[0]->SetElement(2,3,+00000000);
camTranMat[0]->SetElement(3,0,+00000000);
camTranMat[0]->SetElement(3,1,+00000000);
camTranMat[0]->SetElement(3,2,+00000000);
camTranMat[0]->SetElement(3,3,+00000001);
camTranMat[1]->SetElement(0,0,+000.9967);
camTranMat[1]->SetElement(0,1,+0.0003928);
camTranMat[1]->SetElement(0,2,+00.08113);
camTranMat[1]->SetElement(0,3,-000203.3);
camTranMat[1]->SetElement(1,0,+00000000);
camTranMat[1]->SetElement(1,1,+00000001);
camTranMat[1]->SetElement(1,2,-0.004842);
camTranMat[1]->SetElement(1,3,+00045.89);
camTranMat[1]->SetElement(2,0,-00.08113);
camTranMat[1]->SetElement(2,1,+0.004826);
camTranMat[1]->SetElement(2,2,+000.9967);
camTranMat[1]->SetElement(2,3,+00053.52);
camTranMat[1]->SetElement(3,0,+00000000);
camTranMat[1]->SetElement(3,1,+00000000);
camTranMat[1]->SetElement(3,2,+00000000);
camTranMat[1]->SetElement(3,3,+00000001);
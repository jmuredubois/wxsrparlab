//	// //  // HARDCODED VALUES BAD BAD BAD 20080618 paralax
 /*Transformation matrix for SR3100 camera (left) :
    0.9533   -0.0796    0.2914         0
         0    0.9647    0.2634         0
   -0.3021   -0.2511    0.9196         0
         0         0         0    1.0000

Transformation matrix for SR3000 camera (right) :
    0.9505    0.0808   -0.3000  660.3361
         0    0.9656    0.2601   46.4838
    0.3107   -0.2472    0.9178   77.7271
         0         0         0    1.0000
 */
camTranMat[0]->SetElement(0,0,+000.9533);
camTranMat[0]->SetElement(0,1,-00.07957);
camTranMat[0]->SetElement(0,2,+000.2914);
camTranMat[0]->SetElement(0,3,+00000000);
camTranMat[0]->SetElement(1,0,+00000000);
camTranMat[0]->SetElement(1,1,+000.9647);
camTranMat[0]->SetElement(1,2,+000.2634);
camTranMat[0]->SetElement(1,3,+00000000);
camTranMat[0]->SetElement(2,0,-000.3021);
camTranMat[0]->SetElement(2,1,-000.2511);
camTranMat[0]->SetElement(2,2,+000.9196);
camTranMat[0]->SetElement(2,3,+00000000);
camTranMat[0]->SetElement(3,0,+00000000);
camTranMat[0]->SetElement(3,1,+00000000);
camTranMat[0]->SetElement(3,2,+00000000);
camTranMat[0]->SetElement(3,3,+00000001);
camTranMat[1]->SetElement(0,0,+000.9505);
camTranMat[1]->SetElement(0,1,+00.08081);
camTranMat[1]->SetElement(0,2,-000000.3);
camTranMat[1]->SetElement(0,3,+000660.3);
camTranMat[1]->SetElement(1,0,+00000000);
camTranMat[1]->SetElement(1,1,+000.9656);
camTranMat[1]->SetElement(1,2,+000.2601);
camTranMat[1]->SetElement(1,3,+00046.48);
camTranMat[1]->SetElement(2,0,+000.3107);
camTranMat[1]->SetElement(2,1,-000.2472);
camTranMat[1]->SetElement(2,2,+000.9178);
camTranMat[1]->SetElement(2,3,+00077.73);
camTranMat[1]->SetElement(3,0,+00000000);
camTranMat[1]->SetElement(3,1,+00000000);
camTranMat[1]->SetElement(3,2,+00000000);
camTranMat[1]->SetElement(3,3,+00000001);

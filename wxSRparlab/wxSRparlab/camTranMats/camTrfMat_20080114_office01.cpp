Transformation matrix for SR3100 camera (left) :
    0.9589    0.0111   -0.2835         0
         0    0.9992    0.0390         0
    0.2837   -0.0374    0.9582         0
         0         0         0    1.0000

Transformation matrix for SR3000 camera (right) :
    0.9785    0.0313   -0.2040 -338.3298
         0    0.9884    0.1519   65.5484
    0.2064   -0.1486    0.9671   89.1489
         0         0         0    1.0000

camTranMat[0]->SetElement(0,0,+000.9589);
camTranMat[0]->SetElement(0,1,+00.01108);
camTranMat[0]->SetElement(0,2,-000.2835);
camTranMat[0]->SetElement(0,3,+00000000);
camTranMat[0]->SetElement(1,0,+00000000);
camTranMat[0]->SetElement(1,1,+000.9992);
camTranMat[0]->SetElement(1,2,+00.03904);
camTranMat[0]->SetElement(1,3,+00000000);
camTranMat[0]->SetElement(2,0,+000.2837);
camTranMat[0]->SetElement(2,1,-00.03744);
camTranMat[0]->SetElement(2,2,+000.9582);
camTranMat[0]->SetElement(2,3,+00000000);
camTranMat[0]->SetElement(3,0,+00000000);
camTranMat[0]->SetElement(3,1,+00000000);
camTranMat[0]->SetElement(3,2,+00000000);
camTranMat[0]->SetElement(3,3,+00000001);
camTranMat[1]->SetElement(0,0,+000.9785);
camTranMat[1]->SetElement(0,1,+00.03134);
camTranMat[1]->SetElement(0,2,-0000.204);
camTranMat[1]->SetElement(0,3,-000338.3);
camTranMat[1]->SetElement(1,0,+00000000);
camTranMat[1]->SetElement(1,1,+000.9884);
camTranMat[1]->SetElement(1,2,+000.1519);
camTranMat[1]->SetElement(1,3,+00065.55);
camTranMat[1]->SetElement(2,0,+000.2064);
camTranMat[1]->SetElement(2,1,-000.1486);
camTranMat[1]->SetElement(2,2,+000.9671);
camTranMat[1]->SetElement(2,3,+00089.15);
camTranMat[1]->SetElement(3,0,+00000000);
camTranMat[1]->SetElement(3,1,+00000000);
camTranMat[1]->SetElement(3,2,+00000000);
camTranMat[1]->SetElement(3,3,+00000001);
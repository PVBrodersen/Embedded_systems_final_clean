#include <iostream>
#include <cmath>

using namespace std;

float myNun[2];

void inverse(float x, float y){
    float l1 = 1;
    float l2 = 1;
    float d = 2*l1*l2;
    float n = (x*x+y*y)-(l1*l1-l2*l2);
    float o1 = atan(y/x);
    float o2 = atan(n/d);
    float myNum[] = {o1,o2};
}


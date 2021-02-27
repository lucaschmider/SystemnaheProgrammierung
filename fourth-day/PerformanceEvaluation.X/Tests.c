#include "Tests.h"
#include <math.h>
#include <xc.h>
#define DATATYPE float

void calculation(void) {
    Nop();
    DATATYPE K = 187;
    DATATYPE M = 257; 
    Nop();
    DATATYPE N = K / M;
    Nop();
    
    return;
}

void trigonometry(void) {
    float v;
    Nop();
    v = sin(0);
    Nop();
    v = sin(1);
    Nop();
    v = sin(M_PI_2);
    Nop();
    
    v = cos(0);
    Nop();
    v = cos(1);
    Nop();
    v = cos(M_PI_2);
    Nop();
    
    v = tan(0);
    Nop();
    v = tan(1);
    Nop();
    v = tan(M_PI_2);
    Nop();
    return;
}
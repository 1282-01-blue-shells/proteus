#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"

void testingback();
void testingforward();

int main() {
  ProteOS::registerFunction("testingback", &testingback);
  ProteOS::registerFunction("testingforward",&testingforward);

    ProteOS::run();   
}

void testingback(){
    Motors::drive(-40); 
}

void testingforward(){
    Motors::drive(40);
}
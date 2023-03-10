#include "proteos.hpp"
#include "navigation.hpp"

#include "FEHMotor.h"
#include "FEHUtility.h"

void testingback();
void testingforward();
void abortTest();

int main() {
    ProteOS::registerFunction("testingback", &testingback);
    ProteOS::registerFunction("testingforward", &testingforward);
    ProteOS::registerFunction("abortTest", &abortTest);

    ProteOS::run();   
}

void testingback(){
    Motors::drive(-40); 
}

void testingforward(){
    Motors::drive(40);
}

void abortTest() {
    Motors::start(true);
    Debugger::printNextLine("no cap?");
    throw new AbortException();
}
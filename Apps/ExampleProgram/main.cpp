#include "proteos.hpp"
#include "FEHUtility.h"


void stanley();
float square(float x);
void testSquare();
void nestedCallExample();
void largeFunction();

int var1 = 23;
float var2 = 5.6;

float squareInput = 2;

int main() {
    registerIOVariable("var1", &var1);
    registerIOVariable("var2", &var2);
    registerIOVariable("squareInput", &squareInput);

    registerIOFunction("stanley()", &stanley);
    registerIOFunction("testSquare()", &testSquare);
    registerIOFunction("nestedCallExample()", &nestedCallExample);
    
    openIOMenu();
}

// Example code for printing and breakpoints
void stanley() {
    printLineF(2, "This is the story of a");
    printLineF(3, "man named Stanley.");

    breakpoint();

    printLineF(5, "Stanley worked for a");
    printLineF(6, "company in a big building");
    printLineF(7, "where he was employee");
    printLineF(8, "number 427.");

    breakpoint();

    clearDebugLog();
    printLineF(0, "Employee Number 427's job");
    printLineF(1, "was simple: he sat at his");
    printLineF(2, "desk in room 427, and he");
    printLineF(3, "pushed buttons on a");
    printLineF(4, "keyboard.");

    breakpoint();

    printLineF(6, "Orders came to him");
    printLineF(7, "through a monitor on his");
    printLineF(8, "desk, telling him what");
    printLineF(9, "buttons to push, how long");
    printLineF(10, "to push them, and in what");
    printLineF(11, "order.");
}



// Example for how to test a function with multiple different inputs
void testSquare() {
    float result = square(squareInput);
    printLineF(2, "%.3f squared is: %.3f", squareInput, result);
}

float square(float x) {
    return x*x;
}



// Example of a function that calls another function, which takes a while and can be aborted.
void nestedCallExample() {
    double startTime = TimeNow();

    largeFunction();
    
    printNextLineF("Elapsed time: %f", TimeNow() - startTime);
}

void largeFunction() {
    for (int i = 0; i < 5; i++) {
        printNextLineF("%i/5", i+1);
        sleepWithAbortCheck(1.0);
    }
}
#include "proteos.hpp"
#include "FEHUtility.h"


void stanley();
float square(float x);
void testSquare();
void nestedCallExample();
void largeFunction();
void itsJoever();

int var1 = 23;
float var2 = 5.6f;

float squareInput = 2;

int main(void) {
    ProteOS::registerVariable("var1", &var1);
    ProteOS::registerVariable("var2", &var2);
    ProteOS::registerVariable("squareInput", &squareInput);

    ProteOS::registerFunction("stanley()", &stanley);
    ProteOS::registerFunction("testSquare()", &testSquare);
    ProteOS::registerFunction("nestedCallExample()", &nestedCallExample);
    ProteOS::registerFunction("itsJoever()", &itsJoever);

    ProteOS::run();
}

// Example code for printing and breakpoints
void stanley() {
    Debugger::printLine(2, "This is the story of a");
    Debugger::printLine(3, "man named Stanley.");

    Debugger::breakpoint();

    Debugger::printLine(5, "Stanley worked for a");
    Debugger::printLine(6, "company in a big building");
    Debugger::printLine(7, "where he was employee");
    Debugger::printLine(8, "number 427.");

    Debugger::breakpoint();

    Debugger::clear();
    Debugger::printLine(0, "Employee Number 427's job");
    Debugger::printLine(1, "was simple: he sat at his");
    Debugger::printLine(2, "desk in room 427, and he");
    Debugger::printLine(3, "pushed buttons on a");
    Debugger::printLine(4, "keyboard.");

    Debugger::breakpoint();

    Debugger::printLine(6, "Orders came to him");
    Debugger::printLine(7, "through a monitor on his");
    Debugger::printLine(8, "desk, telling him what");
    Debugger::printLine(9, "buttons to push, how long");
    Debugger::printLine(10, "to push them, and in what");
    Debugger::printLine(11, "order.");
}



// Example for how to test a function with multiple different inputs
void testSquare() {
    float result = square(squareInput);
    Debugger::printWrap(2, "%.3f squared is: %.3f", squareInput, result);
}

float square(float x) {
    return x*x;
}



// Example of a function that calls another function, which takes a while and can be aborted.
void nestedCallExample() {
    double startTime = TimeNow();

    largeFunction();

    Debugger::printNextLine("Elapsed time: %f", TimeNow() - startTime);
}

void largeFunction() {
    for (int i = 0; i < 5; i++) {
        Debugger::printNextLine("%i/5", i+1);
        Debugger::sleep(1.0);
    }
}


// Example of an assertion
void itsJoever() {
    Debugger::printLine(1, "hm! omoshiroi...");
    Debugger::sleep(3);
    Debugger::printLine(3, "..demo!");
    Debugger::sleep(1.5);
    Debugger::printLine(4, "joewari da!");
    Debugger::sleep(2.5);
    
    assertTrue(false, "[Biden Blast]!");
}
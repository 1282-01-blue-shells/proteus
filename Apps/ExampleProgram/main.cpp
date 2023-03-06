#include "proteos.hpp"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "FEHUtility.h"


void stanley();
float square(float x);
void testSquare();
void computePi();
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
    ProteOS::registerFunction("computePi()", &computePi);
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



// Example of a function that takes a while and can be aborted.
void computePi() {
    float targetTime = TimeNow() + 10;

    for (int n = 1;; n++) { // start by computing one digit, then two, then three, and so on
        int m = 10*n/3 + 1;
        int A[m]; // this is a buckets.
        int digits[n]; // output
        for (int i = 0; i < m; i++) {
            A[i] = 2; // because pi is 2.22222... in base n/(2n+1)
        }
        for (int d = 0; d < n; d++) {
            if (TimeNow() > targetTime) break; // Stop if it's been too long
            for (int i = 0; i < m; i++) {
                A[i] *= 10; // Uhh because base 10?
            }
            for (int i = m-1; i > 0; i--) {
                int bd = 2*i + 1; // the denominator of the base at this bucket
                A[i-1] += A[i] / bd * i; // vaguely reminiscent of "carrying the one" in addition
                A[i] %= bd;
            }
            digits[d] = A[0]/10; // we got a digit now
            A[0] %= 10;
            if (digits[d] == 10) { // sometimes the digit fucking sucks
                digits[d] = 0;
                int i = d-1;
                while (digits[i] == 9) {
                    digits[i] = 0;
                    i--;
                }
                digits[i]++;
            }
        }
        if (TimeNow() > targetTime) break;
        char buf[287];
        sprintf(buf, "Pi = %i.", digits[0]);
        for (int i = 0; i < n; i++) {
            if (i+7 > 286) break;
            buf[i+7] = digits[i] + '0'; // mmm i love segfaults
        }
        buf[(n+7 < 286) ? n+7 : 286] = '\0'; // can't forget your null terminator

        Debugger::printWrap(1, "%s", buf);
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
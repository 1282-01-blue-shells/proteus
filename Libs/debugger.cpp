#include "debugger.hpp"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "exception"

#include "navigation.hpp"

#include "FEHLCD.h"


// Static variable definitions

int Debugger::backgroundColor = 0x000000;
int Debugger::defaultFontColor = 0x00E0F0;
int Debugger::errorColor = 0xFF0000;

bool Debugger::inDebugger = false;
int Debugger::debuggerFontColor = Debugger::defaultFontColor;

char Debugger::debuggerText[HEIGHT_CHARS][WIDTH_CHARS + 1];


// Function definitions

AssertionException::AssertionException(const char* functionName_, int lineNumber_, const char* message_) {
    functionName = functionName_;
    lineNumber = lineNumber_;
    message = message_;
}

void Debugger::debugFunction(const char* functionName, void (*funcPtr)()) {
    inDebugger = true;

    setFontColor();
    clear();

    LCD.Clear(backgroundColor);
    LCD.SetFontColor(debuggerFontColor);
    LCD.WriteRC("Abort", 13, 21);
    printLine(0, "Debugger: %s", functionName);
    printLine(12, "Touch to run function.");

    float x, y;
    waitUntilPressAndRelease(&x, &y);

    printLine(12, "");

    try {
        abortCheck();
        (*funcPtr)();
        printLine(12, "Completed. Touch to close.");
    } catch (AbortException* e) {
        setFontColor(errorColor);
        printLine(12, "Aborted. Touch to close.");
        delete e;
    } catch (AssertionException* e) {
        setFontColor(errorColor);
        printLine(9, "Assertion Failed at");
        printLine(10, "line %i in %s", e->lineNumber, e->functionName);
        printLine(11, "%s", e->message);
        printLine(12, "Touch to close.");
        delete e;
    }

    Motors::stop();

    // if pressed, wait until release
    while (LCD.Touch(&x, &y));
    
    waitUntilPressAndRelease(NULL, NULL);

    inDebugger = false;
}


void Debugger::printLine(int row, const char* format, ...) {
    if (!inDebugger || row < 0 || row >= HEIGHT_CHARS) return;
    va_list valist;
    va_start(valist, format);
    vsnprintf(debuggerText[row], WIDTH_CHARS, format, valist);
    LCD.SetFontColor(backgroundColor);
    LCD.FillRectangle(0, row*17, 320, 17);
    LCD.SetFontColor(debuggerFontColor);
    LCD.WriteRC(debuggerText[row], row, 0);
    va_end(valist);
}

void Debugger::printAppend(int row, const char* format, ...) {
    if (!inDebugger || row < 0 || row >= HEIGHT_CHARS) return;
    va_list valist;
    va_start(valist, format);
    LCD.SetFontColor(debuggerFontColor);
    int currentLength = strlen(debuggerText[row]);
    vsnprintf(debuggerText[row] + currentLength, WIDTH_CHARS - currentLength, format, valist);
    LCD.WriteRC(debuggerText[row], row, 0);
    va_end(valist);
}

int Debugger::printNextLine(const char* format, ...) {
    if (!inDebugger) return -1;
    va_list valist;
    va_start(valist, format);
    LCD.SetFontColor(debuggerFontColor);
    int row = -1;
    for (int i = 0; i < HEIGHT_CHARS; i++) {
        if (strlen(debuggerText[i]) == 0) {
            row = i;
            vsnprintf(debuggerText[row], WIDTH_CHARS, format, valist);
            LCD.WriteRC(debuggerText[row], row, 0);
            break;
        }
    }
    va_end(valist);
    return row;
}

void Debugger::printWrap(int startRow, const char* format, ...) {
    if (!inDebugger || startRow < 0 || startRow >= HEIGHT_CHARS) return;

    char buf[WIDTH_CHARS*HEIGHT_CHARS + 1];
    va_list valist;
    va_start(valist, format);

    vsnprintf(buf, WIDTH_CHARS*HEIGHT_CHARS, format, valist);

    char* bufPos = buf;
    int currentRow = startRow;
    while (currentRow < HEIGHT_CHARS && strlen(bufPos) > 0) {
        strncpy(debuggerText[currentRow], bufPos, WIDTH_CHARS);
        LCD.SetFontColor(backgroundColor);
        LCD.FillRectangle(0, currentRow*17, 320, 17);
        LCD.SetFontColor(debuggerFontColor);
        LCD.WriteRC(debuggerText[currentRow], currentRow, 0);
        bufPos += strlen(debuggerText[currentRow]);
        currentRow++;
    }
    
    va_end(valist);
}

void Debugger::setFontColor(int color) {
    debuggerFontColor = color;
}

void Debugger::setFontColor() {
    debuggerFontColor = defaultFontColor;
}

void Debugger::clear() {
    for (int i = 0; i < 13; i++) {
        printLine(i, "");
    }
}

void Debugger::breakpoint() {
    if (!inDebugger) return;
    float x, y;
    printLine(12, "Touch to continue.");

    waitUntilPressAndRelease(&x, &y);

    printLine(12, "");

    if (x > 240 && y > 200) {
        throw new AbortException();
    }
}

bool Debugger::breakpoint(float timeout) {
    if (!inDebugger) return false;
    float xr, yr, x = 0, y = 0;
    double targetTime = TimeNow() + timeout;
    printLine(12, "Touch or wait to continue.");

    while (!LCD.Touch(&xr, &yr) && TimeNow() < targetTime);
    while (LCD.Touch(&xr, &yr) && TimeNow() < targetTime) {
        x = xr; y = yr;
    };

    printLine(12, "");

    bool timedOut = TimeNow() >= targetTime;

    if (!timedOut && x > 240 && y > 200) {
        throw new AbortException();
    }

    return timedOut;
}


void Debugger::abortCheck() {
    if (!inDebugger) return;
    float x, y;
    if (LCD.Touch(&x, &y) && x > 240 && y > 200) {
        throw new AbortException();
    }
}

void Debugger::sleep(float time) {
    double targetTime = TimeNow() + time;
    while (TimeNow() < targetTime) {
        abortCheck();
    }
}

void Debugger::waitUntilPressAndRelease(float* x, float* y) {
    if (x == NULL && y == NULL) {
        float throwaway;
        while (!LCD.Touch(&throwaway, &throwaway));
        while (LCD.Touch(&throwaway, &throwaway));
    } else {
        float xRead, yRead;
        float xActual = -1, yActual = -1;
        while (xActual < 0 || yActual < 0) {
            // Wait until the screen is pressed down (read values do not matter)
            while (!LCD.Touch(&xRead, &yRead));
            // Wait until the screen is released, and copy the position while pressed
            while (LCD.Touch(&xRead, &yRead)) {
                if (xRead >= 0 && yRead >= 0) {
                    xActual = xRead;
                    yActual = yRead;
                }
            }
        }
        if (x != NULL) *x = xActual;
        if (y != NULL) *y = yActual;
    }
}
#include "proteos.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <exception>

#include <FEHLCD.h>
#include <FEHUtility.h>
#include <FEHBattery.h>


// Constants

#define BACKGROUND_COLOR 0x5040A0
#define FOREGROUND_COLOR 0xD0C0FF

#define DEBUGGER_BACKGROUND_COLOR 0x000000
#define DEBUGGER_FOREGROUND_COLOR 0x00E0F0

#define WIDTH_CHARS 26
#define HEIGHT_CHARS 13


#define BUFFER_SIZE 30

// Classes

enum UIState {
    Menu,
    LookingAtIOVars,
    LookingAtIOFuncs,
    AccessingIOVars,
    // EditingVar,
    AccessingIOFuncs,
    RanFunc
};

struct AbortException : public std::exception {};


// private function declarations

void drawScreen();
void waitForInput();
void editVariable();
void startDebugger();

void drawFolderIcon(int x, int y);


// global variables

UIState uiState = UIState::Menu;

const char* ioVarNames[MAX_IO_VARIABLES];
bool ioVarIsFP[MAX_IO_VARIABLES];
void* ioVars[MAX_IO_VARIABLES];

const char* ioFuncNames[MAX_IO_FUNCTIONS];
void (*ioFuncs[MAX_IO_FUNCTIONS])();

int currentIOVars = 0;
int currentIOFuncs = 0;

int selectedIOVar = 0;
int selectedIOFunc = 0;


bool inDebugger = false;
bool functionAborted = false;
char debuggerText[HEIGHT_CHARS][WIDTH_CHARS + 1];


// Function definitions

void registerIOVariable(const char* variableName, int* varPtr) {
    if (currentIOVars == MAX_IO_VARIABLES) return;
    ioVarNames[currentIOVars] = variableName;
    ioVarIsFP[currentIOVars] = false;
    ioVars[currentIOVars] = varPtr;
    currentIOVars++;
}

void registerIOVariable(const char* variableName, float* varPtr) {
    if (currentIOVars == MAX_IO_VARIABLES) return;
    ioVarNames[currentIOVars] = variableName;
    ioVarIsFP[currentIOVars] = true;
    ioVars[currentIOVars] = varPtr;
    currentIOVars++;
}

void registerIOFunction(const char* functionName, void (*funcPtr)()) {
    if (currentIOFuncs == MAX_IO_FUNCTIONS) return;
    ioFuncNames[currentIOFuncs] = functionName;
    ioFuncs[currentIOFuncs] = funcPtr;
    currentIOFuncs++;
}

void openIOMenu() {
    while (true) {
        drawScreen();
        waitForInput();
    }
}

void drawScreen() {
    LCD.Clear(BACKGROUND_COLOR);
    LCD.SetFontColor(FOREGROUND_COLOR);

    char buf[BUFFER_SIZE + 1];

    LCD.WriteAt(Battery.Voltage(), 232, 4);
    LCD.WriteAt("V", 304, 4);

    LCD.WriteAt("--------------------------", 4, 16);

    switch (uiState) {
        case UIState::Menu:
            LCD.WriteAt("Menu", 40, 4);
            LCD.WriteAt("ProteOS", 118, 112);
            LCD.WriteAt("Vars", 10, 220);
            drawFolderIcon(4, 164);
            LCD.WriteAt("Funcs", 76, 220);
            drawFolderIcon(76, 164);
            break;

        case UIState::LookingAtIOVars:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt("Variables", 40, 4);
            for (int i = 0; i < currentIOVars; i++) {
                LCD.WriteAt(ioVarNames[i], 16, 40 + 24*i);
                if (ioVarIsFP[i]) {
                    snprintf(buf, BUFFER_SIZE, "%.3f", *((float*) ioVars[i]));
                } else {
                    snprintf(buf, BUFFER_SIZE, "%i", *((int*) ioVars[i]));
                }

                // Cut it off if it's too long
                if (strlen(buf) > 8) {
                    strcpy(buf+5, "...");
                }
                LCD.WriteAt(buf, 292 - 12*strlen(buf), 40 + 24*i);
            }
            break;
        case UIState::LookingAtIOFuncs:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt("Functions", 40, 4);
            for (int i = 0; i < currentIOFuncs; i++) {
                LCD.WriteAt(ioFuncNames[i], 16, 40 + 24*i);
            }
            break;

        case UIState::AccessingIOVars:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt(ioVarNames[selectedIOVar], 40, 4);
            //void* ptr = ioVars[selectedIOVar];

            if (ioVarIsFP[selectedIOVar]) {
                LCD.WriteAt("Type: float", 16, 40);
            } else {
                LCD.WriteAt("Type: int", 16, 40);
            }

            if (ioVarIsFP[selectedIOVar]) {
                snprintf(buf, BUFFER_SIZE, "Value: %f", *((float*) ioVars[selectedIOVar]));
            } else {
                snprintf(buf, BUFFER_SIZE, "Value: %i", *((int*) ioVars[selectedIOVar]));
            }
            LCD.WriteAt(buf, 16, 64);

            LCD.DrawRectangle(120, 200, 80, 32);
            LCD.WriteAt("Edit", 136, 208);
            break;

        case UIState::AccessingIOFuncs:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt(ioFuncNames[selectedIOFunc], 40, 4);
            LCD.DrawRectangle(20, 160, 120, 60);
            LCD.DrawRectangle(180, 160, 120, 60);
            LCD.WriteAt("Run", 62, 182);
            LCD.WriteAt("Debug", 210, 182);
            break;

        case UIState::RanFunc:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt(ioFuncNames[selectedIOFunc], 40, 4);
            LCD.DrawRectangle(20, 160, 120, 60);
            LCD.DrawRectangle(180, 160, 120, 60);
            LCD.WriteAt("Run", 62, 182);
            LCD.WriteAt("Debug", 210, 182);

            LCD.WriteAt("Running...", 16, 40);
            LCD.WriteAt("Completed", 16, 64);
            break;

    }

}

void drawFolderIcon(int x, int y) {
    LCD.DrawHorizontalLine(y+10, x+10, x+25);
    LCD.DrawHorizontalLine(y+15, x+25, x+50);
    LCD.DrawHorizontalLine(y+40, x+10, x+50);
    LCD.DrawVerticalLine(x+10, y+10, y+40);
    LCD.DrawVerticalLine(x+50, y+15, y+40);
    LCD.DrawVerticalLine(x+25, y+10, y+15);
}

void waitForInput() {
    float x, y;
    while (!LCD.Touch(&x, &y));
    while (LCD.Touch(&x, &y));

    switch (uiState) {
        case Menu:
            if (x < 70 && y > 164) {
                uiState = UIState::LookingAtIOVars;
            } else if (x < 142 && y > 164) {
                uiState = UIState::LookingAtIOFuncs;
            }
            break;

        case LookingAtIOVars:
            if (x < 30 && y < 30) {
                uiState = UIState::Menu;
            }
            for (int i = 0; i < currentIOVars; i++) {
                if (y > (float)(37 + 24*i) && y < (float)(61 + 24*i)) {
                    selectedIOVar = i;
                    uiState = UIState::AccessingIOVars;
                }
            }
            break;

        case LookingAtIOFuncs:
            if (x < 30 && y < 30) {
                uiState = UIState::Menu;
            }
            for (int i = 0; i < currentIOFuncs; i++) {
                if (y > (float)(37 + 24*i) && y < (float)(61 + 24*i)) {
                    selectedIOFunc = i;
                    uiState = UIState::AccessingIOFuncs;
                }
            }
            break;

        case AccessingIOVars:
            if (x < 30 && y < 30) {
                uiState = UIState::LookingAtIOVars;
            }
            if (x > 120 && x < 200 && y > 200) {
                editVariable();
            }
            break;

        case AccessingIOFuncs: case RanFunc:
            if (x < 30 && y < 30) {
                uiState = UIState::LookingAtIOFuncs;
            }
            if (y > 140) {
                if (x < 160) {
                    LCD.SetFontColor(BACKGROUND_COLOR);
                    LCD.FillRectangle(16, 40, 200, 48);
                    LCD.SetFontColor(FOREGROUND_COLOR);
                    LCD.WriteAt("Running...", 16, 40);
                    ioFuncs[selectedIOFunc]();
                    LCD.WriteAt("Completed.", 16, 64);
                    uiState = UIState::RanFunc;
                } else {
                    startDebugger();
                }
            }
            break;

    }
}


void editVariable() {
    char text[BUFFER_SIZE + 1];
    if (ioVarIsFP[selectedIOVar]) {
        sprintf(text, "%.3f", *((float*) ioVars[selectedIOVar]));
    } else {
        sprintf(text, "%i", *((int*) ioVars[selectedIOVar]));
    }
    size_t cursorPos = strlen(text);

    // draw buttons
    LCD.SetFontColor(BACKGROUND_COLOR);
    LCD.FillRectangle(120, 200, 80, 40);
    LCD.SetFontColor(FOREGROUND_COLOR);
    LCD.WriteAt("7 8 9   <x  ", 88, 132);
    LCD.WriteAt("4 5 6       ", 88, 156);
    LCD.WriteAt("1 2 3  <   >", 88, 180);
    LCD.WriteAt("- 0 .  Enter", 88, 204);



    while (true) {
        LCD.SetFontColor(BACKGROUND_COLOR);
        LCD.FillRectangle(0, 96, 320, 20);
        LCD.SetFontColor(FOREGROUND_COLOR);
        LCD.WriteAt(text, 4, 96);
        LCD.WriteAt("_", 4 + 12*cursorPos, 100);

        // wait for input
        float x, y;
        while (!LCD.Touch(&x, &y));
        while (LCD.Touch(&x, &y));

        // process input
        if (x > 82 && x < 154 && y > 129 && y < 225) {
            // Numbers/Symbols
            memmove(text+cursorPos+1, text+cursorPos, strlen(text+cursorPos) + 1);
            int bx = (int)((x - 82) / 24);
            int by = (int)((y - 129) / 24);
            if (by == 3) {
                text[cursorPos] = (bx == 0 ? '-' : (bx == 1 ? '0' : '.'));
            } else {
                text[cursorPos] = (char)('1' + 3*(2-by) + bx);
            }
            if (cursorPos < BUFFER_SIZE-1) cursorPos++;
        } else if (x > 166 && x < 190 && y > 177 && y < 201) {
            // <
            if (cursorPos > 0) cursorPos--;
        } else if (x > 214 && x < 238 && y > 177 && y < 201) {
            // >
            if (cursorPos < BUFFER_SIZE-1 && cursorPos < strlen(text)) cursorPos++;
        } else if (x > 178 && x < 214 && y > 129 && y < 153) {
            // <x
            if (cursorPos > 0) {
                cursorPos--;
                memmove(text+cursorPos, text+cursorPos+1, strlen(text+cursorPos+1) + 1);
            }
        } else if (x > 166 && x < 238 && y > 201 && y < 225) {
            // Enter
            if (ioVarIsFP[selectedIOVar]) {
                float output = 0;
                int success = sscanf(text, "%f", &output);
                if (success) {
                    *((float*) ioVars[selectedIOVar]) = output;
                    return;
                } else {
                    LCD.WriteAt("Parse error", 160, 80);
                    while (!LCD.Touch(&x, &y));
                    LCD.SetFontColor(BACKGROUND_COLOR);
                    LCD.WriteAt("Parse error", 160, 80);
                    LCD.SetFontColor(FOREGROUND_COLOR);
                }
            } else {
                int output = 0;
                int success = sscanf(text, "%i", &output);
                if (success) {
                    *((int*) ioVars[selectedIOVar]) = output;
                    return;
                }
            }

        } else if (x < 30 && y < 30) {
            return;
        }
    }
}


void startDebugger() {
    functionAborted = false;
    inDebugger = true;

    clearDebugLog();

    LCD.Clear(DEBUGGER_BACKGROUND_COLOR);
    LCD.SetFontColor(DEBUGGER_FOREGROUND_COLOR);
    LCD.WriteRC("Abort", 13, 21);
    printLineF(0, "Debugger: %s", ioFuncNames[selectedIOFunc]);
    printLineF(12, "Touch to run function.");

    float x, y;
    while (!LCD.Touch(&x, &y));
    while (LCD.Touch(&x, &y));

    printLineF(12, "");

    if (x > 240 && y > 200) {
        functionAborted = true;
    } else {
        try {
            abortCheck();
            ioFuncs[selectedIOFunc]();
            printLineF(12, "Completed. Touch to close.");
        } catch (AbortException* e) {
            printLineF(12, "Aborted. Touch to close.");
            delete e;
        }
    }

    while (LCD.Touch(&x, &y));
    while (!LCD.Touch(&x, &y));
    while (LCD.Touch(&x, &y));
    inDebugger = false;
}


void printLineF(int row, const char* format, ...) {
    if (!inDebugger || row < 0 || row >= HEIGHT_CHARS) return;
    va_list valist;
    va_start(valist, format);
    vsnprintf(debuggerText[row], WIDTH_CHARS, format, valist);
    LCD.SetFontColor(DEBUGGER_BACKGROUND_COLOR);
    LCD.FillRectangle(0, row*17, 320, 17);
    LCD.SetFontColor(DEBUGGER_FOREGROUND_COLOR);
    LCD.WriteRC(debuggerText[row], row, 0);
    va_end(valist);
}

void printAppendF(int row, const char* format, ...) {
    if (!inDebugger || row < 0 || row >= HEIGHT_CHARS) return;
    va_list valist;
    va_start(valist, format);
    int currentLength = strlen(debuggerText[row]);
    vsnprintf(debuggerText[row] + currentLength, WIDTH_CHARS - currentLength, format, valist);
    LCD.WriteRC(debuggerText[row], row, 0);
    va_end(valist);
}

int printNextLineF(const char* format, ...) {
    if (!inDebugger) return -1;
    va_list valist;
    va_start(valist, format);
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

void clearDebugLog() {
    for (int i = 0; i < 13; i++) {
        printLineF(i, "");
    }
}

void breakpoint() {
    if (!inDebugger) return;
    float x, y;
    printLineF(12, "Touch to continue.");

    while (!LCD.Touch(&x, &y));
    while (LCD.Touch(&x, &y));

    printLineF(12, "");

    if (x > 240 && y > 200) {
        throw new AbortException();
    }
}

bool breakpoint(float timeout) {
    if (!inDebugger) return false;
    float x, y;
    double targetTime = TimeNow() + timeout;
    printLineF(12, "Touch or wait to continue.");

    while (!LCD.Touch(&x, &y) && TimeNow() < targetTime);
    while (LCD.Touch(&x, &y) && TimeNow() < targetTime);

    printLineF(12, "");

    bool timedOut = TimeNow() >= targetTime;

    if (!timedOut && x > 240 && y > 200) {
        throw new AbortException();
    }

    return timedOut;
}


void abortCheck() {
    if (!inDebugger) return;
    float x, y;
    if (LCD.Touch(&x, &y) && x > 240 && y > 200) {
        throw new AbortException();
    }
}

void sleepWithAbortCheck(float time) {
    double targetTime = TimeNow() + time;
    while (TimeNow() < targetTime) {
        abortCheck();
    }
}

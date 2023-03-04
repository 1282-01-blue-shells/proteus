#include "proteos.hpp"

#include "string.h"
#include "stdio.h"

#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHBattery.h"


// Static variable definitions

int ProteOS::backgroundColor = 0x5040A0;
int ProteOS::foregroundColor = 0xD0C0FF;
ProteOS::UIState ProteOS::uiState = UIState::Menu;
int ProteOS::currentVars = 0;
int ProteOS::currentFuncs = 0;

// Function definitions

void ProteOS::registerVariable(const char* variableName, int* varPtr) {
    if (currentVars >= MAX_VARIABLES) return;
    varNames[currentVars] = variableName;
    varTypes[currentVars] = VariableType::Int;
    varPtrs[currentVars] = varPtr;
    currentVars++;
}

void ProteOS::registerVariable(const char* variableName, float* varPtr) {
    if (currentVars >= MAX_VARIABLES) return;
    varNames[currentVars] = variableName;
    varTypes[currentVars] = VariableType::Float;
    varPtrs[currentVars] = varPtr;
    currentVars++;
}

void ProteOS::registerVariable(const char* variableName, bool* varPtr) {
    if (currentVars >= MAX_VARIABLES) return;
    varNames[currentVars] = variableName;
    varTypes[currentVars] = VariableType::Bool;
    varPtrs[currentVars] = varPtr;
    currentVars++;
}

void ProteOS::registerFunction(const char* functionName, void (*funcPtr)()) {
    if (currentFuncs >= MAX_FUNCTIONS) return;
    funcNames[currentFuncs] = functionName;
    funcPtrs[currentFuncs] = funcPtr;
    currentFuncs++;
}

void ProteOS::run() {
    while (true) {
        drawScreen();
        waitForInput();
    }
}

void ProteOS::drawScreen() {
    LCD.Clear(backgroundColor);
    LCD.SetFontColor(foregroundColor);

    char buf[BUFFER_SIZE + 1];

    LCD.WriteAt("--------------------------", 4, 16);

    LCD.WriteAt(Battery.Voltage(), 232, 4);
    LCD.WriteAt("V", 304, 4);

    switch (uiState) {
        case UIState::Menu:
            LCD.WriteAt("Menu", 40, 4);
            LCD.WriteAt("ProteOS", 118, 112);
            LCD.WriteAt("Vars", 10, 220);
            drawFolderIcon(4, 164);
            LCD.WriteAt("Funcs", 76, 220);
            drawFolderIcon(76, 164);
            break;

        case UIState::LookingAtVars:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt("Variables", 40, 4);
            for (int i = 0; i < currentVars; i++) {
                LCD.WriteAt(varNames[i], 16, 40 + 24*i);
                switch (varTypes[i]) {
                    case Bool:
                        snprintf(buf, BUFFER_SIZE, "%s", *((bool*) varPtrs[i]) ? "true" : "false");
                        break;
                    case Int:
                        snprintf(buf, BUFFER_SIZE, "%i", *((int*) varPtrs[i]));
                        break;
                    case Float:
                        snprintf(buf, BUFFER_SIZE, "%.3f", *((float*) varPtrs[i]));
                }

                // Cut it off if it's too long
                if (strlen(buf) > 8) {
                    strcpy(buf+5, "...");
                }
                LCD.WriteAt(buf, 292 - 12*strlen(buf), 40 + 24*i);
            }
            break;
        case UIState::LookingAtFuncs:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt("Functions", 40, 4);
            for (int i = 0; i < currentFuncs; i++) {
                LCD.WriteAt(funcNames[i], 16, 40 + 24*i);
            }
            break;

        case UIState::AccessingVar:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt(varNames[selectedVar], 40, 4);

            switch (varTypes[selectedVar]) {
                case Bool:
                    LCD.WriteAt("Type: bool", 16, 40);
                    break;
                case Int:
                    LCD.WriteAt("Type: int", 16, 40);
                    break;
                case Float:
                    LCD.WriteAt("Type: float", 16, 40);
                    break;
            }

            switch (varTypes[selectedVar]) {
                case Bool:
                    snprintf(buf, BUFFER_SIZE, "Value: %s", *((bool*) varPtrs[selectedVar]) ? "true" : "false");
                    break;
                case Int:
                    snprintf(buf, BUFFER_SIZE, "Value: %i", *((int*) varPtrs[selectedVar]));
                    break;
                case Float:
                    snprintf(buf, BUFFER_SIZE, "Value: %f", *((float*) varPtrs[selectedVar]));
            }
            LCD.WriteAt(buf, 16, 64);

            LCD.DrawRectangle(120, 200, 80, 32);
            LCD.WriteAt("Edit", 136, 208);
            break;

        case UIState::AccessingFunc:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt(funcNames[selectedFunc], 40, 4);
            //LCD.DrawRectangle(20, 160, 120, 60);
            LCD.DrawRectangle(180, 160, 120, 60);
            //LCD.WriteAt("Run", 62, 182);
            LCD.WriteAt("Debug", 210, 182);
            break;
    }

}

void ProteOS::drawFolderIcon(int x, int y) {
    LCD.DrawHorizontalLine(y+10, x+10, x+25);
    LCD.DrawHorizontalLine(y+15, x+25, x+50);
    LCD.DrawHorizontalLine(y+40, x+10, x+50);
    LCD.DrawVerticalLine(x+10, y+10, y+40);
    LCD.DrawVerticalLine(x+50, y+15, y+40);
    LCD.DrawVerticalLine(x+25, y+10, y+15);
}

void ProteOS::waitUntilPressAndRelease(float* x, float* y) {
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

void ProteOS::waitForInput() {
    float x, y;
    waitUntilPressAndRelease(&x, &y);

    switch (uiState) {
        case Menu:
            if (x < 70 && y > 164) {
                uiState = UIState::LookingAtVars;
            } else if (x < 142 && y > 164) {
                uiState = UIState::LookingAtFuncs;
            }
            break;

        case LookingAtVars:
            if (x < 30 && y < 30) {
                uiState = UIState::Menu;
            }
            for (int i = 0; i < currentVars; i++) {
                if (y > (float)(37 + 24*i) && y < (float)(61 + 24*i)) {
                    selectedVar = i;
                    uiState = UIState::AccessingVar;
                }
            }
            break;

        case LookingAtFuncs:
            if (x < 30 && y < 30) {
                uiState = UIState::Menu;
            }
            for (int i = 0; i < currentFuncs; i++) {
                if (y > (float)(37 + 24*i) && y < (float)(61 + 24*i)) {
                    selectedFunc = i;
                    uiState = UIState::AccessingFunc;
                }
            }
            break;

        case AccessingVar:
            if (x < 30 && y < 30) {
                uiState = UIState::LookingAtVars;
            }
            if (x > 120 && x < 200 && y > 200) {
                editVariable();
            }
            break;

        case AccessingFunc:
            if (x < 30 && y < 30) {
                uiState = UIState::LookingAtFuncs;
            }
            if (y > 140 && x > 160) {
                Debugger::debugFunction(funcNames[selectedFunc], funcPtrs[selectedFunc]);
            }
            break;

    }
}


void ProteOS::editVariable() {
    char text[BUFFER_SIZE + 1];
    switch (varTypes[selectedVar]) {
        case Bool:
            snprintf(text, BUFFER_SIZE, "%i", *((char*) varPtrs[selectedVar]));
            break;
        case Int:
            snprintf(text, BUFFER_SIZE, "%i", *((int*) varPtrs[selectedVar]));
            break;
        case Float:
            snprintf(text, BUFFER_SIZE, "%.3f", *((float*) varPtrs[selectedVar]));
    }
    size_t cursorPos = strlen(text);

    // draw buttons
    LCD.SetFontColor(backgroundColor);
    LCD.FillRectangle(120, 200, 80, 40);
    LCD.SetFontColor(foregroundColor);
    LCD.WriteAt("7 8 9   <x  ", 88, 132);
    LCD.WriteAt("4 5 6       ", 88, 156);
    LCD.WriteAt("1 2 3  <   >", 88, 180);
    LCD.WriteAt("- 0 .  Enter", 88, 204);



    while (true) {
        LCD.SetFontColor(backgroundColor);
        LCD.FillRectangle(0, 96, 320, 20);
        LCD.SetFontColor(foregroundColor);
        LCD.WriteAt(text, 4, 96);
        LCD.WriteAt("_", 4 + 12*cursorPos, 100);

        // wait for input
        float x, y;
        waitUntilPressAndRelease(&x, &y);

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

            switch (varTypes[selectedVar]) {
                case Bool:
                    int output = 0;
                    int success = sscanf(text, "%i", &output);
                    if (success) {
                        *((bool*) varPtrs[selectedVar]) = (bool) output;
                        return;
                    }
                    break;
                case Int:
                    int output = 0;
                    int success = sscanf(text, "%i", &output);
                    if (success) {
                        *((int*) varPtrs[selectedVar]) = output;
                        return;
                    }
                    break;
                case Float:
                    float output = 0;
                    int success = sscanf(text, "%f", &output);
                    if (success) {
                        *((float*) varPtrs[selectedVar]) = output;
                        return;
                    } else {
                        LCD.WriteAt("Parse error", 160, 80);
                        while (!LCD.Touch(&x, &y));
                        LCD.SetFontColor(backgroundColor);
                        LCD.WriteAt("Parse error", 160, 80);
                        LCD.SetFontColor(foregroundColor);
                    }
                    break;
            }
            
        } else if (x < 30 && y < 30) {
            return;
        }
    }
}



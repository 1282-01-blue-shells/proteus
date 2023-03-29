#include "proteos.hpp"

#include "debugger.hpp"

#include "string.h"
#include "stdio.h"

#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHBattery.h"
#include "FEHRPS.h"


// Static variable definitions

int ProteOS::backgroundColor = 0x5040A0;
int ProteOS::foregroundColor = 0xD0C0FF;
ProteOS::UIState ProteOS::uiState = UIState::Menu;
int ProteOS::currentVars = 0;
int ProteOS::currentFuncs = 0;
int ProteOS::selectedVar = 0;
int ProteOS::selectedFunc = 0;

const char* ProteOS::varNames[MAX_VARIABLES] = {0};
ProteOS::VariableType ProteOS::varTypes[MAX_VARIABLES];
void* ProteOS::varPtrs[MAX_VARIABLES] = {0};

const char* ProteOS::funcNames[MAX_FUNCTIONS] = {0};
void (*ProteOS::funcPtrs[MAX_FUNCTIONS])() = {0};

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
            LCD.WriteAt("RPS", 160, 220);
            drawRPSIcon(148, 164);
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
            LCD.DrawRectangle(100, 160, 120, 60);
            //LCD.WriteAt("Run", 62, 182);
            LCD.WriteAt("Debug", 130, 182);
            break;

        case UIState::UsingRPSNotConnected:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt("RPS", 40, 4);
            LCD.WriteAt("RPS not connected.", 16, 40);
            LCD.DrawRectangle(100, 160, 120, 60);
            LCD.WriteAt("Connect", 118, 182);
            break;
        
        case UIState::UsingRPSConnected:
            LCD.WriteAt("<", 4, 4);
            LCD.WriteAt("RPS", 40, 4);
            LCD.WriteAt("RPS is connected.", 16, 40);
            LCD.WriteAt("Current Region: ", 16, 64);
            LCD.WriteAt(RPS.CurrentRegion(), 208, 64);
            LCD.WriteAt("Time left: ", 16, 88);
            LCD.WriteAt(RPS.Time(), 148, 88);
            LCD.WriteAt("X: ", 16, 112);
            LCD.WriteAt(RPS.X(), 52, 112);
            LCD.WriteAt("Y: ", 16, 136);
            LCD.WriteAt(RPS.Y(), 52, 136);
            LCD.WriteAt("Heading: ", 16, 160);
            LCD.WriteAt(RPS.Heading(), 124, 160);
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

void ProteOS::drawRPSIcon(int x, int y) {
    LCD.DrawCircle(x+30, y+25, 15);
    LCD.DrawHorizontalLine(y+25, x+15, x+45);
    LCD.DrawHorizontalLine(y+18, x+17, x+43);
    LCD.DrawHorizontalLine(y+32, x+17, x+43);
    LCD.DrawVerticalLine(x+30, y+10, y+40);
    
    LCD.DrawLine(x+21, y+25, x+23, y+18);
    LCD.DrawLine(x+21, y+25, x+23, y+32);
    LCD.DrawLine(x+23, y+18, x+30, y+10);
    LCD.DrawLine(x+23, y+32, x+30, y+40);

    LCD.DrawLine(x+39, y+25, x+37, y+18);
    LCD.DrawLine(x+39, y+25, x+37, y+32);
    LCD.DrawLine(x+37, y+18, x+30, y+10);
    LCD.DrawLine(x+37, y+32, x+30, y+40);

}

void ProteOS::waitForInput() {
    float x, y;
    Debugger::waitUntilPressAndRelease(&x, &y);

    switch (uiState) {
        case Menu:
            if (x < 70 && y > 164) {
                uiState = UIState::LookingAtVars;
            } else if (x < 142 && y > 164) {
                uiState = UIState::LookingAtFuncs;
            } else if (x < 214 && y > 164) {
                if (RPS.CurrentRegion() >= 0) {
                    uiState = UIState::UsingRPSConnected;
                } else {
                    uiState = UIState::UsingRPSNotConnected;
                }
                
            }
            break;

        case LookingAtVars:
            if (x < 30 && y < 30) {
                uiState = UIState::Menu;
                break;
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
                break;
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
                break;
            }
            if (x > 120 && x < 200 && y > 200) {
                editVariable();
            }
            break;

        case AccessingFunc:
            if (x < 30 && y < 30) {
                uiState = UIState::LookingAtFuncs;
                break;
            }
            if (y > 140 && x > 80 && x < 240) {
                Debugger::debugFunction(funcNames[selectedFunc], funcPtrs[selectedFunc]);
            }
            break;

        case UsingRPSNotConnected:
            if (x < 30 && y < 30) {
                uiState = UIState::Menu;
                break;
            }
            if (y > 140 && x > 80 && x < 240) {
                RPS.InitializeTouchMenu();
                Sleep(2.0);
                uiState = UIState::UsingRPSConnected;
            }
            break;

        case UsingRPSConnected:
            if (x < 30 && y < 30) {
                uiState = UIState::Menu;
                break;
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
        Debugger::waitUntilPressAndRelease(&x, &y);

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
            int outputI;
            float outputF;
            int success;
            switch (varTypes[selectedVar]) {
                case Bool:
                    outputI = 0;
                    success = sscanf(text, "%i", &outputI);
                    if (success) {
                        *((bool*) varPtrs[selectedVar]) = (bool) outputI;
                        return;
                    }
                    break;
                case Int:
                    outputI = 0;
                    success = sscanf(text, "%i", &outputI);
                    if (success) {
                        *((int*) varPtrs[selectedVar]) = outputI;
                        return;
                    }
                    break;
                case Float:
                    outputF = 0;
                    success = sscanf(text, "%f", &outputF);
                    if (success) {
                        *((float*) varPtrs[selectedVar]) = outputF;
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


//#include "debugger.cpp"
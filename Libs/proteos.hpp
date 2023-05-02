#ifndef PROTEOS_HPP
#define PROTEOS_HPP

#include "debugger.hpp"


#define MAX_VARIABLES 8
#define MAX_FUNCTIONS 8


class ProteOS {
public:

    // Default: 0x5040A0
    static int backgroundColor;
    // Default: 0xD0C0FF
    static int foregroundColor;


    // Registers a variable so that it can be read and changed from the menu.
    // Currently supported variable types: int, float, and bool
    static void registerVariable(const char* variableName, int* varPtr);
    static void registerVariable(const char* variableName, float* varPtr);
    static void registerVariable(const char* variableName, bool* varPtr);

    // Registers a function so that it can be called from the menu. Function must have no 
    //   arguments and return nothing.
    static void registerFunction(const char* functionName, void (*funcPtr)());

    // Opens the menu to allow the user to access variables and functions.
    static void run();


private:

    enum UIState {
        Menu,
        LookingAtVars,
        LookingAtFuncs,
        AccessingVar,
        AccessingFunc,
        UsingRPSNotConnected,
        UsingRPSConnected
    };
    static UIState uiState;

    enum VariableType {
        Bool,
        Int,
        Float
    };

    static const char* varNames[MAX_VARIABLES];
    static VariableType varTypes[MAX_VARIABLES];
    static void* varPtrs[MAX_VARIABLES];

    static const char* funcNames[MAX_FUNCTIONS];
    static void (*funcPtrs[MAX_FUNCTIONS])();

    static int currentVars;
    static int currentFuncs;

    static int selectedVar;
    static int selectedFunc;


    static void drawScreen();
    static void waitForInput();
    static void editVariable();
    static void drawFolderIcon(int x, int y);
    static void drawRPSIcon(int x, int y);
};


#endif

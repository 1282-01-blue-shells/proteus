#ifndef PROTEOS_HPP
#define PROTEOS_HPP


#define MAX_IO_VARIABLES 8
#define MAX_IO_FUNCTIONS 8


// IO functions

void registerIOVariable(const char* variableName, int* varPtr);
void registerIOVariable(const char* variableName, float* varPtr);
void registerIOFunction(const char* functionName, void (*funcPtr)());

void openIOMenu();


// Debugger functions

// Writes text on the specified line, overwriting any text that was previously there.
void printLineF(int row, const char* format, ...);

// Appends text at the end of the specified line.
void printAppendF(int row, const char* format, ...);

// Writes the specified text on the first empty line, and returns the line number that was written to.
int printNextLineF(const char* format, ...);

// Erases everything that has been printed to the screen.
void clearDebugLog();

// Prompts the user to press the screen to continue, and pauses execution until they do so.
void breakpoint();

// Pauses execution until the screen is tapped, or until a certain amount of time has passed.
// Returns true if the breakpoint timed out.
bool breakpoint(float timeout);

// If the function you want to debug has a busy loop, call this function every loop to enable use
//   of the Abort button.
void abortCheck();

// If the function you want to debug must wait for an amount of time, use this instead of Sleep()
//   to enable use of the Abort button.
void sleepWithAbortCheck(float time);

// Waits until the screen is pressed and released, and outputs the last position before the screen
//   was released. x and y can be null if you do not need the position.
void waitUntilPressAndRelease(float* x, float* y);


#endif

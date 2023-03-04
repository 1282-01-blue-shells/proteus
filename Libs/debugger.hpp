#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "exception"


#define WIDTH_CHARS 26
#define HEIGHT_CHARS 13

#define BUFFER_SIZE 30


#define assertTrue(condition, message)  if (!(condition)) { throw new AssertionException(__func__, __LINE__, message); }


struct AbortException : public std::exception {};

struct AssertionException : public std::exception {
    const char* functionName;
    int lineNumber;
    const char* message;
    AssertionException(const char* functionName, int lineNumber, const char* message);
};


class Debugger {
public:

    // Members //

    // Default: 0x000000
    static int backgroundColor;
    // Default: 0x00E0F0
    static int defaultFontColor;
    // Default: 0xFF0000
    static int errorColor;



    // Functions //

    // Writes text on the specified line, overwriting any text that was previously there.
    static void printLine(int row, const char* format, ...);

    // Appends text at the end of the specified line.
    static void printAppend(int row, const char* format, ...);

    // Writes the specified text on the first empty line, and returns the line number that was written to.
    static int printNextLine(const char* format, ...);

    // Writes text on the specified line, with any overflow wrapping to the next lines.
    static void printWrap(int startRow, const char* format, ...);

    // Change the font color used by the debugger
    static void setFontColor(int color);

    // Reset the font color used by the debugger
    static void setFontColor();

    // Erases everything that has been printed to the screen.
    static void clear();

    // Prompts the user to press the screen to continue, and pauses execution until they do so.
    static void breakpoint();

    // Pauses execution until the screen is tapped, or until a certain amount of time has passed.
    // Returns true if the breakpoint timed out.
    static bool breakpoint(float timeout);

    // If the function you want to debug has a busy loop, call this function every loop to enable use
    //   of the Abort button.
    static void abortCheck();

    // If the function you want to debug must wait for an amount of time, use this instead of 
    //   FEHUtility's Sleep() to enable use of the Abort button.
    static void sleep(float time);

    // Waits until the screen is pressed and released, and outputs the last position before the screen
    //   was released. x and y can be null if you do not need the position.
    static void waitUntilPressAndRelease(float* x, float* y);

    // Runs a function in the debugger. Used internally
    static void debugFunction(const char* functionName, void (*funcPtr)());


private:
    static bool inDebugger;
    static char debuggerText[HEIGHT_CHARS][WIDTH_CHARS + 1];

    static int debuggerFontColor;
};
#endif

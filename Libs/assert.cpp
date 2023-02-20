#include <FEHBuzzer.h>
#include <FEHLCD.h>

[[noreturn]] void _Assert(
    const char *file,
    const int line,
    const char *fn,
    const char *cond
) {
    LCD.Clear(RED);
    LCD.WriteLine("Assertion failed at");

    LCD.Write(' ');
    LCD.Write(file);
    LCD.Write(':');
    LCD.Write(line);
    LCD.Write(" in ");
    LCD.Write(fn);
    LCD.Write("():");
    // Leave the third row blank and put the condition in the fourth row with a left-indentation of
    // one.
    LCD.WriteRC(cond, 3, 1);

    FEHBuzzer().Beep();
    // Spin.
    while (true);
}

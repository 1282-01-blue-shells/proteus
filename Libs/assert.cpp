#include <FEHBuzzer.h>
#include <FEHLCD.h>

[[noreturn]] void _Assert(
    const char *file,
    const int line,
    const char *fn,
    const char *cond
) {
    LCD.Clear(RED);
    LCD.SetFontColor(LCD.White);
    LCD.WriteLine("Assertion failed at");

    LCD.Write(' ');
    LCD.Write(file);
    LCD.WriteLine(',');

    LCD.Write(" line ");
    LCD.Write(line);
    LCD.WriteLine(',');

    LCD.Write(" in ");
    LCD.Write(fn);
    LCD.WriteLine("():");
    LCD.WriteLine("");

    LCD.WriteLine("condition");
    LCD.WriteLine("");
    LCD.SetFontColor(LCD.Gray);
    LCD.WriteLine(cond);

    LCD.SetFontColor(LCD.White);
    LCD.WriteRC("is false.", 13, 9);

    FEHBuzzer().Beep();
    // Spin.
    while (true);
}

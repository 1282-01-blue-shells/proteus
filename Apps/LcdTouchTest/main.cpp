//! \file
//! \brief An application to display the X and Y coordinates of an LCD touch.
//! \author Will Blankemeyer

#include <cstdio>

#include <assert.hpp>
#include <FEHLCD.h>

void drawCoords(const int x, const int y) {
    assert(x >= 0);
    assert(y >= 0);

    LCD.Clear();
    LCD.DrawPixel(x, y);

    // The parentheses and comma are three characters, the coordinates make up three characters each
    // (for a total of six), and we need an extra byte for the NUL terminator.
    char buf[10];
    snprintf(buf, sizeof(buf), "(%3hu,%3hu)", x, y);
    LCD.WriteAt(buf, x, y);
}

int main(void) {
    LCD.Clear();

    int x, y;
    // Wait until the LCD is touched.
    while (!LCD.Touch(&x, &y));
    // The LCD is being touched now.
    drawCoords(x, y);

    while (true) {
        if (LCD.Touch(&x, &y)) {
            drawCoords(x, y);
        }
    }
}

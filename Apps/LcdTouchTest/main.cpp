//! An application to display the X and Y coordinates of an LCD touch.
//!
//! \author Will Blankemeyer

#include <cstdio>

#include <FEHLCD.h>

void draw_coords(const float x, const float y) {
    LCD.Clear();

    const int ix = static_cast<int>(x);
    const int iy = static_cast<int>(y);

    // The parentheses and comma are three characters, the coordinates make up three characters each
    // (for a total of six), and we need an extra byte for the NUL terminator.
    char buf[10];
    snprintf(buf, sizeof(buf), "(%3u,%3u)", ix, iy);
    LCD.WriteAt(buf, ix, iy);
}

int main(void) {
    LCD.Clear();

    float x, y;
    // Wait until the LCD is touched.
    while (!LCD.Touch(&x, &y));
    // The LCD is being touched now.
    draw_coords(x, y);

    while (true) {
        if (LCD.Touch(&x, &y)) {
            draw_coords(x, y);
        }
    }
}

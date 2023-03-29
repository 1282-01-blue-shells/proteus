#include <cstdio>

#include <FEHLCD.h>
#include <FEHServo.h>

FEHServo r2d2Servo(FEHServo::Servo1);

int main() {
    unsigned int angle = 90;

    r2d2Servo.SetMin(500);
    r2d2Servo.SetMax(2315);
    r2d2Servo.SetDegree((float)angle);

    LCD.SetFontColor(RED);
    LCD.DrawRectangle(0, 0, 100, 100);
    LCD.SetFontColor(BLUE);
    LCD.DrawRectangle(100, 0, 100, 100);
    
    while (true) {
        float x, _y;
        if (LCD.Touch(&x, &_y)) {
            if (x > 100.f) {
                if (angle <= 180) {
                    angle++;
                }
            } else {
                if (angle > 0) {
                    angle--;
                }
            }
            r2d2Servo.SetDegree((float)angle);
            
            char buf[20];
            snprintf(buf, sizeof(buf), "deg: %u", angle);

            LCD.WriteAt(buf, 0, 0);
        }
    }
}
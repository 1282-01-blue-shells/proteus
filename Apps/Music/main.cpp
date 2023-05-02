#include "proteos.hpp"

#include "FEHBuzzer.h"
#include "FEHUtility.h"
#include "FEHLCD.h"
#include "uart.h"


void note( int frequency, int duration )//tone at user defined frequency and user defined integer duration (milliseconds)
{
    //to get low byte, typecase to unsigned char
    unsigned char frequency_low = ( unsigned char ) ( frequency & 0xFF );
    //to get high byte, right shift by eight and then cast
    unsigned char frequency_high = ( unsigned char ) ( ( frequency >> 8 ) & 0xFF );
    //to get low byte, typecase to unsigned char
    unsigned char duration_low = ( unsigned char ) ( duration & 0xFF );
    //to get high byte, right shift by eight and then cast
    unsigned char duration_high = ( unsigned char ) ( ( duration >> 8 ) & 0xFF );

    uart_putchar( UART5_BASE_PTR, 0x7F ); // start byte to propeller
    uart_putchar( UART5_BASE_PTR, 0x0A ); // command to propeller to signal the buzzer
    uart_putchar( UART5_BASE_PTR, frequency_high );
    uart_putchar( UART5_BASE_PTR, frequency_low );
    uart_putchar( UART5_BASE_PTR, duration_high );
    uart_putchar( UART5_BASE_PTR, duration_low );
    uart_putchar( UART5_BASE_PTR, 0xFF );

    Sleep(duration);
}

void note(FEHBuzzer::stdnote frequency, float duration )
{
    note((int) frequency, (int)(duration*1000));
}

// Megalovania time for the win!
void tune1() {
    note(Buzzer.D4, 1/16.);
    Sleep(1/16.);
    note(Buzzer.D4, 1/16.);
    Sleep(1/16.);
    note(Buzzer.D5, 3/16.);
    Sleep(1/16.);
    note(Buzzer.A4, 1/4.);
}

void tune2() {
    note(Buzzer.C4, 1/4.);
    note(Buzzer.Ef4, 4/8.);
    note(Buzzer.F4, 4/8.);
    note(Buzzer.Fs4, 3/4.);
}

int main() {
    LCD.Clear();
    ProteOS::registerFunction("tune1()", &tune1);
    ProteOS::registerFunction("tune2()", &tune2);

    ProteOS::run();
}
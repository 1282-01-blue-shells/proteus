#include "proteos.hpp"
#include "navigation.hpp"
#include "FEHMotor.h"
#include "FEHUtility.h"
//Author AK Miedler

//Declaring Functions

//Variables
float bluethreshold=.9f; // need a range of .4 to .7 ?
float redthreshold=.3f;
float startingthreshold=2.0f;

AnalogInputPin lightSensor(FEHIO::P0_7);

float rampDistance = 34;
float lightDistance = 2;

void waitForLight();
void runcheckpoint2();
void gotocoloredlight();
void whatcolorisit();
void comehome();

int main() {
    ProteOS::registerVariable("redthreshold", &redthreshold);
    ProteOS::registerVariable("bluethreshold", &bluethreshold);
    ProteOS::registerVariable("rampDistance", &rampDistance);
    ProteOS::registerVariable("lightDistance", &lightDistance);

    ProteOS::registerFunction("runcheckpoint2()", &runcheckpoint2);
    ProteOS::registerFunction("waitForLight()",&waitForLight);
    ProteOS::registerFunction("gotocoloredlight()",&gotocoloredlight);
    ProteOS::registerFunction("whatcolorisit()",&whatcolorisit);
    ProteOS::registerFunction("comehome()",&comehome);
    
    ProteOS::run();

}

//running checkpoint
void runcheckpoint2()
{
    waitForLight();
    gotocoloredlight();
    whatcolorisit();
    comehome();
}

//function to keep bot stationary with no light and start moving when light turns on
void waitForLight() {
    Debugger::printLine(9, "Waiting for light...");

    // wait for light
    while (lightSensor.Value()>startingthreshold); // if no light do nothing


    // Change font color to yellow
    Debugger::setFontColor(0xFFFF00);

    // Print funny message
    Debugger::printWrap(10, "NOW AT LAST I SEEEE THE LIGHT");

    // Reset font color
    Debugger::setFontColor();
}

void gotocoloredlight(){
    Debugger::printNextLine("starting"); //positions to be in line with ramp (eventually want RPS here)
    Motors::drive(2);
    Motors::turn(-20);
    Motors::drive(2);
    Motors::turn(-20);

    Debugger::printNextLine("going forward"); //goes up ramp
    Motors::drive(rampDistance);
    Debugger::printNextLine("CLEARED THE RAMP");
    //Debugger::printNextLine("turning left"); //turns to be in line with light
    
    // turn to face left wall
    Motors::turn(-90);
    // drive up to wall
    Motors::drive(20);
    // drive one second, running into wall
    Motors::start(true);
    Debugger::sleep(1);
    Motors::stop();
    // move back to luggage drop-off
    Motors::drive(-8);
    // turn to face luggage
    Motors::turn(-90);
    // drive one second, running into luggage
    Motors::start(true);
    Debugger::sleep(1);
    Motors::stop();
    // drive up next to the light
    Motors::drive(-17);
    // turn again and back up into the light
    Motors::turn(-90);
    Motors::drive(-lightDistance);


    //Debugger::printNextLine("going forward"); //goes to be on top of light
    Motors::drive(20);

    Motors::stop();
    Debugger::sleep(2.f);
    Debugger::printNextLine("determining light color...");
}

void whatcolorisit(){
    //if light is red
    if (lightSensor.Value() < redthreshold){
        //backup into red button
        Debugger::printNextLine("LIGHT IS RED"); 
        Motors::drive(-3); //back up
        Motors::turn(50); //turn so parallelish with kiosk
        Motors::drive(2); //move forward a few inches
        Motors::turn(-40); //position in front of red button
        Motors::drive(6);//go until hit button
    }

    // if light is blue
    else if (lightSensor.Value() < bluethreshold){
        //backup into blue button
        Debugger::printNextLine("LIGHT IS BLUE");
        Motors::drive(-3); //back up 
        Motors::turn(30); //turn to face kiosk
        //Motors::turn(45); //position in front of blue button
        Motors::drive(-6); //go until hit button (once add microswitches it will be until they are pressed)
    }

    else{
        // For if neither work
        Debugger::printNextLine("I AM BLIND D:");
    }

}

void comehome(){
    //rps would be great here to get to same point moving on but whatever
    Debugger::printNextLine("GARRRYYY COME HOME");
    Motors::drive(-5); //move backward away from kiosk
    Motors::turn(-90); //rotate forwards
    Motors::drive(5); //move forward a little
    Motors::turn(-90); //turn left
    Motors::drive(3); //move forward a little
    Motors::turn(90); //turn right
    Motors::drive(15); //go down ramp
    Motors::stop(); //cut motors
    Debugger::printNextLine("GARRRYYYYY IS HOMEEEEE");
}
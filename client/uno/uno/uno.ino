
#include <Servo.h>
#include "Adafruit_APDS9960.h"

Servo motor; 

Adafruit_APDS9960 proximity;

int motorPos = 0;

char dummy = 123;

//motor difinition
const int doorOpen = 45;
const int doorClose = 0;


//PIN definition
const int buttonPin = 8;
const int motorPin = 5;
const int LEDPin = 3;
const int wifiInputPin = 7;
const int proximityPin = 2;


//State machine
enum programState {DOORCLOSEDWAITFORLIGHT,DOORCLOSEDIN, DOORCLOSEDOUT, DOOROPENIN, DOOROPENOUT};

programState state;


#define DEBUG(msg) Serial.println(msg);


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  //set up motor
  motor.attach(motorPin);
  motor.write(doorClose);

  //set up button
  pinMode(buttonPin, INPUT);

  //set up LED
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);

  //set up wifi Input
  pinMode(wifiInputPin, INPUT);
  digitalWrite(wifiInputPin, LOW);

  //set up proximity sensor
  pinMode(proximityPin, INPUT_PULLUP);

  if(!proximity.begin()){
    Serial.println("proximity init failed");
  }

  //enable proximity mode
  proximity.enableProximity(true);

  //set the interrupt threshold to fire when proximity reading goes above 175
  proximity.setProximityInterruptThreshold(0, 20);

  //enable the proximity interrupt
  proximity.enableProximityInterrupt();


  state = DOORCLOSEDOUT;
  DEBUG("DOOR CLOSED OUT");
}

void openDoor() {
   int pos;
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    motor.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
}

void closeDoor() {
   int pos;
  for (pos = 180; pos != 0; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    motor.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  switch(state) {
    case DOORCLOSEDOUT: {
      //scan for wifi input see if door open command has come
      //int remoteSwitch = digitalRead(wifiInputPin);
      if(digitalRead(wifiInputPin) == HIGH) {
        DEBUG("DOOR CLOSED OUT: remote pressed");
        //motor.write(doorOpen);
        openDoor();
        state = DOOROPENIN;
        DEBUG("DOOR OPEN waiting for car to get in");
      }
      break;
    }
    case DOOROPENIN: {
      //wait for car to get in
      if(!digitalRead(proximityPin) && proximity.readProximity() > 20) {
        DEBUG("DOOR OPEN: car in, turning on light and closing door");
        //car is in close door, turn on light
        digitalWrite(LEDPin, HIGH);
        closeDoor();
        state = DOORCLOSEDWAITFORLIGHT;
        proximity.clearInterrupt();
        DEBUG("DOOR CLOSED waiting for user to turn off light");
      }
      break;
    }
    case DOORCLOSEDWAITFORLIGHT: {
      //car is in, wait for light to turn off
      DEBUG("DOOR CLOSED waiting light off");
      if(digitalRead(buttonPin) == HIGH) {
        DEBUG("DOOR CLOSED light off");
        digitalWrite(LEDPin, LOW);
        state = DOORCLOSEDIN;
        //wait a bit to prevent double scan for the button on the other state
        unsigned long startTime = millis ();
        while (millis () - startTime < 2000)
          ;
        DEBUG("DOOR CLOSED waiting for user want to get out with car");        
        
      }
    }
    case DOORCLOSEDIN: {
      
      //see if button pressed or remote pressed see if the user want to get out
      if(digitalRead(buttonPin)==HIGH) {
        DEBUG("DOOR CLOSED user want to get out with car");
        digitalWrite(LEDPin, HIGH);
        openDoor();
        state = DOOROPENOUT;
        DEBUG("DOOR OPEN waiting for user to close the door");
      }
      break;
    }
    case DOOROPENOUT: {
      if(digitalRead(wifiInputPin) == HIGH) {
        DEBUG("DOOR OPEN user closed the door");
        digitalWrite(LEDPin, LOW);
        closeDoor();
        state = DOORCLOSEDOUT;
        unsigned long startTime = millis ();
        while (millis () - startTime < 2000)
          ;
        DEBUG("DOOR CLOSED OUT");
        
      }
      break;
    }
    default:
      
    break;
  }

}

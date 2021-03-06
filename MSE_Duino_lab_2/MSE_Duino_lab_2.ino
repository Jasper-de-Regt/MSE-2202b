//**************************************************************************************************************************************************************************************************************//
//**************************************************************************************************************************************************************************************************************//

//*********************  NOTES  **************************
//*********************  MSE Lab 2 ***********************
//*********************  Jasper de Regt ******************
//*********************  250822216 ***********************
//*********************  end of notes  **************************

//**************************************************************************************************************************************************************************************************************//
//**************************************************************************************************************************************************************************************************************//

// Global Variables

// to change number of leds, just update the first line of code
const unsigned int numberOfLeds = 6;                              // the number of leds

// these are the pin assignments. These assignments need to match the real world connections
const unsigned int ledPin[] = {4, 5, 6, 7, 8, 9};               // the microcontroller pin -> led connections from left to right. You will only use as many pins as you have number of LEDS
const unsigned int potentiometerPin = 14;                         // assigns an analog pin to the potentiometer. Note that pins above 13 are analog pins. So potentiometerPin-14=Apin. The dial type pot is pin P4 on J2
const unsigned int switchPin = 3;                                // Assigns a digital pin to the toggle switch. Toggle S1A is switch 1, Up is high, down is low
const unsigned int pushButtonPin = 2;                            // Assigns a digital pin to the pushbutton. Use PB1 on JP4
const unsigned int onBoardLED = 13;                               // might as well turn on the built in LED to show pushbutton state
const unsigned int servoPin = 12;                                  // assigns a digital pin to the servo

// Servo stuff
#include <Servo.h>                                                // includes the servo library to that its easy to command servo positions
unsigned int servoPos[numberOfLeds];                              // creates an array. It will be used to store angle values from 0-180.
Servo myServo;                                                     // creates a servo object of class servo

// LED blink stuff
bool ledState[numberOfLeds];                                      // an array of bools that describes the respective led state (true/false; on/off)
unsigned int stateCounter = 0;                                    // tracks which state occurence we are in. State corresponds to Exercise 1 chart in Lab Manual // numberOfPossibleStates = (numberOfLeds-1)*4
unsigned int ledInterval = 255;                           // the delay use for the led change loop
unsigned int buttonPotInterval = 255;                            // the delay as set by the buttons/pot
unsigned long previousMillis = 0;                                 // the LEDs were last updated at this millis(), if millis()-previousMillis>ledInterval then its time do update the LEDs again

// Button debounce stuff
bool previousButtonIsPressed = false;                     //stores the previous buttonIsPressed state. Stores what the button's state was after the last time there was a change in state
unsigned long previousButtonStateChangeTime = 0;        // stores the last millis() that the buttonIsPressed condition changed
unsigned int templedInterval;                             // Updates with how long you hold the pushbutton down for. When you let go (and if it is greater than debounce), it becomes the new ledInterval

// Serial stuff
unsigned long watchDog = 0;                         // tracks the last time that serial was received
// end of global variables

//**************************************************************************************************************************************************************************************************************//
//**************************************************************************************************************************************************************************************************************//

void setup() {
  // put your setup code here, to run once:

  // setup led pins stored in ledPin[] as output pins. This code works for any number of leds as long as there are enough pins stores in the ledPin array
  for (int i = 0; i < numberOfLeds; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
  // other pin assignments
  pinMode(potentiometerPin, INPUT);             // sets the potentiometer pin as an input pin
  pinMode(pushButtonPin, INPUT_PULLUP);        // Note that the pushbutton is a pull down button, so we need to pull up this input. INPUT_PULLUP activates the internal pullup resistor
  pinMode(switchPin, INPUT_PULLUP);            // INPUT_PULLUP is probably not needed since this switch switches between gnd and +5v. The only time the pullup has any effect is when the switch is moving (I assume its a break before make switch)
  pinMode(onBoardLED, OUTPUT);                 // sets the onboard led as an output pin. Because why not.
  digitalWrite(onBoardLED, HIGH);               // turns on the onboard led. The watchdog hasnt expired yet when we start the loop, so the led should be on
  myServo.attach(servoPin);                     // attaches the servo class object to an output pin

  Serial.begin(9600);        // pours a bowl of serial for troubleshooting and communications
  ledState[0] = true;        // set the first (far left) led state. We need to start somewhere in the blink pattern.

  // this function will populate the array of servoPos[] angles with angles from 0-180
  // The angles will be set in equal intervals. 4 LEDs/Positions would result in 0-60-120-180.
  for (int i = 0; i < numberOfLeds; i++) {
    servoPos[i] = i * (180 / (numberOfLeds - 1));
  }
}

//**************************************************************************************************************************************************************************************************************//
//**************************************************************************************************************************************************************************************************************//

void loop() {
  // put your main code here, to run repeatedly:

  // if serial is available, update the ledInterval based on the received value and turn on the LED
  if (Serial.available()) {
    ledInterval = Serial.read();      // set the ledInterval based on the serial value
    watchDog = millis();                      // resets the watchdog
    digitalWrite(onBoardLED, HIGH);           // if we are in receiving serial mode, turn on the on board led
  }
  // if the watchdog has expired, turn off the led13 and just set the ledInterval internally based on the buttonPotInterval
  else if ((millis() - watchDog) > 1000) {
    digitalWrite(onBoardLED, LOW);
    ledInterval = buttonPotInterval;
  }


  // this code is always sending out the buttonPotInterval if someone wants to listen; great. If not then its still ok.
  byte reading = buttonPotInterval;
  Serial.write(reading);


// these functions set the buttonPotDelay based on the sliding switch / Potentiometer / Pushbutton as described in the lab manual
  if (!digitalRead(switchPin)) {    // if the sliding switch is down, set ledInterval based on potentiometer
    buttonPotInterval = map(analogRead(potentiometerPin), 0, 1023, 0, 255); // reads the potentiometer as a value from 0-1023 and maps it to a value of 0-255, then sets that 0-255 value as ledInterval
  }
  else {    // else if the sliding switch is up, set ledInterval based on button press
    buttonPotInterval = debouncePushButtonandUpdateDelayBasedOnPress();
  }




  // if enough time has passed, update blinky lights stuff. recall that the lab asks for different time intervals
  // if stateCounter == even; time required is short = ledInterval            if stateCounter == odd; time required is longer = ledInterval + 1*2*ledInterval  (which = 3*ledInterval)
  if ((millis() - previousMillis) > (ledInterval + ((stateCounter % 2) * 2 * ledInterval)))  {
    // if at end of possible states, reset stateCounter
    if (stateCounter == ((numberOfLeds - 1) * 4)) {
      stateCounter = 0;
    }
    if (!(stateCounter % 2)) {                                        // if stateCounter even then we want to turn on only a single LED;
      if (stateCounter < ((numberOfLeds - 1) * 2)) {                        // if in the first half of possible states then the leds are moving left to right
        for (int i = 0; i < numberOfLeds; i++) {                                  // turns off all the LEDS
          ledState[i] = false;
        }
        ledState[stateCounter / 2] = true;                                        // turns on the single led that we want
        myServo.write(servoPos[stateCounter / 2]);                                //turns the servo towards the single LED that is on
      }
      else if (stateCounter >= ((numberOfLeds - 1) * 2)) {                  // else if second half of possible states then the leds are moving right to left
        for (int i = 0; i < numberOfLeds; i++) {                                  // turns off all the LEDS
          ledState[i] = false;
        }
        ledState[(((numberOfLeds - 1) * 4) - stateCounter) / 2] = true;           // turns on the single led that we want
        myServo.write(servoPos[(((numberOfLeds - 1) * 4) - stateCounter) / 2]);   //turns the servo towards the single LED that is on
      }
    }
    if (stateCounter % 2) {                                            // else if the statecounter is odd, then we want to turn on the LED that is either to the right or to the left of the LED that is already on.
      if (stateCounter < ((numberOfLeds - 1) * 2)) {                         // if in the first half of possible states then the leds are moving left to right
        for (int i = numberOfLeds; i >= 0; i--) {                            // so we will see which LED is on, then turn on the LED to the right of the LED that is already on
          if (ledState[i] == true) {
            ledState[i + 1] = true;
          }
        }
      }
      else if (stateCounter >= ((numberOfLeds - 1) * 2)) {                  // else if its in the second half of possible states then the LEDS are moving right to left
        for (int i = 0; i < numberOfLeds; i++) {                            // so we will see which LED is on, then turn on the LED to the left of the LED that is already on
          if (ledState[i] == true) {
            ledState[i - 1] = true;
          }
        }
      }
    }

    for (int i = 0; i < numberOfLeds; i++) {        // each LED has a respective bool. This function will turn the LED on or off depending on that bool
      digitalWrite(ledPin[i], ledState[i]);
    }

    previousMillis = millis();        // stores the last time that the LEDs were updated.
    stateCounter++;                   // increments stateCounter, so next time we will update the LEDs to the next state
  }
}
// end of loop

//**************************************************************************************************************************************************************************************************************//
//**************************************************************************************************************************************************************************************************************//

// start of functions

// this function polls the pushbutton, debounces it, and returns how long the button was held down. If the button wasn't held down long enough
// then the function will just return the current buttonPotInterval. If the value were to exceed 255mS then it will just return 255mS
unsigned int debouncePushButtonandUpdateDelayBasedOnPress () {
  // this is currently written for a pull up resistor/pull down switch. To reverse that logic remove the ! in the next line, update pinMode to remove the pullup, and add a pulldown resisor
  bool buttonIsPressed = !digitalRead(pushButtonPin);        //stores whether the button is currently pressed or not. pressed results in a true
  if (buttonIsPressed != previousButtonIsPressed) {        // if there is a change in buttonIsPressed condition, remember the current time
    previousButtonStateChangeTime = millis();
  }
  if ((millis() - previousButtonStateChangeTime) > 30) {        // the 30 is the debounce time in mS. The button needs to have been in the previous state for this many mS in order to register it as a valid buttonState
    if (buttonIsPressed) {                                              // if the button pressed, set a new ledInterval
      templedInterval = ((millis() - previousButtonStateChangeTime)) / 3;
      previousButtonIsPressed = buttonIsPressed;
      return buttonPotInterval;
    }
    else if (!(buttonIsPressed)) {
      //buttonPotInterval = templedInterval;///////////////////
      previousButtonIsPressed = buttonIsPressed;
      if (templedInterval <= 255) {
        return templedInterval;
      }
      else {
        return 255;
      }
    }
  }
  else  { // if note enough debounce time has passes, just pass the old buttonPotInterval
    //previousButtonIsPressed = buttonIsPressed;      // end of function, updates previousButtonIsPressed state with the current state
    previousButtonIsPressed = buttonIsPressed;
    return buttonPotInterval;
  }
}








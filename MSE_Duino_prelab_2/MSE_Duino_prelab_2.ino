// this prelab is really easy to write quick and dirty code for
// to make it more fun; I wrote code that can easily have the number of leds changed

//**********************  NOTES  ************************
// Example: say you want to use 4 leds.
// imagine an array of 12 leds. We can only see the middle 4 leds. The other leds are imaginary.
// the 4 leds on the far left start as on, then the 4 leds gradually move to the far right
// once the 4 leds reach the far right, they "bounce" back and gradually move to the far left
// There are always 4 leds on, and 8 off.
// this is the principle that I used to write my code. There is an array of bools that represent all (real and imagenary) led states.
// The bools are shuffled around in the prescribed pattern


// To change to x number of leds, you will just have to switch the first two lines of code
// You just need to enter the number of leds, and their pins

const unsigned int numberOfLeds = 6;                                // this global var represents the number of leds
const unsigned int ledPin[] = {4, 5, 6, 7, 8, 9, 10};           // these ints represent the pins the leds are connected to. Enter the pin numbers in led order from left to right (or right to left). We only use as many pins as we have leds
unsigned long interval = 200;                                        // sets the "delay" between changes in state(this code doesnt use delay)

bool arrayOfStates[numberOfLeds * 3];                               // these bools represent the state of the leds (including the "invisible" leds from notes)
bool dir;                                                           // (direction) this bool determines which way the leds are moving. False is left to right, true is right to left
unsigned long previousMillis(0);                                    // saves the time at which we previously updated led states
#define commentsForSerialMonitor;                                   // defining this will print usefull messages to the serial monitor. Comment this out if you do not want those messages

void setup() {
  // put your setup code here, to run once:

  // this loop will set the ledPins (stored in array) to output mode
  for (int i = 0; i < numberOfLeds; i++) {
    pinMode(ledPin[i], OUTPUT);
  }

#ifdef commentsForSerialMonitor
  Serial.begin(9600);                                              // pour a bowl of serial. The leds hurt my eyes so I'll just use serial monitor for testing.
#endif

  // sets the first third of array elements to true
  for (int i = 0; i < numberOfLeds; i++) {
    arrayOfStates[i] = true;
  }
}


void loop() {
  // put your main code here, to run repeatedly:

  // if more than interval has expired we do blinky lights stuff, if not we run through loop again
  if (millis() - previousMillis >= interval) {

    // save the last time you blinked the LED
    previousMillis = millis();

#ifdef commentsForSerialMonitor
    // this little function will print the arrayOfStates bools to serial monitor (because leds hurt my eyes)
    Serial.println();
    for (int i = 0; i < (numberOfLeds * 3); i++) {
      Serial.print(arrayOfStates[i]);
      Serial.print("");
    }
#endif

    // this little function updates led states based on its respective bool
    for (int i = 0; i < numberOfLeds; i++) {
      digitalWrite(ledPin[i], arrayOfStates[i + numberOfLeds]);
    }

    // if the last arrayOfStates element is true, change the order from right to left, else if the first arrayOfStates element is true, change the order from left to right
    if (arrayOfStates[numberOfLeds * 3 - 1] == true) {
      dir = true;
    }
    else if (arrayOfStates[0] == true) {
      dir = false;
    }

    // if dir is false, change led states from left to right
    if (dir == false) {
      for (int i = (numberOfLeds * 3 - 1); i >= 0; i--) {
        if (arrayOfStates[i] == true) {
          arrayOfStates[i + 1] = true;
          arrayOfStates[i] = false;
        }
      }
    }
    // else if dir is true, change led states from right to left
    else {
      for (int i = 0; i < (numberOfLeds * 3); i++) {
        if (arrayOfStates[i] == true) {
          arrayOfStates[i - 1] = true;
          arrayOfStates[i] = false;
        }
      }
    }
  }
}

/*
    STATE MACHINE
    This structure holds all of the actions your device will perform. The way it works is each state is a loop that will run until
    you tell the machine to go somewhere else by setting the variable "theState" to the value of a new state in the machine. Each
    state should have a logical condition telling it what to look for to pop it into the next state. Any functions you want to call
    should be called once while the machine is moving between states.

    More details and examples at the bottom of the page.

*/

void stateActions() {
  switch (theState) {                                           //Name of the state machine ("theState")
    case stateIs0:                                              //State the machine will go to when theState == statIs0;
      if (blynkButtonValue[0] == 0 && blynkButtonSwitched[0]) { //Only do an action when Blynk V0 has been switched off
        theState = stateIs1;                                    //Set theState = stateIs1, which will put the machine into that state once stateActions() is called in the main loop
        Ring.Fade(NAVY, TEAL, 100, 5);                          //Play the Ring.Fade() animation
        fadeType = ONCE;                                        //Set the fade type to "ONCE" so it fades up and then holds on that color
        Serial.println("V0 switched Off.");                     //Print out "V0 switched Off."
        Serial.print("Switching to State: ");                   //Print out "Switching to State: "
        Serial.println(theState);                               //Print out the name of the state the machine is going to
      }
      break;                                                    //Separate this state from the next one in the machine
    case stateIs1:
      if (blynkButtonValue[1] == 1 && blynkButtonSwitched[1]) {
        theState = stateIs2;
        Ring.Scanner(TEAL, 25);
        Serial.println("V1 switched On.");
        Serial.print("Switching to State: ");
        Serial.println(theState);
      }
      break;
    case stateIs2:
      if (blynkButtonValue[1] == 0 && blynkButtonSwitched[1]) {
        theState = stateIs3;
        Ring.TheaterChase(TEAL, BLUE_VIOLET, 100);
        Serial.println("V1 switched Off.");
        Serial.print("Switching to State: ");
        Serial.println(theState);
      }
      break;
    case stateIs3:
      if (blynkButtonValue[2] == 1 && blynkButtonSwitched[2]) {
        theState = stateIs4;
        Serial.println("V2 switched On.");
        Serial.print("Switching to State: ");
        Serial.println(theState);
        Ring.Flicker(TEAL, INDIAN_RED, 20, 1);
      }
      break;
    case stateIs4:
      if (blynkButtonValue[2] == 0 && blynkButtonSwitched[2]) {
        theState = stateIsDefault;
        Serial.println("V2 switched ON.");
        Serial.print("Switching to State: ");
        Serial.println(theState);
      }
      else {
        stopWatch(10000);                         //Start a stop watch that goes for 10 seconds (10000 milliseconds)
        if (stopWatchToggle) {                    //When the 3 seconds are through, do an action
          theState = stateIsDefault;              //Set the state to "stateIsDefault"
          Blynk.virtualWrite(V2, 0);              //Reset the Blynk virtual pin V2 (to make the interface look cleaner)
          Serial.print("Switching to State: ");   //Print out "Switching to State: "
          Serial.println(theState);               //Print out the name of the state the machine is going to
        }
      }
      break;
    case stateIs5:
#if HAS_GESTURE == 1 && HAS_SERVO == 1
      if (proxValue > 10 && proxValue < 15) {
        theState = stateIsDefault;
        Serial.println("Proximity is even closer.");
        Serial.print("Switching to State: ");
        Serial.println(theState);
        pwm.setPWM(0, 0, pulseWidth(0));
        Serial.println("Moving Servo 0 to 0.");
        pwm.setPWM(1, 0, pulseWidth(0));
        Serial.println("Moving Servo 1 to 0.");
      }
#endif
      break;
    case stateIs6:
    case stateIs7:          //Any state that doesn't have a "break;" at the end will step through whatever actions you've put in it and then the ones below until it hits a "break:".
    case stateIsDefault:    //Leave the lines between "stateIsDefault:" and "default:" empty
    default:                //Default states are meant for things like standby modes or where a machine will hang out right after startup.
      if (hit == false) {                                       //Activate the Ring.Fade() animation function once when the state machine lands in the default state
        Ring.Fade(ORANGE_RED, GOLD, 100, 5);                    //Play the Ring.Fade() animation
        fadeType = LOOP;                                        //Set the fade type to "LOOP" so it pulses continuously
        hit = true;                                             //Sets a boolean to make sure the animation is only triggered once in this state
      }

      if (blynkButtonValue[0] == 1 && blynkButtonSwitched[0]) {
        theState = stateIs0;
        hit = false;                                            //Set "hit" to "false" whenever you exit the default loop to make sure the default animation is ready to play again
        Serial.println("V0 switched On.");
        Serial.print("Switching to State: ");
        Serial.println(theState);
        Ring.Fade(GOLD, NAVY, 100, 5);
        fadeType = ONCE;
      }
#if HAS_GESTURE == 1 && HAS_SERVO == 1
      if (proxValue > 20 && proxValue < 30) {
        theState = stateIs5;
        hit = false;                                            //Set "hit" to "false" whenever you exit the default loop to make sure the default animation is ready to play again
        Serial.println("Proximity is close.");
        Serial.print("Switching to State: ");
        Serial.println(theState);
        pwm.setPWM(0, 0, pulseWidth(180));
        Serial.println("Moving Servo 0 to 180.");
        pwm.setPWM(1, 0, pulseWidth(180));
        Serial.println("Moving Servo 1 to 180.");
        Ring.Fade(GOLD, HOT_PINK, 100, 5);
        fadeType = ONCE;
      }
#endif
      break;
  }
}

/*
    STATE MACHINES - CONTINUED
    To use: Put a logic statement that looks for an input (such as a Blynk virtual pin) inside the case. Inside that logic statement,
      put everything you'd like to happen including moving into another state. This is really useful for things like having a smart
      bedside lamp start up into a ready state when you push a button, play an animation when turned upside down, play a different one
      when turned right side up, and then go to sleep on a timer that can only be activated after turning it over twice.
        Example:     case stateIs0:
                     if(blynkButtonValue[0] == 0 && blynkButtonSwitched[0]) {
                       theState=stateIs1;
                       Ring.Fade(NAVY, TEAL, 100, 5);
                        Serial.println("V0 switched Off.");
                        Serial.print("Switching to State: ");
                        Serial.println(theState);
                        fadeType = ONCE;
                      }
                      break;

    LOGICAL STATEMENTS
   .Create a logical statement to check a condition (like whether a sensor/button/pin has been changed) and the properties
    of the new condition. After these conditions are met, the machine moves into the state you specify by setting "theState" to that
    new state. Make sure to comment with a description for what you're checking for to make debugging easier (e.g.
    "Serial.println("V1 switched On.");")
       Examples:  "gestureType == APDS9960_UP" for detecting an "UP" gesture from the Adafruit Gesture Sensor
                  "blynkButtonValue[1] == 1 && blynkButtonSwitched[1]" for detecthing whether Blynk button V1 has been switched on

    ANIMATIONS
    There are 6 pre-programmed animations in the KeithNeoPatterns library: Ring.RainbowCycle(), Ring.TheaterChase(), Ring.ColorWipe(),
    Ring.Scanner(), Ring.Fade(), and Ring.Flicker(). They each have different behaviors and need different settings to work.
    To use: Ring.RainbowCycle() swirls a rainbow around the ring. It needs a number to set the length of time in milliseconds between
              animation frames.
              Example: Ring.RainbowCycle(50);
            Ring.TheaterChase() marches alternating lights around the ring. It needs two colors to switch between followed by a number
              to set the length of time in milliseconds between animation frames.
              Example: Ring.TheaterChase(TEAL, BLUE_VIOLET, 100);
                You can also reverse the direction of the animation: Ring.TheaterChase(TEAL, BLUE_VIOLET, 100, REVERSE);
            Ring.ColorWipe() swirls a color around the ring. It needs a color to swirl and a number to set the length of time in
              milliseconds between animation frames.
              Example: Ring.ColorWipe(TEAL, 50);
                 You can also reverse the direction of the animation: RingColorWipe(TEAL, 50 REVERSE);
            Ring.Scanner() runs a comet of light around the ring back and forth. It needs a color to use and a number to set the length of
              time in milliseconds between animation frames.
              Example: Ring.Scanner(TEAL, 50);
            Ring.Fade() fades between two colors. It needs two colors to switch between followed by a number
              to set the length of time in milliseconds between animation frames. It also has a flag that you need to set each time you use
              it to pick whether you want the fade to continuously pulse, or end up on the final color you indicate.
              Example: Ring.Fade(NAVY, TEAL, 100, 5);
                       fadeType = ONCE;
                          You can also use:
                       Ring.Fade(NAVY, TEAL, 100, 5);
                       fadeType = LOOP;
            Ring.Flicker() flickers the ring like a candle. It needs two colors to switch between followed by a number to set how often it
            flickers, and another to set the length of time in milliseconds between animation frames.
              Example: Ring.Flicker(TEAL, INDIAN_RED, 20, 1);

*/

/*
    DEFAULT FUNCTIONS
    These are the default functions that drive this program. These include timers, functions to check
    the state of buttons, and routines to move servos to specific locations.

    DO NOT EDIT ANYTHING ON THIS TAB

*/

////RING COMPLETE - Sets what NeoPixel animations do after they've finished a routine////
void RingComplete() {
  if (Ring.ActivePattern == FADE && fadeType == LOOP) {
    Ring.Reverse();
  }
  if (Ring.ActivePattern == FADE && fadeType == ONCE) {
    Ring.ActivePattern = NONE;
  }
  if (Ring.ActivePattern == FLICKER) {
    Ring.Reverse();

    if (Ring.Direction == REVERSE) { // go back down
      Ring.Interval = random(5, 22); // choose random speed in range
    }
    else {
      rnd = random(random(45, 55), random(80, 255));

      Ring.Pixel = random(0, Ring.numPixels()); // pick a random Pixel
      Ring.Interval = 1;
      Ring.Color2 = Ring.Color(2, rnd, 0, rnd - 10); //GRBW random red and random white-10
    }
  }
}

////STOPWATCH - Countdown allowing you to switch states after specific intervals without using delay()////
bool stopWatch(int actionDuration) {
  if (stopWatchToggle) {
    actionStartMs = millis();
    stopWatchToggle = false;
  }
  else {
    if (millis() - actionStartMs < actionDuration) {
    }
    else {
      actionDuration = 0;
      stopWatchToggle = true;
      Serial.println("stopWatch hit 0.");
    }
  }
}

////BLYNK INPUT INSPECTORS////
int blynkButtonId[12] = {V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11}; //Stores of the names of Blynk virtual pins 0-11
int blynkButtonSwitched[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};         //Identifies which pins have been updated since last checked
int blynkButtonValue[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};            //Stores the values of each Blynk virtual pin


////BLYNK WRITE DEFAULT - Updates the value of each Blynk virtual pin whenever they change////
BLYNK_WRITE_DEFAULT() {
  blynkButtonValue[request.pin] = param.asInt();
  blynkButtonSwitched[request.pin] = 1;
}

////CLEAR BLYNK - Resets the blynkButtonSwitched[] array////
void clearBlynk () {
  for (int i = 0; i < 12; ++i) {
    blynkButtonSwitched[i] = 0;
  }
}

////PULSE WIDTH - Sets servo angles////
#if HAS_SERVO == 1
int pulseWidth(int angle) {
  int pulse_wide, analog_value;
  pulse_wide   = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
  return analog_value;
}
#endif

////GET PROXIMITY - uses an interrupt to get the proximity sensor reading////
#if HAS_GESTURE == 1
void getProximity() {
  if (!digitalRead(APDS9960_INT)) {
    proxValue = apds.readProximity(); //Set proxValue to the proximity reading
    apds.clearInterrupt();            //Clear the interrupt
  }
}
#endif

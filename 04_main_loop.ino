/*
    THE MAIN LOOP
    A State Machine is

    DO NOT EDIT ANYTHING ON THIS TAB

*/

void loop() {
  Blynk.run();

#if HAS_GESTURE == 1              //Reads the Gesture Sensor only if it is initialized
  getProximity();
#endif

  stateActions();
  clearBlynk();
  Ring.Update();
}

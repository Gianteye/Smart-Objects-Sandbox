/*
    SMART OBJECTS SANDBOX
    This is a sketch designed to help you prototype concepts for smart objects. It relies on a few components
    and libraries that are easy to find and offer a lot of interactions and animations without very much setup.
    This sketch will be paired with the Adafruit Feather Huzzah and a NeoPixel Ring. It also can be extended with
    the Servo FeatherWing and APDS9960 Gesture Sensor Breakout from Adafruit right out of the box.

    To Use: Each tab has notes about the code contained within. Make sure to read through everything to make
    sure you are putting your work in the right spot - Arduino tabs can sometimes compile in unintuitive ways.
    The two tabs you will be spending your time on will be "02_custom_functions" and "03_state_machine". 02
    contains a place for any functions you want to create for your projects, and 03 is a state machine where you
    can direct what your Huzzah does under different conditions.

    FIRST RUN
    Install Blynk on your phone - https://blynk.io/en/getting-started
    Sign up for a Blynk account
    Follow all of the instructions in step "Using Arduino IDE" in the following tutorial
      - https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide
      This show you how to:
        Install the Adafruit ESP8266 Library from the Arduino Library Manager (unless it is already in your Libraries folder)
        Install the CP2104 Driver
        Install the ESP8266 Board Package
        Set up your Arduino environment for using the Huzzah

    Drag the libraries provided ("Adafruit_APDS9960_Library", "Adafruit_NeoPixel", "Adafruit_PWM_Servo_Driver_Library",
    "KeithNeoPatterns" and "Blynk") provided in the class resources folder to your Arduino Libraries folder

    How to set up the Blynk demo program:
      Start a new Blynk project and name it "Blynk Buttons and States"
      Click on the nut icon and mail yourself the app's Auth Token
      Paste your auth token into the char auth[] variable
      Add a new device to the app and give your Feather Huzzah a name
      Add 4 buttons to the app
        Name the first V0 Button, assign it to output V0, and make it a Push button
        Name the second V1 Button, assign it to output V1, and make it a Switch
        Name the third V2 Button, assign it to output V2, and make it a Switch
        Name the fourth V3 Button, assign it to output V3, and make it a Switch

    Before doing anything else, you should make sure to paste in your Blynk credentials under "SETTINGS FOR BLYNK"
    and your WiFi SSID and pass under "WIFI CREDENTIALS"

    Make sure the "FEATHER COMPONENT INCLUDES" right below this comment section are set up correctly for your hardware

    LINKS:
    Projects:
      NeoPixel NeoPatterns  https://learn.adafruit.com/multi-tasking-the-arduino-part-3/using-neopatterns
        This project is based off of NeoPatterns to drive animations without using delay()
      LED Candle - https://www.instructables.com/id/About-This-Candle-How-to-Read-This-Instructable/
        The KeithNeoPatterns library comes from keith204's LED Candle project
    Documentation:
      Blynk - http://docs.blynk.cc/
      Adafruit Servo FeatherWing - https://learn.adafruit.com/adafruit-8-channel-pwm-or-servo-featherwing
      Adafruit APDS9960 Breakout Board - https://learn.adafruit.com/adafruit-apds9960-breakout
      Adafruit NeoPixels - https://learn.adafruit.com/adafruit-feather-huzzah-esp8266

*/

////FEATHER COMPONENT INCLUDES////
#define HAS_SERVO 0                 //Set to 0 if you are not using servos with the Servo FeatherWing, set to 1 if you are
#define HAS_GESTURE 0               //Set to 0 if you are not using the APDS9960 Gesture Sensor, set to 1 if you are
#define HAS_RGBW 1                  //Set to 0 if you are not using a RBGW NeoPixel Ring, set to 1 if you are

////MAIN LIBRARIES////
#include <Adafruit_NeoPixel.h>      //Adafruit NeoPixel library
#include "KeithNeoPatterns.h"       //NeoPixel animation ibrary
#include <Wire.h>                   //I2C communication library
#include <ESP8266WiFi.h>            //Huzzah board WiFi library
#include <ESP8266WiFiMulti.h>       //Huzzah board WiFi multiple login library
ESP8266WiFiMulti wifiMulti;         //Sets "wifiMulti" as the nickname for the WiFi connection routine
#include <BlynkSimpleEsp8266.h>     //Huzzah board Blynk library
#include <SoftwareSerial.h>         //Serial communication library

////ADAFRUIT APDS9960 GESTURE SENSOR SETTINGS////
#if HAS_GESTURE == 1
#include "Adafruit_APDS9960.h"      //Gesture sensor library
Adafruit_APDS9960 apds;             //Sets "apds" as the nickname for the gesture sensor
#define APDS9960_INT  2
int proxValue;
//uint8_t gestureType;
#endif

////SERVO FEATHERWING SETTINGS////
#if HAS_SERVO == 1
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define MIN_PULSE_WIDTH       1000
#define MAX_PULSE_WIDTH       2000
#define DEFAULT_PULSE_WIDTH   1500
#define FREQUENCY             60
uint8_t servonum = 0;
#endif

////NEOPIXEL RING SETTINGS////
#define PIN 15                  //Data pin for the NeoPixel Ring
#define NUM_LEDS 12             //Number of pixels on the NeoPixel Ring
#define BRIGHTNESS 10           //Global brightness setting for the NeoPixel Ring

void RingComplete();

#if HAS_RGBW == 1
NeoPatterns Ring(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800, &RingComplete);
#else
NeoPatterns Ring(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800, &RingComplete);
#endif

uint32_t baseColor; //Default ON resting color variable
uint32_t offColor;  //Default OFF resting color variable
byte rnd = 0;

enum fadeTypes {
  LOOP,
  ONCE,
};
fadeTypes fadeType = LOOP;

////BLYNK SETTINGS////
#define CONNECT_TIMEOUT 10000   //Length of time to wait for a WiFi connection before timing out (10sec)

//Go to the Project Settings in your app (nut icon) to find the token.
//Copy the authorization token for your Blynk app and paste it after "char auth[] = "
char auth[] = "aa6cf71be6f7424eabb31d6b854184bc";

////STATE MACHINE SETTINGS////
bool hit = false;
bool stopWatchToggle = true;
unsigned long actionStartMs = millis();
unsigned long previousMillis = 0;        // will store last time LED was updated
long OffTime = 100;          // milliseconds of off-time

enum possibleStates {
  stateIs0,
  stateIs1,
  stateIs2,
  stateIs3,
  stateIs4,
  stateIs5,
  stateIs6,
  stateIs7,
  stateIsDefault,
};
possibleStates theState = stateIsDefault;
int lastState;


////SETUP - Runs once when your microcontroller boots, and then moves over to the main loop////
void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Smart Objects Sandbox V20");
  Serial.println("");

  ////WIFI CREDENTIALS////
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("NETGEAR92", "UKM7HA99K@Fm39gfbp#CmK");       //Enter in your home WiFi name (SSID) and password
  wifiMulti.addAP("VFL (g/n)", "VFL12345");                     //This is the SSID and password for the VFL
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");  //Credentials for any other WiFi you commonly use can go here
  wirelessConnect();

  for (int i = 0; i < 12; i++) {
    Blynk.virtualWrite(i, 0);
    delay(50);
  }

  ////GESTURE SENSOR INITIALIZE - Will only be loaded if you have a gesture sensor plugged in and set "HAS_GESTURE" to 1////
#if HAS_GESTURE == 1
  pinMode (APDS9960_INT, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(APDS9960_INT), isr, FALLING);
  pinMode(0, OUTPUT);
  if (!apds.begin()) Serial.println("Failed to initiate Gesture Sensor. Check wiring and reboot."); //Gesture Sensor altert - stops boot process if the gesture sensor isn't loading properly
  while (!apds.begin()) {
    digitalWrite(0, LOW);
    delay(250);
    digitalWrite(0, HIGH);
    delay(250);
  }
  Serial.println("Gesture Sensor active.");
  digitalWrite(0, LOW);

  apds.enableProximity(true);                     //Gesture mode will be entered once proximity mode senses something close
  apds.setProximityInterruptThreshold(0, 10);     //Set the interrupt threshold to fire when proximity reading goes above 15
  apds.enableProximityInterrupt();
  //apds.enableGesture(true);                     //Due to the way the gesture library is coded, detecting gestures will cause NeoPixel animations to slow down
#endif

  ////SERVO INITIALIZE - Will only be loaded if you have a servo FeatherWing plugged in and set "HAS_SERVO" to 1////
#if HAS_SERVO == 1
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);        //Analog servos run at ~60 Hz updates
  pwm.setPWM(0, 0, pulseWidth(0));  //Assign a servo to address 0 (There should be one of these for each servo you use
  pwm.setPWM(1, 0, pulseWidth(0));  //Assign a servo to address 1
#endif

  //Set ring brightness and activate
  Ring.setBrightness(BRIGHTNESS);       //Set the brightness value
  Ring.begin();
  Ring.show();                          //Initialize all pixels to 'off'
  baseColor = Ring.Color(153, 50, 204); //Set the ring default color
  Ring.Color1 = baseColor;
}

////WIRELESS CONNECT - Tries to connect to all of the wireless connections listed in wifiMulti.addAP(); sequentially////
void wirelessConnect() {
  Serial.println("Connecting Wifi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    unsigned long startConnecting = millis();
    Serial.println("WiFi not connected.");
    delay(1000);
    if (millis() > startConnecting + CONNECT_TIMEOUT) {
      Serial.println("Unable to connect to WiFi.");
      break;
    }
  }

  if (wifiMulti.run() == WL_CONNECTED) {
    Blynk.config(auth);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    unsigned long startConnecting = millis();
    while (!Blynk.connected()) {
      Blynk.connect();

      if (millis() > startConnecting + CONNECT_TIMEOUT) {
        Serial.println("Unable to connect to Blynk server.");
        break;
      }
      Serial.println("Connected to Blynk!");
      Serial.println("");
    }
  }
}

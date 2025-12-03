/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - ESP32 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

/**
 * FILL OUT THE FOLLOWING BASED ON YOUR TEMPLATE INFORMATION
 * 1. BLYNK_TEMPLATE_ID (line 47)
 * 2. BLYNK_TEMPLATE_NAME (line 48)
 * 3. BLYNK_AUTH_TOKEN (line 49)
 * 4. MY_SSID (line 53)
 * 5. MY_PASSWORD (line 54)
 */

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID           "TMPL2Puc_FQQf" // FILL THIS OUT
#define BLYNK_TEMPLATE_NAME         "ESE5190 LAB4" // FILL THIS OUT
#define BLYNK_AUTH_TOKEN            "VVblyWrH-Y32YDRbfAd7WkThirCQXY-L" // FILL THIS OUT

// Your WiFi credentials.
// Set password to "" for open networks.
// #define MY_SSID                     "TMOBILE-5E74" // FILL THIS OUT
// #define MY_PASSWORD                 "cd5kn2shvr6" // FILL THIS OUT

#define MY_SSID                     "EvoPhilly" // FILL THIS OUT
// #define MY_PASSWORD                 "cd5kn2shvr6" // FILL THIS OUT
// const char *ssid = "TMOBILE-5E74";
// const char *password = "cd5kn2shvr6";

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// #define GPIO_V0 17
// #define GPIO_V1 18

// #define GPIO_V2 14 
// #define GPIO_V3 12

volatile int joyX = 0;
volatile int joyY = 0;


// BLYNK_WRITE(V0)
// {
//   int pinValue = param.asInt(); // read the BUTTON
//   if (pinValue == 1) {
//     Serial.println("Button pressed from Blynk0!");
//     digitalWrite(GPIO_V0, HIGH);
//   } else {
//     Serial.println("Button released.");
//     digitalWrite(GPIO_V0, LOW);
//   }
// }

// BLYNK_WRITE(V1)
// {
//   int pinValue = param.asInt(); // read the BUTTON
//   if (pinValue == 1) {
//     Serial.println("Button pressed from Blynk1!");
//     digitalWrite(GPIO_V1, HIGH);
//   } else {
//     Serial.println("Button released.");
//     digitalWrite(GPIO_V1, LOW);
//   }
// }

// BLYNK_WRITE(V2)
// {
//   int pinValue = param.asInt(); // read the BUTTON
//   if (pinValue == 1) {
//     Serial.println("Button pressed from Blynk2!");
//     digitalWrite(GPIO_V2, HIGH);
//   } else {
//     Serial.println("Button released.");
//     digitalWrite(GPIO_V2, LOW);
//   }
// }

// BLYNK_WRITE(V3)
// {
//   int pinValue = param.asInt(); // read the BUTTON
//   if (pinValue == 1) {
//     Serial.println("Button pressed from Blynk3!");
//     digitalWrite(GPIO_V3, HIGH);
//   } else {
//     Serial.println("Button released.");
//     digitalWrite(GPIO_V3, LOW);
//   }
// }
const int DAC_X = 17;   // GPIO25 → DAC1
const int DAC_Y = 18;   // GPIO26 → DAC2


BLYNK_WRITE(V4)
{
  joyX = param.asInt();
  outputDAC();
  // Serial.print("[Joystick] X=");
  // Serial.print(joyX);
  // Serial.print("  Y=");
  // Serial.println(joyY);
}

BLYNK_WRITE(V5)
{
  joyY = param.asInt();
  outputDAC();
  // Serial.print("[Joystick] X=");
  // Serial.print(joyX);
  // Serial.print("  Y=");
  // Serial.println(joyY);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, MY_SSID, NULL);
  
  // pinMode(GPIO_V0, OUTPUT);
  // pinMode(GPIO_V1, OUTPUT);
  // pinMode(GPIO_V2, OUTPUT);
  // pinMode(GPIO_V3, OUTPUT);

  // digitalWrite(GPIO_V1, LOW);
  // digitalWrite(GPIO_V0, LOW);
  // digitalWrite(GPIO_V2, LOW);
  // digitalWrite(GPIO_V0, LOW);

  pinMode(DAC_X, OUTPUT);
  pinMode(DAC_Y, OUTPUT);

 	
}

void outputDAC()
{
  int dacX = map(joyX, 0, 1024, 0, 255);
  int dacY = map(joyY, 0, 1024, 0, 255);

  dacX = constrain(dacX, 0, 255);
  dacY = constrain(dacY, 0, 255);

  dacWrite(DAC_X, dacX);
  dacWrite(DAC_Y, dacY);

  // 串口打印调试
  Serial.print("[Joystick X] Value = ");
  Serial.print(joyX);
  Serial.print(" → DAC = ");
  Serial.print(dacX);
  Serial.print("    ");

  Serial.print("[Joystick Y] Value = ");
  Serial.print(joyY);
  Serial.print(" → DAC = ");
  Serial.println(dacY);
}


void loop()
{
  Blynk.run();
}

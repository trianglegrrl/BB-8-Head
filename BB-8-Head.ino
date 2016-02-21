/*
 BB-8 head electronics (random movement)
 by Alaina Hardie <https://alainahardie.com>

 modified on 21 Feb 2016
 by Alaina Hardie

*/

#include <Servo.h>
#include <SoftwareSerial.h>
#include <Adafruit_Soundboard.h>
#include <Adafruit_NeoPixel.h>

#define SFX_TX_PIN 5 // TX and RX for the Adafruit FX Mini sound board
#define SFX_RX_PIN 6 // Note: these pinouts are for the pins labeled on the sound board
#define SFX_RST 4 // Connect to the RST pin on the sound board for putting in serial mode

#define SERVO_PIN 9

#define NEOPIXEL_CONTROL_PIN 10
#define NUMBER_OF_NEOPIXELS 1 // Holoprojector, PSI, logic
#define NEOPIXEL_HOLOPROJECTOR 1
#define NEOPIXEL_PSI 2
#define NEOPIXEL_LOGIC 3

#define RADAR_EYE_LED_PIN 11 // For the red LED in the radar eye

Servo headTurnServo;  // continuous-rotation servo for turning the haed
SoftwareSerial soundBoardSerial = SoftwareSerial(SFX_TX_PIN, SFX_RX_PIN);
Adafruit_NeoPixel neopixelArray = Adafruit_NeoPixel(60, NEOPIXEL_CONTROL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_Soundboard soundBoard = Adafruit_Soundboard(&soundBoardSerial, NULL, SFX_RST);

#define FLASH_PERIOD 1000

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

unsigned long previousMillis = 0;

bool pin1State = HIGH;
bool pin2State = LOW;

int lastServoVal = 90;

void setup() {
  Serial.begin(9600); // Console

  initializeSoundboard();
  initializeNeopixels();
  initializeServos();

  randomSeed(analogRead(5)); // Seed with sample of random unconnected analog voltage
}

void initializeServos() {
  // There's only one servo right now

  headTurnServo.attach(SERVO_PIN);
  headTurnServo.write(90); // This is continuous-rotation, so brake the servo
}

void initializeSoundboard() {
  // Adafruit FX Mini sound board
  soundBoardSerial.begin(9600);

  if (!soundBoard.reset()) {
    Serial.println("Not found");
    while (1);
  } else {
    Serial.println("Adafruit sound board found");
  }
}

void initializeNeopixels() {
  neopixelArray.begin();
  neopixelArray.show(); // Initialize all pixels to 'off'
}

void speakRandomSound() {
  int soundFileNumber = random(0,10);

  Serial.print("Speaking random sound ");
  Serial.println(soundFileNumber);

  soundBoard.playTrack(soundFileNumber);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<neopixelArray.numPixels(); i++) {
      neopixelArray.setPixelColor(i, Wheel((i+j) & 255));
    }
    neopixelArray.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return neopixelArray.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return neopixelArray.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return neopixelArray.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void doRandomLightThing() {
  Serial.println("DOUBLE RAINBOW ALL THE WAY");

  rainbow(2);
}

void turnHeadRandomly() {
  int randomSpeed = random(60, 120);
  int randomWait = random(15, 35);

  Serial.print("Turning randomly at speed ");
  Serial.print(randomSpeed);
  Serial.print(" and waiting for ");
  Serial.print(randomWait);
  Serial.println(" ms");

  headTurnServo.write(randomSpeed);
  delay(randomWait);
}

void loop() {
  int randomTask = random(0, 2);
    if(randomTask == 0) {
      speakRandomSound();
    } else if (randomTask == 1) {
      doRandomLightThing();
    } else {
      turnHeadRandomly();
    }

    delay(random(100, 300)); // Delay between 1 and 3 seconds
}






/************************ MENU HELPERS ***************************/

void flushInput() {
  // Read all available serial input to flush pending data.
  uint16_t timeoutloop = 0;
  while (timeoutloop++ < 40) {
    while(soundBoardSerial.available()) {
      soundBoardSerial.read();
      timeoutloop = 0;  // If char was received reset the timer
    }
    delay(1);
  }
}

char readBlocking() {
  while (!Serial.available());
  return Serial.read();
}

uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);
  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}

uint8_t readline(char *buff, uint8_t maxbuff) {
  uint16_t buffidx = 0;

  while (true) {
    if (buffidx > maxbuff) {
      break;
    }

    if (Serial.available()) {
      char c =  Serial.read();
      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0) {  // the first 0x0A is ignored
          continue;
        }
        buff[buffidx] = 0;  // null term
        return buffidx;
      }
      buff[buffidx] = c;
      buffidx++;
    }
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}
/************************ MENU HELPERS ***************************/

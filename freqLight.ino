/* LedStripRainbow: Example Arduino sketch that shows
 * how to make a moving rainbow pattern on an
 * Addressable RGB LED Strip from Pololu.
 *
 * To use this, you will need to plug an Addressable RGB LED
 * strip from Pololu into pin 12.  After uploading the sketch,
 * you should see a moving rainbow.
 */

//#include <PololuLedStrip.h>
#include "arduinoFFT.h"
#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES];

int sensorPin = 25;    // select the input pin for the potentiometer//
//int ledPin = 13;      // select the pin for the LED

// Create an ledStrip object and specify the pin it will use.
 const int redPin = 5;     // 13 corresponds to GPIO13
const int greenPin = 4;   // 12 corresponds to GPIO12
const int bluePin = 21;    // 14 corresponds to GPIO14

const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 30
int* colors = new int[3];

void setup()
{

   // configure LED PWM functionalitites
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled43w
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  
    Serial.begin(9600);
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY));
}

// Converts a color from HSV to RGB.
// h is hue, as a number between 0 and 360.
// s is the saturation, as a number between 0 and 255.
// v is the value, as a number between 0 and 255.
int* hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    int* color = new int[3];
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    color[0] = r;
    color[1] = g;
    color[2] = b;
    return color;
}

float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

float step(float e, float x) { return x < e ? 0.0 : 1.0; }



void loop()
{
  // Update the colors.
  uint16_t time = millis() >> 2;
//  ledStrip.write(colors, LED_COUNT);
   for(int i=0; i<SAMPLES; i++)
    {
        microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script. 
     
        vReal[i] = analogRead(25); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes imaginary term 0 always
        /*remaining wait time between samples if necessary*/
        while(micros() < (microSeconds + samplingPeriod))
        {
          //do nothing
        }
    }
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    /*Find peak frequency and print peak*/
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
   //int hsv = (peak/1000)*359;
   //Serial.println(peak-200/800 * 359);
   //Serial.println(analogRead(A0));
  colors = hsvToRgb(peak/3, 100, 255);
  Serial.println(peak);
//  for(uint16_t i = 0; i < LED_COUNT-1; i++)
//  {
//    //byte x = (time >> 2) - (i << 3);
//      //rgb_color temp = colors[i+1];
//    
//      //colors[i+1] = colors[i];
//      colors[i] = colors[0];
//      ledStrip.write(colors, LED_COUNT);
//      //delay(1);
//  }
     //colors[i+1] = colors[i];

  // Write the colors to the LED strip.
//    ledStrip.write(colors, LED_COUNT);


  ledcWrite(redChannel, colors[0]);
  ledcWrite(greenChannel, colors[1]);
  ledcWrite(blueChannel, colors[2]);

  //delay(10);
}

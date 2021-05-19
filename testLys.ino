//NeoPixel 
#include <Adafruit_NeoPixel.h>
#define PIN 3   // input pin Neopixel is attached to
#define NUMPIXELS      20 // number of neopixels in strip
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Phototransistor: 10 kOhm restistor to the phototransistor 
const int ptPin=A0; //Phototransistor-pin

int delayval = 100; // timing delay in milliseconds

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

void setup() {
  // Initialize the NeoPixel library.
  pixels.begin();
  pixels.show();//turn OFF all pizzels
  pixels.setBrightness(50); //max=255, light gets too sharp/blinding if the brightness is on max
  pinMode(ptPin,INPUT); //phototransistor
}

void loop() {
  if (isCupOn()){
//    setRandomColor();
//    colorLights();
//
//    setGreenColor();
//    colorLights();
//
//    setRedColor();
//    colorLights();
    rainbow(5);
    theaterChaseRainbow(50);
    
  }
  if (!isCupOn()) {
    pixels.clear();
    pixels.show();
  }

  
}

// setColor()
void colorLights(){
  for (int i=0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));

    // This sends the updated pixel color to the hardware.
    pixels.show();

    // Delay for a period of time (in milliseconds).
    delay(delayval);
    } 
}

// picks random values to set for RGB
void setRandomColor(){
  redColor = random(0, 255);
  greenColor = random(0,255);
  blueColor = random(0, 255);
}

void setGreenColor(){
  redColor = 0;
  greenColor = 255;
  blueColor = 0;
}

void setRedColor(){
  redColor = 255;
  greenColor = 0;
  blueColor = 0;
}

void setBlueColor(){
  redColor = 0;
  greenColor = 0;
  blueColor = 255;
}

//Phototransistor - component that generates a current proportional to the quantity of light absorbed
//reads the Phototransistor, from 0-1023.
//if the value is below a certain value, return true (the Cup is covering the coaster)
//else nothing is covering the coaster
bool isCupOn(){
  int sensorValue=analogRead(ptPin);
  if (sensorValue>=50){
    return false;
  }
  if (sensorValue<50){
    return true;
  }
}

//From Strandtest example, NeoPixel Library
// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<pixels.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

//From Strandtest example, NeoPixel Library
// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      pixels.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<pixels.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / pixels.numPixels();
        uint32_t color = pixels.gamma32(pixels.ColorHSV(hue)); // hue -> RGB
        pixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      pixels.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

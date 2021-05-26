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

bool bothConnected = false;


#include <SPI.h>
#include <RH_NRF24.h>

RH_NRF24 nrf24;


void setup() {
  // Initialize the NeoPixel library.
  pixels.begin();
  pixels.show();//turn OFF all pizzels
  pixels.setBrightness(50); //max=255, light gets too sharp/blinding if the brightness is on max
  pinMode(ptPin,INPUT); //phototransistor

   Serial.begin(9600);
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");  
}

void loop() {    
  //if this coaster sends invitation 
  if (isCupOn()){
    pixels.setBrightness(50);
    colorPixels(255,255,0); //gul 
    Serial.println("Sender kaffepause-invitasjon");
    // Send a message to nrf24_server
    uint8_t data[] = "Jeg er klar for kaffepause!";
    nrf24.send(data, sizeof(data));
    nrf24.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    bool gotReply=false;
    while(!gotReply){
      if (nrf24.waitAvailableTimeout(10))
      { 
        // Should be a reply message for us now   
        if (nrf24.recv(buf, &len))
        {
          Serial.print("got reply: ");
          Serial.println((char*)buf);
          
          gotReply=true;
          bothConnected=true;
          
        }
        else
        {
          Serial.println("recv failed");
        }
      }  
  }

   if (bothConnected){
    //when the coaster gets a reply - connect both
    rainbow(2); //rainbow effect to show that the two coasters are connected
    //colorPixels(0,255,0); //gronn
    startAndCountDownBreak(60); //Color pixels green and turn of LEDs one by one until the break ends.
   }
  
    
  } //end isCupOn();

  //else waiting to see if the other coaster sends invitation
  else {
    pixels.setBrightness(25); //turn down brightness so that it doesn't shine so bright when it's 'inactive'
    colorPixels(255,255,255); //set white
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    //check to see if coaster has gotten an invitation 
    if (nrf24.recv(buf, &len))
        {
          Serial.print("Got invitation: ");
          Serial.println((char*)buf);
          pixels.setBrightness(50); //set up Brightness
          colorPixels(255,255,0); //color yellow
          bool acceptInvitation=false;
          while (!acceptInvitation){
            acceptInvitation=isCupOn();
          }
          Serial.println("Aksepterer kaffepause-invitasjon");
          // Send a message to nrf24_server
          uint8_t data[] = "Jeg blir med paa kaffepause!!";
          nrf24.send(data, sizeof(data));
          nrf24.waitPacketSent();
          rainbow(2); //rainbow effect to show that the two coasters are connected
          startAndCountDownBreak(60); //Color pixels green and turn of LEDs one by one until the break ends.
        }
  }
}


//color fra 0-255
//Color all pixels with 
void colorPixels(int red, int green, int blue){
  for (int i=0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i,pixels.Color(red,green,blue));
    pixels.show();
    delay(10); //so that the lights show up one after one 
  }
}

// ColorLights according to variables redColor,greenColor,blueColor
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

//5min*60sec = 300 sec 
//countdownBreakTime
void startAndCountDownBreak(int seconds){
  colorPixels(0,255,0); //color pixels green 
  int milliseconds = seconds*1000;
  int timeBetween = milliseconds/NUMPIXELS; //round up/down how long time it will take before each led is turned off
  delay(timeBetween);
  for (int i=NUMPIXELS-1; i = 0; i--){
    pixels.clear(); //clear all pixels. Haven't found a method that only clears one pixel
    pixels.show();
    for (int j=0; j=i; j++){
      pixels.setPixelColor(j,pixels.Color(0,255,0)); //make it green 
      pixels.show();
    }
    delay(timeBetween);
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

void setWhiteColor(){
  redColor=255;
  greenColor=255;
  blueColor=255;
}


//Phototransistor - component that generates a current proportional to the quantity of light absorbed
//reads the Phototransistor, from 0-1023.
//if the value is below a certain value, return true (the Cup is covering the coaster)
//else nothing is covering the coaster
bool isCupOn(){
  int sensorValue=analogRead(ptPin);
  if (sensorValue<250){ 
    return true;
  }
  else {
    return false;
  }
}

//From Strandtest example, NeoPixel Library
// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
// 26.05: changed from 5*65536 to 2*65536
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 2*65536; firstPixelHue += 256) {
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

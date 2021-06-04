//NeoPixel 
#include <Adafruit_NeoPixel.h>
#define PIN 3   // input pin Neopixel is attached to / hvilken Pin lyslenka sin datainput har
#define NUMPIXELS      20 // number of neopixels in strip / antall LEDlys i lyslenka
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Radiohead nrf24
#include <SPI.h> 
#include <RH_NRF24.h>
// inkluderer bibliotekene vi trenger og definerer pins og antall led

RH_NRF24 nrf24;

const int ptPin=A0; //Fototransistoren 

int delayval = 100; // delay i ms

int redColor = 0;
int greenColor = 0;
int blueColor = 0;
// variabler for LED

void setup() {
  // Initialize the NeoPixel library.
  pixels.begin();
  pixels.show();//viser lysene, forelopig avskrudd
  pinMode(ptPin,INPUT); //fototransistor

  Serial.begin(9600);
    while (!Serial) 
      ; // venter på serial port tilkobling
    if (!nrf24.init())
      Serial.println("init failed");
    // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
    // feilmeldinger
    
    if (!nrf24.setChannel(1))
      Serial.println("setChannel failed");
    if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
      Serial.println("setRF failed");
    // feilmeldinger    
      
     pixels.setBrightness(25); //max=255, lysstyrke. setter lysstyrken litt lavere slik at lysene ikke lyser saa skarpt
     colorPixels(255,255,255); //lyser hvitt
}

void loop() {
  if (nrf24.available() && !isCupOn())
    // dersom det er en melding og koppen ikke er på
   { 
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    // lagrer meldingen og lengden på meldingen
    
    if (nrf24.recv(buf, &len))
    // dersom meldingen er gyldig
    {
      while (!isCupOn()){
        // her har vi mottat meldingen om kaffepause, men må vente på koppen
        pixels.setBrightness(50); 
        colorPixels(255,255,0); 
        // brukeren varsles ved at lyset blir gult
      }
      
      
      uint8_t data[] = "Det er jeg også :^)";
      // lager meldingen
      nrf24.send(data, sizeof(data));
      nrf24.waitPacketSent();
      // her blir meldingen sendt
    }
    else
    // om den ikke er gyldig
    {
      Serial.println("recv failed");
      // feilmelding
    }
    rainbow(2);
    // regnbue
    pause ();
    // nå er det kaffepause
  }
  if (isCupOn()){
  // dersom koppen blir lagt paa
    pixels.setBrightness(50);
    colorPixels(255,255,0); 
    // bli gul
  uint8_t data[] = "jeg vil ha kaffepause";
  // lag melding
  nrf24.send(data, sizeof(data));
  // sender melding
  
  nrf24.waitPacketSent();
  // vent på svar
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  // lagrer meldingen og lengden på meldingen

  if (nrf24.waitAvailableTimeout(500)) 
  { 
    // venter paa returmelding  
    if (nrf24.recv(buf, &len))
    // dersom meldingen er gyldig og vi faar en returmelding
    {
      rainbow(2);
      // regnbue 
      pause ();
      // nå er det kaffepause
    }
    else
    // ugyldig melding
    {
      Serial.println("recv failed");
      // feilmelding
    }
  }
  else
  {
    Serial.println("No reply, is nrf24_server running?");
    // dette vil den skrive ut mens den venter på svar
  }
  delay(400);
  // venter litt
    
  }
  if (!isCupOn()){
    // dersom koppen ikke er på og det ikke er pause så skal lyset være hvitt
    colorPixels(255,255,255);
  }
}

void colorPixels(int red, int green, int blue){
// metode for å bytte farge på alle lysene
  for (int i=0; i < NUMPIXELS; i++) {
  // for hver led
    pixels.setPixelColor(i,pixels.Color(red,green,blue)); // gi leden farge
    pixels.show(); // vis
    delay(10); //så lysene har litt mellomrom
  }
}

bool isCupOn(){
// definerer metoden for å sjekke om koppen er paa
  int sensorValue=analogRead(ptPin);
  // henter verdien fra fototransistoren, verdi kan vaere max 1023
  if (sensorValue>50){
    return false;
    // dersom det er lyst (koppen er ikke paa) gi false
  }
  if (sensorValue<50){
    return true;
    // dersom det er mørkt (koppen er paa) gi true
    
  }
}

//rainbow er tatt fra Strandtest example, NeoPixel Library
// enkelte justeringer er gjort i koden for at den ikke skal loopes gjennom altfor mange ganger
// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
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

void pause (){
// definerer pausemetoden
  colorPixels(0,255,0);
  // gjør alle lysene grønne
  for (int i=0; i < NUMPIXELS; i++) {
   // for hver pixel
    pixels.setPixelColor(i,pixels.Color(255,255,255));
    pixels.show();
    // gjør pixelen hvit
    delay(1000); 
    //venter angitt tid før den gjør neste pixel hvit.  
    //delay(15000) dersom det er 5 min pause, for testing brukte vi 1000ms
  }
}

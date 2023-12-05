#include <FastLED.h>
#include <DMXSerial.h>

#define switchPin 2
#define ledPin 3
#define NUM_LEDS 110
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

int colors[][3] = {
  //{255,255,255}, // White
  {255,0,0}, // Red
  //{255,128,0}, // Orange
  {255,60,0}, // Yellow
  //{128,255,0}, // Lime
  {0,255,0}, // Green
  {255,0,64}, // Rose
  //{0,255,128}, // Pale green
  {0,255,128}, // Turquoise
  //{0,128,255}, // Pale blue
  {0,0,255}, // Blue
  //{64,0,255}, // Purple
  {255,0,255}, // Pink
};

int num_colors = sizeof(colors) / sizeof(colors[0]);

int id_code = 85; // ID Code (always 85 or 217)
int group_code = 0; // Group (0 = All)

// Send data for a brief period then shut off to avoid interference
int hold_time = 2000; // milliseconds to hold each colour
int tx_time = 1000; // milliseconds to transmit for

bool tx = 0;

void setup() {
  delay(1000);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<WS2811,ledPin, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(128);

  // Read the switch at startup to define the mode - default to Rx
  if (digitalRead(switchPin)) {
    DMXSerial.init(DMXReceiver);
    
    // Show a single white LED to indicate power on
    leds[0].setRGB(255,255,255);
    FastLED.show();

  } else {
    tx = 1;
    DMXSerial.init(DMXController);
    DMXSerial.maxChannel(512);
  
    // Long transmit at startup to get things in sync
    DMXSerial.write(1, id_code);
    DMXSerial.write(2, group_code);
    DMXSerial.write(3, 255);
    DMXSerial.write(4, 255);
    DMXSerial.write(5, 255);
    digitalWrite(LED_BUILTIN, HIGH);
    leds[0].setRGB(255,255,255);
    FastLED.show();
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
  }

} // setup


void loop() {
  if (!tx) { 
    // Receiver Mode
    if (DMXSerial.read(1) != 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      CRGB color(DMXSerial.read(3), DMXSerial.read(4), DMXSerial.read(5));
      if (leds[0] != color) {
        fill_solid(leds, NUM_LEDS, color);
        FastLED.show();
      }
    }
    else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  } else {
    // Transmit mode
    for (int i=0; i <num_colors; i++) {
    DMXSerial.write(2, group_code);
    DMXSerial.write(3, colors[i][0]);
    DMXSerial.write(4, colors[i][1]);
    DMXSerial.write(5, colors[i][2]);

    //led[0].setRGB(colors[i][0],colors[i][1],colors[i][2]);
    CRGB color(colors[i][0], colors[i][1], colors[i][2]);
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    
    DMXSerial.write(1, id_code);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(tx_time);
    
    // Turn off transmitter until next change
    DMXSerial.write(1, 0);
    digitalWrite(LED_BUILTIN, LOW);
    delay(hold_time);
  }
  }
} // loop

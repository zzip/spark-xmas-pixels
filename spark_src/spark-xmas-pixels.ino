#include "application.h"
#include "neopixel.h"
//#include "spark_disable_wlan.h" // For faster local debugging only


// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN A7
#define PIXEL_COUNT 11
#define PIXEL_TYPE WS2812

// CONVERT_AT (for combining WS2811 and WS2812 (r,g,b) and (g,r,b))
// set this to the pixel number where your WS2811 strip starts if any
// we assume the first part of your strip is WS2812
// set to -1 if you only use a WS2811 strip
#define CONVERT_AT 12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

//bool clear_first = false;
int cycle_wait = 10;
int brightness = 255;
byte red = 1;
byte green = 1;
byte blue = 1;
char cmd = 'f';

//-----------------------------1---------2---------3---------4---------5---------6----
//--------------------1234567890123456789012345678901234567890123456789012345678901234
char sequence1[64] = "-1R9o1l9s9i5r9W1w2l9R1l1R1l1r1o9W1w1W1w1R1r1s9i9R1l2";
byte current_sequence = 0;
byte current_sequence_count = 0;

byte seq_red = 1;
byte seq_green = 1;
byte seq_blue = 255;


byte state = 0;

//strobe initial background
uint32_t background[PIXEL_COUNT];

//linerider
 byte lineriderTable[] = { 5,28,85,255,85,28,5 };


int setSequence(String new_seq) {
  current_sequence = 0;
  new_seq.toCharArray(sequence1, 64);

  return 1;
}

int handleParams(String command) {
    state = 0;
    int p = 0;

    //stop the sequence



    while (p<(int)command.length()) {
        int i = command.indexOf(',',p);
        if (i<0) i = command.length();
        int j = command.indexOf(':',p);
        if (j<0) break;
        String key = command.substring(p,j);
        String value = command.substring(j+1,i);

        //only interrupt the sequence if we receive a new effect
        if (key=="E")
            sequence1[0] = '-';

        // global params
        if (key=="w")
            cycle_wait = value.toInt();
        else if (key=="E")
            cmd = value[0];
        //else if (key=="C")
        //    clear_first = (value=="1");
        else if (key=="B") {
            brightness = value.toInt();
            strip.setBrightness(brightness);
            strip.show();
        }
        else if (key=="r")
            red = value.toInt();
        else if (key=="g")
            green = value.toInt();
        else if (key=="b")
            blue = value.toInt();
        else if (key=="c") {
            String hexcolor = value;
            int number = (int) strtol( &hexcolor[1], NULL, 16);
            red = number >> 16;
            green = number >> 8 & 0xFF;
            blue = number & 0xFF;
        }

        p = i+1;
    }

    return 1;

}

int processParams() {
    if (state == 0) {
      Spark.publish("new-cmd", &cmd, 60, PRIVATE);
      state = 1;
    }

//    if (clear_first == true) {
//      fill(0,0,0);
//      clear_first = false;
//    }

    switch (cmd) {
        case 'b': //breath
            breath(cycle_wait);
            return 2048;
        case 'c': //cylon
            cylon(cycle_wait);
            return 2048;
        case 'f': //fill
            fill(red, green, blue);
            cmd='-';
            return 2;
        case 'F': //Fade In
            fade_in(cycle_wait);
            return 6;
        case 'i':
            fire();
            return 5;
        case 'l':
            linerider(cycle_wait);
            return 4;
        case 'L':
            lightUp(cycle_wait);
            return 10;
        case 'o':
            fill(0,0,0);
            cmd='-';
            return 0;
        case 'O':
            fade_out(cycle_wait);
            return 7;
        case 'r': //rainbow
            rainbow(cycle_wait);
            return 4096;
        case 'R': //rainbow
            rainbowCycle(cycle_wait);
            return 4096;
        case 's': //strobe
            strobe(red, green, blue, 15, cycle_wait);
            return 1;
        case 'w':
            wipe(1, red, green, blue, cycle_wait);
            cmd='-';
            return 3;
        case 'W':
            wipe(-1, red, green, blue, cycle_wait);
            cmd='-';
            return 3;
        default:
            delay(5);
            return 128;
    }
    return 0;
}

int processSequence() {
    //grab current_sequence command and play_count
    cmd = sequence1[current_sequence];
    byte total_play_count = sequence1[current_sequence+1] - '0';

    switch (cmd) {
        case 'i':
        case 's':
        total_play_count *= 10;
    }

    //if play_count > current_sequence_count, set cmd to current_sequence cmd, increase play_count, and call processParams for actually doing the effect
    if  (total_play_count > current_sequence_count) {
        current_sequence_count++;
    }
    else {
        //reset current play count, hop to next sequence, and call ourselves again
        if (current_sequence+2 <  strnlen(sequence1,64))
            current_sequence += 2;
        else
            current_sequence = 0;

        red = seq_red;
        blue = seq_blue;
        green = seq_green;
        current_sequence_count = 0;
        state = 0;
        processSequence();
        //if play_count = current_sequence_count, get following sequence cmd, set current_sequence_count to 0 and call processParams for actually doing the effect
    }



    processParams();
    return 0;
}


void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);

  //--------------123456789012------------------
  Spark.function("handleparams", handleParams);
  Spark.function("setsequence", setSequence);
}

void loop() {
  if (sequence1[0] != '-')
    processSequence();
  else
    processParams();
}

/*
  int i;
  delay(2000);

  for(int i=0;i<=2;i++)
    { linerider(1200); //20*60 }

*/


//wrapping function to convert led strip ws2811 grb to rgb
void setColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b )
{
    if (n > CONVERT_AT)
     strip.setPixelColor(n,g,r,b);
    else
     strip.setPixelColor(n,r,g,b);
}

void setColor(uint16_t n, uint32_t c)
{
    uint8_t
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
    setColor(n, r, g, b);
}


// Query color from previously-set pixel (returns packed 32-bit RGB value)
uint32_t getColor(uint16_t n) {
  uint32_t pixel = 0;

  //get pixel colors
  uint32_t c = strip.getPixelColor(n);

  if (c != 0) {
    //extract to r,g,b
    uint8_t
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;

    //correct r and g position if needed
    if (n > CONVERT_AT)
        pixel = g << 16 | r << 8 | b;
    else
        pixel = r << 16 | g << 8 | b;
  }

  return pixel; // Pixel # is out of bounds
}



//Effects

void fill(uint8_t r, uint8_t g, uint8_t b)
{
  for(int i=0; i < strip.numPixels(); i++)
  {
     setColor(i,r,g,b);
  }
  strip.show();
}


void lightUp(uint8_t wait) {
  for(byte percent=0; percent <= 100; percent++) //Strip crash on USB Power
    { strobe(255, 255, 255, percent, wait); }
}

void breath(uint8_t wait)
{
  fade_in(wait);
  fade_out(wait);
}

void fade_in(uint8_t wait)
{
    for(int i=brightness; i < 255 ; i++)
    {
      //setColor(i,r,g,b);
      brightness += 1;
      strip.setBrightness(brightness);
      strip.show();
      delay(wait);
    }
}

void fade_out( uint8_t wait)
{
    for(int i=brightness; i > 1; i--)
    {
      //setColor(i,r,g,b);
      brightness -= 1;
      strip.setBrightness(brightness);
      strip.show();
      delay(wait);
    }
}

void cylon ( uint8_t wait)
{
  for(int i=1; i <= 3; i++)
  {
    if (i>3)
      i-=3;
    wipe(1, 255/2*(2-i)*(3-i),-255*(1-i)*(3-i),255/2*(1-i)*(2-i),wait);
    wipe(1, 0,0,0,wait);
  }
}

//dir = 1 => away from source, -1 => to source
void wipe(int dir, uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
{
  if (dir==1) {
    for(int i=0; i < strip.numPixels(); i++)
    {
      setColor(i,r,g,b);
      strip.show();
      delay(wait);
    }
  }
  else if (dir==-1) {
    for(int i=strip.numPixels()-1; i >= 0; i--)
    {
      setColor(i,r,g,b);
      strip.show();
      delay(wait);
    }
  }

}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      setColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
    if (state == 0) { break; }
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i< strip.numPixels(); i++) {
      setColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
    if (state == 0) { break; }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void strobe(uint8_t r, uint8_t g, uint8_t b, byte percent, uint8_t wait) {
  int i;

  //do this only on initial call
  if (state < 2) {
    state = 2;
    for(i=0; i < strip.numPixels(); i++)
    {
        background[i] = getColor(i);
    }
  }

  for (i=0; i < strip.numPixels(); i++)
    {
      if(random(0, 100) <= percent)
      {
        setColor(i,r,g,b);
      }
      else
      {
        setColor(i,background[i]);
      }
    }
  strip.show();
  delay(wait);
}

void fire()
{
    byte rand;
    for (int i=0; i < strip.numPixels(); i++)
    {
        rand = random(10,100);
        setColor(i,rand,rand * 0.75,0);
        //strip.setPixelColor(i,rand,rand * 0.75,0);
    }
    strip.show();
    delay(random(20));
}

void fire2(uint16_t duration) //doesn't work
{
  unsigned long stopTime = millis() + duration;
  byte rand;
  int i;
  uint32_t old[strip.numPixels()];
  while(millis()<=stopTime)
  {
    for(i=0; i < strip.numPixels(); i++)
      old[i] = strip.getPixelColor(i);

    for (i=0; i < strip.numPixels(); i++)
    {
      rand = random(10,100);

      if(i>=1&&i<=strip.numPixels()-2)
      {
        setColor(i, (strip.Color(rand, rand * 0.75, 0)+old[i]+old[i-1]+old[i+1])/4);
      }
      else
        setColor(i,rand,rand * 0.75,0);
    }
    strip.show();
    delay(random(20));
  }
}

void linerider(int wait)
{
  //setup (place the initial pixels, at the end of the strip)
  for (int i=1;i<=6;i++)
    setColor(strip.numPixels()-i,lineriderTable[i],0,0);
  strip.show();
  delay(wait);

  //shift them in one direction (from the end to the start of the strip)
  for(int i=0;i<=strip.numPixels()-8;i++)
  {
    shift(-1);
    delay(wait);
  }

  //shift them back in the other direction (from the start back to the end of the strip)
  for(int i=0;i<=strip.numPixels()-8;i++)
  {
    shift(1);
    delay(wait);
  }
}

void swap(int index1, int index2)
{
  uint32_t temp = strip.getPixelColor(index1);
  setColor(index1, strip.getPixelColor(index2));
  setColor(index2, temp);
  strip.show();
}

void shift(int dir) //dir = 1 => away from source, -1 => to source
{
  uint32_t temp;
  int i;
  if (dir==1)
  {
    temp = getColor(strip.numPixels()-1);
    for(i=strip.numPixels()-1; i > 0; i--)
    {
      setColor(i, getColor(i-1));
    }
    setColor(0, temp);
    strip.show();
  }
  else if(dir==-1)
  {
    temp = getColor(0);
    for (i=0; i < strip.numPixels()-1; i++)
    {
      setColor(i, getColor(i+1));
    }
    setColor(strip.numPixels()-1,temp);
  }
  strip.show();
}


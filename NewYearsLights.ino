#include "Tlc5940.h"
#include "colors.h"

int time = 1000;
int debug = 0;
int h = 0;

// global value for smoother set_all behavior (between calls of setAll())
int _set_all_led_num = 0;

#define MAX_AT_ONCE 6

#define NUM_LEDS 21
int LEDS[NUM_LEDS][3] = {
  {0, 1, 2 },
  {3, 4, 5 },
  {6, 7, 8  },
  {9, 10, 11  },
  {12, 13, 14  },
  {16, 17, 18  },
  {19, 20, 21  },
  {22, 23, 24  },
  {25, 26, 27  },
  {28, 29, 30  },
  {31, 32, 33  },
  {34, 35, 36  },
  {37, 38, 39  },
  {40, 41, 42  },
  {43, 44, 45  },
  {46, 47, 48  },
  {49, 50, 51  },
  {52, 53, 54  },
  {55, 56, 57  },
  {58, 59, 60  },
  {63, 62, 61  },
};


// set color functions - they all set a single led (numbered by index in the
// LEDS array) to value from some color struct
void setColor(int ledNum, LedRGB lrgb) {
  ledNum = ledNum % NUM_LEDS;
  int red_pin = LEDS[ledNum][0];
  int green_pin = LEDS[ledNum][1];
  int blue_pin = LEDS[ledNum][2];

  Tlc.set(red_pin, lrgb.r);
  Tlc.set(green_pin, lrgb.g);
  Tlc.set(blue_pin, lrgb.b);
}

void setColor(int ledNum, RGB rgb) {
  LedRGB lrgb = RGBtoLED(rgb);
  setColor(ledNum, lrgb);
}

void setColor(int ledNum, HSV hsv) {
  if (hsv.h > 359) {
    hsv.h = hsv.h % 360;
  }
  RGB rgb = HSVtoRGB(hsv);
  setColor(ledNum, rgb);
}

// sets all LEDs to hsv value for a duration of ms
void setAll(HSV hsv, long ms) {
  int cycles = (NUM_LEDS / MAX_AT_ONCE) + 1;
  long start = millis();
  int delay_time = 1;

  while((millis() - start) < ms) {
    for (int cycle = 0; cycle < cycles; cycle++) {
      Tlc.clear();
      for(int i = 0; i < MAX_AT_ONCE; i++) {
        // increment led number, reset to 0 at max
        _set_all_led_num = ++_set_all_led_num % NUM_LEDS;
        setColor(_set_all_led_num, hsv);
      }
      Tlc.update();
      delayMicroseconds(1000);
    }
  }
  Tlc.clear();
  Tlc.update();
  return;
}


// color conversion functions
LedRGB RGBtoLED(RGB rgb) {
  if(rgb.r > 1 || rgb.g > 1 || rgb.b > 1) {
    Serial.print("Exceeds expected RGB values");
  }
  else {
    LedRGB lrgb = {
      rgb.r*4000, rgb.g*4000, rgb.b*4000    };
    return lrgb;
  }
}

RGB HSVtoRGB(HSV hsv) {
  // algorithm from http://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB
  RGB rgb;
  RGB rgb_p;

  float chroma = hsv.v * hsv.s;
  float sector = float(hsv.h) / 60.0;
  // remainder is sector mod 2 in the real number sense
  float remainder = sector - ((int(sector) / 2) * 2) ;
  float x = chroma * (1 - abs(remainder - 1));
  switch(int(sector)) {
    case 0:
      rgb_p = (RGB){chroma, x, 0};
      break;
    case 1:
      rgb_p = (RGB){x, chroma, 0};
      break;
    case 2:
      rgb_p = (RGB){0, chroma, x};
      break;
    case 3:
      rgb_p = (RGB){0, x, chroma};
      break;
    case 4:
      rgb_p = (RGB){x, 0, chroma};
      break;
    case 5:
      rgb_p = (RGB){chroma, 0, x};
      break;
    default:
      rgb_p = (RGB){0, 0, 0};
  }

  float m = hsv.v - chroma;
  rgb = (RGB){rgb_p.r + m, rgb_p.g + m, rgb_p.b + m};

  if(debug){
    Serial.println("HSV:");
    Serial.println(hsv.h);
    Serial.println(hsv.s);
    Serial.println(hsv.v);
    Serial.println("RGB:");
    Serial.println(rgb.r);
    Serial.println(rgb.g);
    Serial.println(rgb.b);
    Serial.println("-----");
  }

  return rgb;
}


// pattern functions
void tester() {
  for (int j = 0; j < 3; j++) {
    int hue = j * 120;
    for (int n = 0; n < NUM_LEDS; n++) {
      Tlc.clear();
      HSV hsv = {hue, 1, .1};
      setColor(n, hsv);
      Tlc.update();
      delay(100);
    }
  }
}

void cyclingGradients() {
  int c = random(0,360);  
  int loopTime = random(2,30);
  for (int i = 0; i < NUM_LEDS; i++) {
      int n = i % 21;
      h++;
      Tlc.clear();
      for (int j = 0; j < 120; j++) {
        Tlc.clear();
        HSV hsv1 = {j + c - 5, 1, .5 };
        HSV hsv2 = {j + c - 10, 1, .5 };
        HSV hsv3 = {j + c - 15, 1, .5 };
        HSV hsv4 = {j + c - 20, 1, .5 };
        HSV hsv5 = {j + c - 25, 1, .5 };
        if (j > 5 && j < 100) setColor(n, hsv1);
        if (j > 10 && j < 105) setColor(n + 4, hsv2);
        if (j > 15 && j < 110) setColor(n + 10, hsv1);
        if (j > 20 && j < 115) setColor(n + 14, hsv2);
        if (j > 25 && j < 120) setColor(n + 18, hsv2);
        Tlc.update();
        delay(loopTime);  
      }
   }
}

void patches() {
    for (int n = 0; n < 5; n++) {
      int h = random(0,360);
      for (int m = 0; m < 50; m++) {
        Tlc.clear();
        HSV hsv = {m + h, 1, .5 };
        setColor(random(0,NUM_LEDS), hsv);
        setColor(random(0,NUM_LEDS), hsv);
        setColor(random(0,NUM_LEDS), hsv);   
        setColor(random(0,NUM_LEDS), hsv);
        setColor(random(0,NUM_LEDS), hsv);
        setColor(random(0,NUM_LEDS), hsv);       
        Tlc.update();
        delay(100);  
    }     
    Tlc.clear();
    Tlc.update();
  }
}
    
void randomPatches() {
  for (int i = 0; i < 25; i++) {
    int h = random(0,360);
    for (int n = 0; n < 5; n++) {
      HSV hsv = {h, 1, .5};
      setColor(random(0,NUM_LEDS), hsv);
      Tlc.update();
      delay(100);  
    }     
    Tlc.clear();
    Tlc.update();
  }
}

void doubleRainbow() {
  for(int h = 0; h < 720; h+=1) {
     HSV hsv = {h, 1, 1 };
     setAll(hsv, 100);
     Serial.println(h);
  }
}

void single() {
  int i = 0;
  for (int i = 0; i < 24; i++) {
    for(int n = 0; n < 360; n++) {
    Tlc.clear();
    i++;
    HSV hsv = {i, 1, 1 };
    setColor(random(0,NUM_LEDS), hsv);
    Tlc.update();
    delay(random(50,100));
    }  
  }
}    

// setup and loop
void setup() {
  Serial.begin(9600);
  Tlc.init();
}

void loop() {
  patches();
  single();
  randomPatches();
  cyclingGradients();
  //tester();
  //doubleRainbow();
}


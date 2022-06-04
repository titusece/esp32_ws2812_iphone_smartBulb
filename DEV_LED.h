
////////////////////////////////////
//   DEVICE-SPECIFIC LED SERVICES //
////////////////////////////////////

#include "extras/PwmPin.h"                          // library of various PWM functions

////////////////////////////////////


#include <FastLED.h>

#define LED_PIN     15
#define NUM_LEDS    16
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100
      
//////////////////////////////////

struct DEV_RgbLED : Service::LightBulb {       // RGB LED (Command Cathode)

  LedPin *redPin, *greenPin, *bluePin;
  
  SpanCharacteristic *power;                   // reference to the On Characteristic
  SpanCharacteristic *H;                       // reference to the Hue Characteristic
  SpanCharacteristic *S;                       // reference to the Saturation Characteristic
  SpanCharacteristic *V;                       // reference to the Brightness Characteristic
  
  DEV_RgbLED(int red_pin, int green_pin, int blue_pin) : Service::LightBulb(){       // constructor() method

    power=new Characteristic::On();                    
    H=new Characteristic::Hue(0);              // instantiate the Hue Characteristic with an initial value of 0 out of 360
    S=new Characteristic::Saturation(0);       // instantiate the Saturation Characteristic with an initial value of 0%
    V=new Characteristic::Brightness(100);     // instantiate the Brightness Characteristic with an initial value of 100%
    V->setRange(5,100,1);                      // sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
  } // end constructor

  boolean update(){                         // update() method

    boolean p;
    float v, h, s, r, g, b;

    h=H->getVal<float>();                      // get and store all current values.  Note the use of the <float> template to properly read the values
    s=S->getVal<float>();
    v=V->getVal<float>();                      // though H and S are defined as FLOAT in HAP, V (which is brightness) is defined as INT, but will be re-cast appropriately
    p=power->getVal();

    char cBuf[128];
    
    if(power->updated()){
      p=power->getNewVal();
      sprintf(cBuf,"Power=%s->%s, ",power->getVal()?"true":"false",p?"true":"false");
    } else {
      sprintf(cBuf,"Power=%s, ",p?"true":"false");
    }
    LOG1(cBuf);
      
    if(H->updated()){
      h=H->getNewVal<float>();
      sprintf(cBuf,"H=%.0f->%.0f, ",H->getVal<float>(),h);
    } else {
      sprintf(cBuf,"H=%.0f, ",h);
    }
    LOG1(cBuf);

    if(S->updated()){
      s=S->getNewVal<float>();
      sprintf(cBuf,"S=%.0f->%.0f, ",S->getVal<float>(),s);
    } else {
      sprintf(cBuf,"S=%.0f, ",s);
    }
    LOG1(cBuf);

    if(V->updated()){
      v=V->getNewVal<float>();
      sprintf(cBuf,"V=%.0f->%.0f  ",V->getVal<float>(),v);
    } else {
      sprintf(cBuf,"V=%.0f  ",v);
    }
    LOG1(cBuf);

    // Here we call a static function of LedPin that converts HSV to RGB.
    // Parameters must all be floats in range of H[0,360], S[0,1], and V[0,1]
    // R, G, B, returned [0,1] range as well

    LedPin::HSVtoRGB(h,s/100.0,v/100.0,&r,&g,&b);   // since HomeKit provides S and V in percent, scale down by 100

    int R, G, B;

    R=p*r*100;                                      // since LedPin uses percent, scale back up by 100, and multiple by status fo power (either 0 or 1)
    G=p*g*100;
    B=p*b*100;

    for(int i = 0; i < NUM_LEDS; i++) {   
      // let's set an led value
      leds[i] = CRGB(R, G, B);
      FastLED.delay(1);
    }
  
    sprintf(cBuf,"RGB=(%d,%d,%d)\n",R,G,B);
    LOG1(cBuf);

    //redPin->set(R);                      // update the ledPin channels with new values
    //greenPin->set(G);    
    //bluePin->set(B);    
      
    return(true);                               // return true
  
  } // update
};
      
//////////////////////////////////

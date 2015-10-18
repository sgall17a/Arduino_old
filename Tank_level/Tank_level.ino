#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP183.h>
#include <Streaming.h>
#include "U8glib.h"

// For hardware SPI:
// Connect SCK to SPI Clock, SDO to SPI MISO, and SDI to SPI MOSI
// See  http://arduino.cc/en/Reference/SPI for your Arduino's SPI pins!
// On UNO, Clock is #13, SDO/MISO is #12 and SDI/MOSI is #11

// You can also use software SPI and define your own pins!
#define BMP183_CLK  13
#define BMP183_SDO  12  // AKA MISO
#define BMP183_SDI  11  // AKA MOSI

// You'll also need a chip-select pin, use any pin!
#define TANK   9
#define AIR    10

// initialize with hardware SPI
//Adafruit_BMP183 bmp = Adafruit_BMP183(BMP183_CS);
// or initialize with software SPI and use any 4 pins
Adafruit_BMP183 bmp_t = Adafruit_BMP183(BMP183_CLK, BMP183_SDO, BMP183_SDI, TANK);
Adafruit_BMP183 bmp_a = Adafruit_BMP183(BMP183_CLK, BMP183_SDO, BMP183_SDI, AIR);
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) 
{
  Serial.begin(9600);
  Serial.println("BMP183 Pressure Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bmp_t.begin())
  {
    /* There was a problem detecting the BMP183 ... check your connections */
    Serial.print("Ooops, no Tank sensor detected ... Check your wiring!");
    while(1);
  }
    if(!bmp_a.begin())
  {
    /* There was a problem detecting the BMP183 ... check your connections */
    Serial.print("Ooops, no air sensor detected ... Check your wiring!");
    while(1);
  }
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
int n =0;
double sum = 0.0;
double avg;
//Difference between sensors = 14.7 pascals (tank > air);
double diff = 188;
//Pascals per cm water 
double cmh2o= 98.6;
double air;
double tank;
double tmp;
char *s = "          ";

void loop(void){
    //read("Tank",bmp_t);
    //read("Air",bmp_a);
    delay(1000);

    u8g.firstPage();  
  u8g.setFont(u8g_font_fub14);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
    u8g.setCursorPos(15,15);
  do {
   air = bmp_a.getPressure();
   tank = bmp_t.getPressure();
    u8g.drawStr(0,0,"Baro: ");
    sprintf(s,"%-05d",int(air/100));
    u8g.drawStr(60,0,s);  
    u8g.drawStr(0,20,"Temp:  ");
    tmp = bmp_a.getTemperature();
    sprintf(s,"%4d.%01d%",int(tmp),int((tmp-int(tmp))*10)) ;
    u8g.drawStr(60,20,s);
    double tank_height = (tank -air - diff) / cmh2o;
    sprintf(s,"%4d",int(tank_height));
    u8g.drawStr(0,40,"Tank:");
    u8g.drawStr(60,40,s);    
  }while( u8g.nextPage() );
}


